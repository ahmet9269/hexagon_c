/**
 * @file ExtrapTrackDataZeroMQOutgoingAdapter.cpp
 * @brief ZeroMQ UDP RADIO Outgoing Adapter implementation
 * @details Implements IAdapter lifecycle and message sending for ExtrapTrackData
 *          with background worker thread for non-blocking sends (~20ns enqueue)
 * 
 * @author a_hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @note DIP compliant - depends on IMessageSocket abstraction
 */

#include "adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.hpp"
#include "adapters/common/messaging/ZeroMQSocket.hpp"
#include "utils/Logger.hpp"

#include <iostream>
#include <sstream>
#include <chrono>

// Linux real-time scheduling headers
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

namespace adapters {
namespace outgoing {
namespace zeromq {

// ==================== DIP Compliant Constructor ====================

ExtrapTrackDataZeroMQOutgoingAdapter::ExtrapTrackDataZeroMQOutgoingAdapter(
    std::unique_ptr<adapters::common::messaging::IMessageSocket> socket)
    : endpoint_("")
    , group_(DEFAULT_GROUP)
    , socket_(std::move(socket))
    , running_(false)
    , ready_(false)
    , ownsSocket_(false) {  // Socket is already configured externally
    
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter created with injected socket (DIP)");
}

// ==================== Legacy Constructor ====================

ExtrapTrackDataZeroMQOutgoingAdapter::ExtrapTrackDataZeroMQOutgoingAdapter()
    : endpoint_("")
    , group_(DEFAULT_GROUP)
    , socket_(nullptr)
    , running_(false)
    , ready_(false)
    , ownsSocket_(true) {  // We own the socket, need to initialize it
    
    loadConfiguration();
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter created (legacy)");
}

ExtrapTrackDataZeroMQOutgoingAdapter::~ExtrapTrackDataZeroMQOutgoingAdapter() {
    stop();
    LOG_DEBUG("ExtrapTrackDataZeroMQOutgoingAdapter destroyed");
}

void ExtrapTrackDataZeroMQOutgoingAdapter::loadConfiguration() {
    // Load socket configuration from class constants
    endpoint_ = DEFAULT_ENDPOINT;
    group_ = DEFAULT_GROUP;
    
    LOG_DEBUG("Configuration loaded - endpoint: {}, group: {}", endpoint_, group_);
}

bool ExtrapTrackDataZeroMQOutgoingAdapter::initializeSocket() {
    // Only initialize if we own the socket (legacy constructor)
    if (!ownsSocket_) {
        // Socket was injected, just check if it's valid
        if (socket_ && socket_->isConnected()) {
            LOG_DEBUG("Using injected socket (DIP mode)");
            return true;
        }
        LOG_ERROR("Injected socket is null or not connected");
        return false;
    }
    
    try {
        // Create ZeroMQSocket with RADIO type (PUB for TCP fallback)
        auto zmqSocket = std::make_unique<adapters::common::messaging::ZeroMQSocket>(
            adapters::common::messaging::ZeroMQSocket::SocketType::PUB);  // Use PUB for TCP
        
        // Connect to endpoint
        if (!zmqSocket->connect(endpoint_, adapters::common::messaging::ZeroMQSocket::ConnectionMode::Connect)) {
            LOG_ERROR("Failed to connect to endpoint: {}", endpoint_);
            return false;
        }
        
        socket_ = std::move(zmqSocket);
        
        LOG_INFO("ZeroMQ socket initialized - endpoint: {}, group: {}", 
                 endpoint_, group_);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize ZeroMQ socket: {}", e.what());
        socket_.reset();
        return false;
    }
}

bool ExtrapTrackDataZeroMQOutgoingAdapter::start() {
    if (running_.load()) {
        LOG_WARN("ExtrapTrackDataZeroMQOutgoingAdapter already running");
        return true;
    }
    
    // For DIP mode, socket may already be connected
    if (!socket_ || !socket_->isConnected()) {
        if (!initializeSocket()) {
            LOG_ERROR("Failed to initialize socket, cannot start adapter");
            return false;
        }
    }
    
    running_.store(true);
    ready_.store(true);
    
    // Start background publisher thread
    publisherThread_ = std::thread([this]() {
        // Configure real-time thread priority on Linux
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = REALTIME_THREAD_PRIORITY;
        int result = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        if (result == 0) {
            LOG_DEBUG("Real-time priority set for outgoing: ExtrapTrackDataAdapter");
        }
        
        // Set CPU affinity to dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        publisherWorker();
    });
    
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter started with background worker");
    return true;
}

void ExtrapTrackDataZeroMQOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    LOG_INFO("Stopping ExtrapTrackDataZeroMQOutgoingAdapter...");
    
    running_.store(false);
    ready_.store(false);
    
    // Wake up the worker thread
    queueCV_.notify_all();
    
    if (publisherThread_.joinable()) {
        publisherThread_.join();
    }
    
    // Close socket
    if (socket_) {
        socket_->close();
    }
    
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter stopped");
}

bool ExtrapTrackDataZeroMQOutgoingAdapter::isRunning() const {
    return running_.load();
}

bool ExtrapTrackDataZeroMQOutgoingAdapter::isReady() const noexcept {
    return ready_.load() && running_.load();
}

std::string ExtrapTrackDataZeroMQOutgoingAdapter::getName() const noexcept {
    return "ExtrapTrackDataZeroMQOutgoingAdapter";
}

// ==================== Non-blocking Send (~20ns enqueue) ====================

void ExtrapTrackDataZeroMQOutgoingAdapter::sendExtrapTrackData(
    const std::vector<domain::model::ExtrapTrackData>& data) {
    
    if (!isReady()) {
        LOG_WARN("Cannot send: adapter not ready");
        return;
    }
    
    for (const auto& item : data) {
        enqueueMessage(item);
    }
}

void ExtrapTrackDataZeroMQOutgoingAdapter::sendExtrapTrackData(
    const domain::model::ExtrapTrackData& data) {
    
    if (!isReady()) {
        LOG_WARN("Cannot send: adapter not ready, dropping track: {}", data.getTrackId());
        return;
    }
    
    // Non-blocking enqueue (~20ns)
    enqueueMessage(data);
}

void ExtrapTrackDataZeroMQOutgoingAdapter::enqueueMessage(
    const domain::model::ExtrapTrackData& data) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        // Prevent unbounded queue growth
        if (messageQueue_.size() >= MAX_QUEUE_SIZE) {
            LOG_WARN("Message queue full, dropping oldest message");
            messageQueue_.pop();
        }
        
        messageQueue_.push(data);
    }
    
    queueCV_.notify_one();
}

// ==================== Background Worker Thread ====================

void ExtrapTrackDataZeroMQOutgoingAdapter::publisherWorker() {
    LOG_DEBUG("Publisher worker started: ExtrapTrackDataAdapter");
    
    while (running_.load()) {
        domain::model::ExtrapTrackData data;
        
        // Wait for message with timeout
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            if (queueCV_.wait_for(lock, std::chrono::milliseconds(100),
                [this]() { return !messageQueue_.empty() || !running_.load(); })) {
                
                if (!running_.load()) {
                    break;
                }
                
                if (!messageQueue_.empty()) {
                    data = std::move(messageQueue_.front());
                    messageQueue_.pop();
                } else {
                    continue;
                }
            } else {
                continue;  // Timeout, check running flag
            }
        }
        
        // Serialize and send (outside lock)
        try {
            std::vector<uint8_t> binaryData = data.serialize();
            
            if (binaryData.empty()) {
                LOG_ERROR("Empty payload for track ID: {}", data.getTrackId());
                continue;
            }
            
            // Send via IMessageSocket abstraction
            if (socket_->send(binaryData, group_)) {
                LOG_DEBUG("[a_hexagon] ExtrapTrackData sent - TrackID: {}, Size: {} bytes", 
                         data.getTrackId(), binaryData.size());
            } else {
                LOG_WARN("Failed to send ExtrapTrackData - TrackID: {}", data.getTrackId());
            }
            
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to send message: {}", e.what());
        }
    }
    
    LOG_DEBUG("Publisher worker stopped: ExtrapTrackDataAdapter");
}

} // namespace zeromq
} // namespace outgoing
} // namespace adapters
