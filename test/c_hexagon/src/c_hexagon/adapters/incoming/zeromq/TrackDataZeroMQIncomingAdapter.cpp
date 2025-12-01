/**
 * @file TrackDataZeroMQIncomingAdapter.cpp
 * @brief ZeroMQ DISH socket adapter for receiving DelayCalcTrackData from B_hexagon
 * @details Implements the primary adapter in hexagonal architecture for incoming track data
 */

#include "TrackDataZeroMQIncomingAdapter.hpp"
#include <iostream>
#include <zmq.hpp>

namespace adapters {
namespace incoming {
namespace zeromq {

// MISRA C++ 2023 compliant named constants
static constexpr int REALTIME_THREAD_PRIORITY = 95;
static constexpr int DEDICATED_CPU_CORE = 1;

/**
 * @brief Default constructor with standard multicast configuration
 * @param track_data_submission Domain port for forwarding received data
 */
TrackDataZeroMQIncomingAdapter::TrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission)
    : track_data_submission_(track_data_submission)
    , running_(false)
    , multicast_endpoint_("udp://udn;239.1.1.1:9002")
    , group_name_("DelayCalcTrackData")
    , zmq_context_(1)
    , dish_socket_(nullptr) {
    
    initializeDishSocket();
}

/**
 * @brief Custom configuration constructor
 * @param track_data_submission Domain port for forwarding received data
 * @param multicast_endpoint Custom UDP multicast endpoint (e.g., "udp://239.1.1.5:9595")
 * @param group_name Custom multicast group name for DISH subscription
 */
TrackDataZeroMQIncomingAdapter::TrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission,
    const std::string& multicast_endpoint,
    const std::string& group_name)
    : track_data_submission_(track_data_submission)
    , running_(false)
    , multicast_endpoint_(multicast_endpoint)
    , group_name_(group_name)
    , zmq_context_(1)
    , dish_socket_(nullptr) {
    
    initializeDishSocket();
}

/**
 * @brief Destructor - stops the subscriber thread and cleans up resources
 * @note RAII pattern handles ZeroMQ socket cleanup automatically
 */
TrackDataZeroMQIncomingAdapter::~TrackDataZeroMQIncomingAdapter() {
    stop();
}

/**
 * @brief Initializes the ZeroMQ DISH socket with performance optimizations
 * @throws zmq::error_t if socket initialization fails
 * @throws std::exception for other initialization errors
 */
void TrackDataZeroMQIncomingAdapter::initializeDishSocket() {
    try {
        std::cout << "🔧 YUSUF DISH Configuration:" << std::endl;
        std::cout << "   📡 Endpoint: " << multicast_endpoint_ << std::endl;
        std::cout << "   👥 Group: " << group_name_ << std::endl;
        
        // Create DISH socket using C++ wrapper - requires Draft API
        dish_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::dish);

        // Configure socket options for optimal performance
        dish_socket_->set(zmq::sockopt::rcvhwm, 0);       // Unlimited receive buffer
        dish_socket_->set(zmq::sockopt::rcvtimeo, 100);   // 100ms timeout for graceful shutdown
        dish_socket_->set(zmq::sockopt::linger, 0);       // No linger on close
        dish_socket_->set(zmq::sockopt::immediate, 1);    // Process messages immediately
        
        // Bind DISH socket to UDP multicast endpoint
        std::cout << "🔧 ZeroMQ DISH Configuration- dish_socket_bind" << std::endl;
        dish_socket_->bind(multicast_endpoint_);
        
        // Join the multicast group for message filtering
        std::cout << "🔧 ZeroMQ DISH Configuration- dish_socket_" << std::endl;
        dish_socket_->join(group_name_.c_str());

    } catch (const zmq::error_t& e) {
        std::cerr << "[DishSubscriber] ZMQ Initialize hatası: " << e.what() << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] Initialize hatası: " << e.what() << std::endl;
        throw;
    }
}

/**
 * @brief Starts the subscriber thread for asynchronous message reception
 * @return true if started successfully, false if already running
 * @note Configures real-time thread priority and CPU affinity on Linux
 */
bool TrackDataZeroMQIncomingAdapter::start() {
    if (running_.load()) {
        return false;
    }

    running_.store(true);

    // Start the subscriber worker thread
    subscriber_thread_ = std::thread([this]() {
        // Configure real-time thread priority on Linux
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = REALTIME_THREAD_PRIORITY;
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        
        // Set CPU affinity to dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        subscriberWorker();
    });

    return true;
}

/**
 * @brief Stops the subscriber thread gracefully
 * @note Waits for the worker thread to complete before returning
 */
void TrackDataZeroMQIncomingAdapter::stop() {
    running_.store(false);

    if (subscriber_thread_.joinable()) {
        subscriber_thread_.join();
    }
}

/**
 * @brief Checks if the subscriber is currently running
 * @return true if running, false otherwise
 */
bool TrackDataZeroMQIncomingAdapter::isRunning() const {
    return running_.load();
}

/**
 * @brief Main worker loop for receiving and processing messages
 * @details Continuously polls the DISH socket for incoming messages,
 *          deserializes them, and forwards valid data to the domain layer
 */
void TrackDataZeroMQIncomingAdapter::subscriberWorker() {
    while (running_.load()) {
        try {
            zmq::message_t received_msg;
            
            // Non-blocking receive with timeout for graceful shutdown
            auto result = dish_socket_->recv(received_msg, zmq::recv_flags::dontwait);
            
            if (!result.has_value() || received_msg.size() == 0U) {
                // No message available, brief sleep before retry
                std::this_thread::sleep_for(std::chrono::microseconds(10));
                continue;
            }

            // Record receive timestamp for latency calculation
            auto receive_time = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();

            // Convert binary message to vector
            const uint8_t* msg_data = static_cast<const uint8_t*>(received_msg.data());
            std::vector<uint8_t> binary_data(msg_data, msg_data + received_msg.size());

            // Deserialize the binary data into DelayCalcTrackData
            domain::ports::DelayCalcTrackData track_data;
            if (track_data.deserialize(binary_data)) {
                if (track_data.isValid() && track_data_submission_) {
                    // Calculate second hop latency
                    auto second_hop_latency_us = static_cast<int64_t>(receive_time - track_data.getSecondHopSentTime());
                    
                    // Forward to domain layer via hexagonal architecture port
                     // std::cout << "📡 DelayCalcTrackData alındı - Track ID: " << track_data.getTrackId() << std::endl;
                    //std::cout << "   🕐 İkinci Hop Gecikme: " << second_hop_latency_us << " μs" << std::endl;
                   // std::cout << "   🕐 Birinci Hop Gecikme: " << track_data.getFirstHopDelayTime() << " μs" << std::endl;
                    //std::cout << "   🕐 Toplam ZMQ Gecikme: " << (track_data.getFirstHopDelayTime() + second_hop_latency_us) << " μs" << std::endl;                    
                    // Domain katmanına gönder (Hexagonal Architecture)
                    track_data_submission_->submitDelayCalcTrackData(track_data);
                } else {
                    std::cerr << "[DishSubscriber] ❌ Invalid DelayCalcTrackData received" << std::endl;
                }
            } else {
                std::cerr << "[DishSubscriber] ❌ Failed to deserialize DelayCalcTrackData" << std::endl;
                std::cerr << "[DishSubscriber] Message size: " << received_msg.size() << " bytes" << std::endl;
            }

        } catch (const zmq::error_t& e) {
            if (e.num() != EAGAIN) {
                std::cerr << "[DishSubscriber] ZMQ Worker thread hatası: " << e.what() << std::endl;
            }
            // Brief sleep on error before retry
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        } catch (const std::exception& e) {
            std::cerr << "[DishSubscriber] Worker thread hatası: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

/**
 * @brief Deserializes binary data into DelayCalcTrackData object
 * @param binary_data Raw binary data received from ZeroMQ
 * @return Optional containing deserialized data if successful, nullopt otherwise
 * @note Uses the model's built-in binary deserialization with validation
 */
std::optional<domain::ports::DelayCalcTrackData> 
TrackDataZeroMQIncomingAdapter::deserializeDelayCalcTrackData(
    const std::vector<uint8_t>& binary_data) {
    
    try {
        domain::ports::DelayCalcTrackData data;
        
        if (data.deserialize(binary_data)) {
            if (data.isValid()) {
                std::cout << "[DishSubscriber] ✅ DelayCalcTrackData successfully deserialized and validated" << std::endl;
                std::cout << "[DishSubscriber] Track ID: " << data.getTrackId() 
                         << ", UpdateTime: " << data.getUpdateTime() << " μs" << std::endl;
                return data;
            } else {
                std::cerr << "[DishSubscriber] ❌ DelayCalcTrackData validation failed after deserialization" << std::endl;
                return std::nullopt;
            }
        } else {
            std::cerr << "[DishSubscriber] ❌ DelayCalcTrackData binary deserialization failed" << std::endl;
            std::cerr << "[DishSubscriber] Expected size: " << data.getSerializedSize() 
                     << " bytes, Received: " << binary_data.size() << " bytes" << std::endl;
            return std::nullopt;
        }

    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] ❌ Exception during DelayCalcTrackData deserialization: " << e.what() << std::endl;
        std::cerr << "[DishSubscriber] Binary data size: " << binary_data.size() << " bytes" << std::endl;
        return std::nullopt;
    }
}

} // namespace zeromq
} // namespace incoming
} // namespace adapters
