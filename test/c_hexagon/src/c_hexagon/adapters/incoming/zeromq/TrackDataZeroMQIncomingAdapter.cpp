/**
 * @file TrackDataZeroMQIncomingAdapter.cpp
 * @brief ZeroMQ DISH socket adapter for receiving DelayCalcTrackData from B_hexagon
 * @details Implements the primary adapter in hexagonal architecture for incoming track data
 */

#include "TrackDataZeroMQIncomingAdapter.hpp"
#include "utils/Logger.hpp"
#include <zmq_config.hpp>  // Must be included before zmq.hpp for Draft API
#include <zmq.hpp>
#include <sstream>
#ifdef __linux__
#include <cerrno>
#include <cstring>
#endif

namespace adapters {
namespace incoming {
namespace zeromq {

namespace {
    /**
     * @brief Constructs UDP multicast endpoint string
     * @param address Multicast address (e.g., "239.1.1.5")
     * @param port Port number
     * @return Formatted endpoint string (e.g., "udp://239.1.1.5:9595")
     */
    std::string buildEndpoint(const char* address, int port) {
        std::ostringstream oss;
        oss << "udp://" << address << ":" << port;
        return oss.str();
    }
}

/**
 * @brief Default constructor with standard multicast configuration
 * @param track_data_submission Domain port for forwarding received data
 */
TrackDataZeroMQIncomingAdapter::TrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission)
    : track_data_submission_(track_data_submission)
    , endpoint_(buildEndpoint(DEFAULT_MULTICAST_ADDRESS, DEFAULT_PORT))
    , group_(DEFAULT_GROUP)
    , adapter_name_("DelayCalcTrackData-InAdapter")
    , zmq_context_(1)
    , dish_socket_(nullptr)
    , running_(false) {
    
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
    , endpoint_(multicast_endpoint)
    , group_(group_name)
    , adapter_name_(group_name + "-InAdapter")
    , zmq_context_(1)
    , dish_socket_(nullptr)
    , running_(false) {
    
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
        LOG_INFO("Socket Configuration - Endpoint: {}, Group: {}", endpoint_, group_);
        
        // RADIO/DISH pattern
        dish_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::dish);
        
        // Configure socket options for optimal performance
        dish_socket_->set(zmq::sockopt::rcvhwm, HIGH_WATER_MARK);
        dish_socket_->set(zmq::sockopt::rcvtimeo, RECEIVE_TIMEOUT_MS);
        dish_socket_->set(zmq::sockopt::linger, LINGER_MS);
        dish_socket_->set(zmq::sockopt::immediate, 1);    // Process messages immediately
        
        // DISH binds (listens for RADIO connections)
        LOG_DEBUG("Binding DISH socket to endpoint");
        dish_socket_->bind(endpoint_);
        
        // Join group for RADIO/DISH pattern
        LOG_DEBUG("Joining group: {}", group_);
        dish_socket_->join(group_.c_str());
        
        LOG_INFO("DISH socket initialized successfully");

    } catch (const zmq::error_t& e) {
        LOG_ERROR("ZMQ initialization error: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Initialization error: {}", e.what());
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
        int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        if (ret != 0) {
            LOG_DEBUG("RT scheduling not available (priority {}): {} - running with default scheduling", REALTIME_THREAD_PRIORITY, std::strerror(ret));
        } else {
            LOG_DEBUG("Incoming adapter thread RT priority set to {}", REALTIME_THREAD_PRIORITY);
        }

        // Set CPU affinity to dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        if (ret != 0) {
            LOG_DEBUG("CPU affinity not set (core {}): {} - running on any available core", DEDICATED_CPU_CORE, std::strerror(ret));
        } else {
            LOG_DEBUG("Incoming adapter thread pinned to CPU core {}", DEDICATED_CPU_CORE);
        }
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
 * @brief Returns the adapter's unique identifier
 * @return Adapter name string for logging and identification
 */
std::string TrackDataZeroMQIncomingAdapter::getName() const {
    return adapter_name_;
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
                    
                    LOG_INFO("[c_hexagon] DelayCalcTrackData received - TrackID: {}, Size: {} bytes",
                             track_data.getTrackId(), binary_data.size());
                    
                    // Log latency metrics (async, ~20ns overhead)
                    utils::Logger::logTrackReceived(
                        track_data.getTrackId(),
                        track_data.getFirstHopDelayTime(),
                        second_hop_latency_us);
                    
                    // Forward to domain layer via hexagonal architecture port
                    track_data_submission_->submitDelayCalcTrackData(track_data);
                } else {
                    LOG_WARN("Invalid DelayCalcTrackData received");
                }
            } else {
                LOG_ERROR("Failed to deserialize DelayCalcTrackData - Message size: {} bytes", received_msg.size());
            }

        } catch (const zmq::error_t& e) {
            if (e.num() != EAGAIN) {
                LOG_ERROR("ZMQ worker thread error: {}", e.what());
            }
            // Brief sleep on error before retry
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        } catch (const std::exception& e) {
            LOG_ERROR("Worker thread error: {}", e.what());
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
                LOG_DEBUG("DelayCalcTrackData deserialized - Track ID: {}, UpdateTime: {} μs",
                         data.getTrackId(), data.getUpdateTime());
                return data;
            } else {
                LOG_WARN("DelayCalcTrackData validation failed after deserialization");
                return std::nullopt;
            }
        } else {
            LOG_ERROR("DelayCalcTrackData deserialization failed - Expected: {} bytes, Received: {} bytes",
                     data.getSerializedSize(), binary_data.size());
            return std::nullopt;
        }

    } catch (const std::exception& e) {
        LOG_ERROR("Exception during deserialization: {} - Data size: {} bytes",
                 e.what(), binary_data.size());
        return std::nullopt;
    }
}

} // namespace zeromq
} // namespace incoming
} // namespace adapters
