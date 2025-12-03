/**
 * @file FinalCalcTrackDataZeroMQOutgoingAdapter.cpp
 * @brief Implementation of ZeroMQ RADIO socket adapter for FinalCalcTrackData
 * @details Implements the outgoing adapter for transmitting processed track data
 *          using ZeroMQ RADIO/DISH pattern for group-based messaging.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#include "FinalCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "utils/Logger.hpp"
#include <chrono>
#include <sstream>

namespace adapters {
namespace outgoing {
namespace zeromq {

namespace {
    /**
     * @brief Constructs UDP multicast endpoint string
     * @param address Multicast address (e.g., "239.1.1.5")
     * @param port Port number
     * @return Formatted endpoint string (e.g., "udp://239.1.1.5:9597")
     */
    std::string buildEndpoint(const char* address, int port) {
        std::ostringstream oss;
        oss << "udp://" << address << ":" << port;
        return oss.str();
    }
}

/**
 * @brief Default constructor with UDP multicast configuration
 */
FinalCalcTrackDataZeroMQOutgoingAdapter::FinalCalcTrackDataZeroMQOutgoingAdapter()
    : endpoint_(buildEndpoint(DEFAULT_MULTICAST_ADDRESS, DEFAULT_PORT))
    , group_(DEFAULT_GROUP)
    , adapter_name_("FinalCalcTrackData-OutAdapter")
    , zmq_context_(1)
    , radio_socket_(nullptr)
    , running_(false)
    , ready_(false) {
    
    initializeRadioSocket();
}

/**
 * @brief Custom configuration constructor
 * @param endpoint ZeroMQ endpoint for RADIO socket
 * @param group_name Group name for message routing
 */
FinalCalcTrackDataZeroMQOutgoingAdapter::FinalCalcTrackDataZeroMQOutgoingAdapter(
    const std::string& endpoint,
    const std::string& group_name)
    : endpoint_(endpoint)
    , group_(group_name)
    , adapter_name_(group_name + "-OutAdapter")
    , zmq_context_(1)
    , radio_socket_(nullptr)
    , running_(false)
    , ready_(false) {
    
    initializeRadioSocket();
}

/**
 * @brief Destructor - ensures graceful shutdown
 */
FinalCalcTrackDataZeroMQOutgoingAdapter::~FinalCalcTrackDataZeroMQOutgoingAdapter() {
    stop();
}

/**
 * @brief Initialize the ZeroMQ RADIO socket
 * @throws zmq::error_t if initialization fails
 */
void FinalCalcTrackDataZeroMQOutgoingAdapter::initializeRadioSocket() {
    try {
        LOG_INFO("Initializing RADIO socket - Endpoint: {}, Group: {}", 
                 endpoint_, group_);

        // Create RADIO socket (publisher for RADIO/DISH pattern)
        radio_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::radio);

        // Configure socket options for optimal performance
        radio_socket_->set(zmq::sockopt::sndhwm, HIGH_WATER_MARK);
        radio_socket_->set(zmq::sockopt::sndtimeo, SEND_TIMEOUT_MS);
        radio_socket_->set(zmq::sockopt::linger, LINGER_MS);
        radio_socket_->set(zmq::sockopt::immediate, 1);        // Process immediately

        // RADIO connects to endpoints (opposite of DISH which binds)
        LOG_DEBUG("Connecting RADIO socket to endpoint");
        radio_socket_->connect(endpoint_);

        ready_ = true;
        LOG_INFO("RADIO socket initialized successfully");

    } catch (const zmq::error_t& e) {
        LOG_ERROR("ZMQ RADIO socket initialization error: {}", e.what());
        ready_ = false;
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("RADIO socket initialization error: {}", e.what());
        ready_ = false;
        throw;
    }
}

/**
 * @brief Start the publisher thread
 * @return true if started successfully
 */
bool FinalCalcTrackDataZeroMQOutgoingAdapter::start() {
    if (running_.load()) {
        LOG_WARN("Adapter already running: {}", adapter_name_);
        return false;
    }

    running_ = true;

    // Start background publisher thread
    publisher_thread_ = std::thread([this]() {
        publisherWorker();
    });

    LOG_INFO("RADIO adapter started: {}", adapter_name_);
    return true;
}

/**
 * @brief Stop the publisher thread gracefully
 */
void FinalCalcTrackDataZeroMQOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }

    LOG_INFO("Stopping RADIO adapter: {}", adapter_name_);
    running_ = false;
    ready_ = false;

    // Wake up the worker thread
    queue_cv_.notify_all();

    if (publisher_thread_.joinable()) {
        publisher_thread_.join();
    }

    LOG_INFO("RADIO adapter stopped: {}", adapter_name_);
}

/**
 * @brief Check if adapter is running
 * @return true if running
 */
bool FinalCalcTrackDataZeroMQOutgoingAdapter::isRunning() const {
    return running_.load();
}

/**
 * @brief Get adapter name
 * @return Adapter identifier string
 */
std::string FinalCalcTrackDataZeroMQOutgoingAdapter::getName() const {
    return adapter_name_;
}

/**
 * @brief Check if ready to send
 * @return true if socket is ready
 */
bool FinalCalcTrackDataZeroMQOutgoingAdapter::isReady() const {
    return ready_.load() && running_.load();
}

/**
 * @brief Send FinalCalcTrackData (non-blocking)
 * @param data Track data to transmit
 * @details Queues message for background transmission
 */
void FinalCalcTrackDataZeroMQOutgoingAdapter::sendFinalTrackData(
    const domain::ports::FinalCalcTrackData& data) {
    
    if (!isReady()) {
        LOG_WARN("Adapter not ready, dropping message - Track ID: {}", data.getTrackId());
        return;
    }

    enqueueMessage(data);
}

/**
 * @brief Enqueue message for transmission
 * @param data Track data to queue
 */
void FinalCalcTrackDataZeroMQOutgoingAdapter::enqueueMessage(
    const domain::ports::FinalCalcTrackData& data) {
    
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        
        // Prevent unbounded queue growth
        if (message_queue_.size() >= MAX_QUEUE_SIZE) {
            LOG_WARN("Message queue full, dropping oldest message");
            message_queue_.pop();
        }
        
        message_queue_.push(data);
    }
    
    queue_cv_.notify_one();
}

/**
 * @brief Background worker for message transmission
 * @details Continuously processes queued messages and sends via ZMQ
 */
void FinalCalcTrackDataZeroMQOutgoingAdapter::publisherWorker() {
    LOG_DEBUG("Publisher worker started");

    while (running_.load()) {
        domain::ports::FinalCalcTrackData data;
        
        // Wait for message with timeout
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            if (queue_cv_.wait_for(lock, std::chrono::milliseconds(100),
                [this]() { return !message_queue_.empty() || !running_.load(); })) {
                
                if (!running_.load()) {
                    break;
                }
                
                if (!message_queue_.empty()) {
                    data = std::move(message_queue_.front());
                    message_queue_.pop();
                } else {
                    continue;
                }
            } else {
                continue;  // Timeout, check running flag
            }
        }

        // Serialize and send
        try {
            std::vector<uint8_t> serialized = data.serialize();
            
            // Create ZMQ message with group
            zmq::message_t msg(serialized.data(), serialized.size());
            msg.set_group(group_.c_str());
            
            auto result = radio_socket_->send(msg, zmq::send_flags::dontwait);
            
            if (result.has_value()) {
                LOG_DEBUG("[c_hexagon] FinalCalcTrackData sent - TrackID: {}, Size: {} bytes",
                         data.getTrackId(), serialized.size());
            } else {
                LOG_WARN("Failed to send FinalCalcTrackData - TrackID: {}", data.getTrackId());
            }

        } catch (const zmq::error_t& e) {
            LOG_ERROR("ZMQ send error: {}", e.what());
        } catch (const std::exception& e) {
            LOG_ERROR("Send error: {}", e.what());
        }
    }

    LOG_DEBUG("Publisher worker stopped");
}

} // namespace zeromq
} // namespace outgoing
} // namespace adapters
