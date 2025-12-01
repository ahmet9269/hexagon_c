/**
 * @file TrackDataZeroMQIncomingAdapter.cpp
 * @brief ZeroMQ UDP DISH Incoming Adapter implementation
 * @details Implements the receive loop and message processing for TrackData
 *          with real-time thread priority and CPU affinity support
 * 
 * @author a_hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"
#include "utils/Logger.hpp"

#include <iostream>
#include <cstring>
#include <chrono>

// Linux real-time scheduling headers
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

namespace adapters {
namespace incoming {
namespace zeromq {

// MISRA C++ 2023 compliant named constants - Rule 9-3-1: Fixed-width integers
static constexpr int32_t REALTIME_THREAD_PRIORITY{95};
static constexpr int32_t DEDICATED_CPU_CORE{1};

TrackDataZeroMQIncomingAdapter::TrackDataZeroMQIncomingAdapter(
    domain::ports::incoming::ITrackDataIncomingPort* incomingPort)
    : protocol_("udp")
    , endpoint_("")
    , groupName_(DEFAULT_GROUP)
    , socketType_(ZMQ_DISH)
    , receiveTimeout_(DEFAULT_RECEIVE_TIMEOUT)
    , context_(1)
    , socket_(nullptr)
    , running_(false)
    , stopRequested_(false)
    , incomingPort_(incomingPort)
    , incomingPortShared_(nullptr) {
    
    loadConfiguration();
    LOG_INFO("TrackDataZeroMQIncomingAdapter created for endpoint: {}", endpoint_);
}

TrackDataZeroMQIncomingAdapter::TrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort)
    : protocol_("udp")
    , endpoint_("")
    , groupName_(DEFAULT_GROUP)
    , socketType_(ZMQ_DISH)
    , receiveTimeout_(DEFAULT_RECEIVE_TIMEOUT)
    , context_(1)
    , socket_(nullptr)
    , running_(false)
    , stopRequested_(false)
    , incomingPort_(incomingPort.get())
    , incomingPortShared_(std::move(incomingPort)) {
    
    loadConfiguration();
    LOG_INFO("TrackDataZeroMQIncomingAdapter created (shared_ptr) for endpoint: {}", endpoint_);
}

TrackDataZeroMQIncomingAdapter::~TrackDataZeroMQIncomingAdapter() {
    stop();
    LOG_DEBUG("TrackDataZeroMQIncomingAdapter destroyed");
}

void TrackDataZeroMQIncomingAdapter::loadConfiguration() {
    // Default configuration for DISH/SUB socket
    // TODO: Load from config file if available
    
    // Original UDP multicast configuration (for production environment)
    // protocol_ = "udp";
    // socketType_ = ZMQ_DISH;
    // endpoint_ = "udp://239.1.1.1:9000";  // TrackData multicast group
    
    // TCP localhost configuration (for development/container environment)
    // Note: For testing with Python PUB, use SUB socket
    protocol_ = "tcp";
    socketType_ = ZMQ_SUB;  // SUB for testing with Python PUB
    endpoint_ = "tcp://127.0.0.1:15000";  // TrackData TCP endpoint
    groupName_ = "TrackData";
    receiveTimeout_ = DEFAULT_RECEIVE_TIMEOUT;
    
    LOG_DEBUG("Configuration loaded - endpoint: {}, group: {}", endpoint_, groupName_);
}

bool TrackDataZeroMQIncomingAdapter::initializeSocket() {
    try {
        socket_ = std::make_unique<zmq::socket_t>(context_, socketType_);
        
        // Configure socket options
        socket_->set(zmq::sockopt::rcvtimeo, receiveTimeout_);
        socket_->set(zmq::sockopt::linger, 0);
        
        // SUB connects to PUB (test_publisher binds)
        socket_->connect(endpoint_);
        
        // Subscribe to all messages
        socket_->set(zmq::sockopt::subscribe, "");
        
        LOG_INFO("ZeroMQ SUB socket initialized - endpoint: {}", endpoint_);
        return true;
        
    } catch (const zmq::error_t& e) {
        LOG_ERROR("Failed to initialize ZeroMQ socket: {}", e.what());
        socket_.reset();
        return false;
    }
}

bool TrackDataZeroMQIncomingAdapter::start() {
    if (running_.load()) {
        LOG_WARN("TrackDataZeroMQIncomingAdapter already running");
        return true;
    }
    
    if (!incomingPort_) {
        LOG_ERROR("Cannot start adapter: incoming port is null");
        return false;
    }
    
    if (!initializeSocket()) {
        LOG_ERROR("Failed to initialize socket, cannot start adapter");
        return false;
    }
    
    stopRequested_.store(false);
    running_.store(true);
    
    // Start receive thread with real-time configuration
    receiveThread_ = std::thread([this]() {
        // Configure real-time thread priority on Linux
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = REALTIME_THREAD_PRIORITY;
        int result = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        if (result == 0) {
            LOG_INFO("Real-time thread priority set to {} (SCHED_FIFO)", REALTIME_THREAD_PRIORITY);
        } else {
            LOG_WARN("Failed to set real-time thread priority (requires root): {}", result);
        }
        
        // Set CPU affinity to dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        result = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        if (result == 0) {
            LOG_INFO("Thread affinity set to CPU core {}", DEDICATED_CPU_CORE);
        } else {
            LOG_WARN("Failed to set CPU affinity: {}", result);
        }
        #endif
        
        receiveLoop();
    });
    
    LOG_INFO("TrackDataZeroMQIncomingAdapter started successfully");
    return true;
}

void TrackDataZeroMQIncomingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    LOG_INFO("Stopping TrackDataZeroMQIncomingAdapter...");
    
    stopRequested_.store(true);
    running_.store(false);
    
    // Wait for receive thread to finish
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
    
    // Close socket
    if (socket_) {
        try {
            socket_->close();
        } catch (const zmq::error_t& e) {
            LOG_WARN("Error closing socket: {}", e.what());
        }
        socket_.reset();
    }
    
    LOG_INFO("TrackDataZeroMQIncomingAdapter stopped");
}

bool TrackDataZeroMQIncomingAdapter::isRunning() const {
    return running_.load();
}

std::string TrackDataZeroMQIncomingAdapter::getName() const noexcept {
    return "TrackDataZeroMQIncomingAdapter";
}

void TrackDataZeroMQIncomingAdapter::receiveLoop() {
    LOG_DEBUG("Receive loop started");
    
    while (!stopRequested_.load()) {
        try {
            zmq::message_t message;
            
            // Non-blocking receive with timeout
            auto result = socket_->recv(message, zmq::recv_flags::none);
            
            if (result.has_value() && result.value() > 0) {
                // Record receive timestamp for latency calculation
                auto receive_time = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                
                // Convert message to byte vector
                std::vector<uint8_t> data(
                    static_cast<const uint8_t*>(message.data()),
                    static_cast<const uint8_t*>(message.data()) + message.size()
                );
                
                // Deserialize using model's built-in deserialization
                domain::model::TrackData trackData;
                if (trackData.deserialize(data)) {
                    if (trackData.isValid() && incomingPort_) {
                        // Calculate latency
                        auto latency_us = receive_time - trackData.getOriginalUpdateTime();
                        
                        LOG_INFO("[a_hexagon] TrackData received - TrackID: {}, Size: {} bytes", 
                                 trackData.getTrackId(), data.size());
                        
                        // Log latency metrics (async, ~20ns overhead)
                        utils::Logger::logTrackReceived(trackData.getTrackId(), latency_us);
                        
                        // Forward to domain service via hexagonal architecture port
                        incomingPort_->processAndForwardTrackData(trackData);
                        
                        LOG_TRACE("Processed TrackData - ID: {}", trackData.getTrackId());
                    } else {
                        LOG_WARN("Invalid TrackData received or incoming port null");
                    }
                } else {
                    LOG_WARN("Failed to deserialize TrackData message ({} bytes)", data.size());
                }
            }
            // Timeout is normal, just continue loop
            
        } catch (const zmq::error_t& e) {
            if (!stopRequested_.load()) {
                LOG_ERROR("ZeroMQ receive error: {}", e.what());
            }
            // Brief sleep on error before retry
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        } catch (const std::exception& e) {
            LOG_ERROR("Worker thread error: {}", e.what());
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
    
    LOG_DEBUG("Receive loop ended");
}

bool TrackDataZeroMQIncomingAdapter::deserializeTrackData(
    const std::vector<uint8_t>& data,
    domain::model::TrackData& trackData) {
    
    // Binary deserialization format:
    // [TrackId: int32_t]
    // [XVelocityECEF: double]
    // [YVelocityECEF: double]
    // [ZVelocityECEF: double]
    // [XPositionECEF: double]
    // [YPositionECEF: double]
    // [ZPositionECEF: double]
    // [OriginalUpdateTime: int64_t]
    
    constexpr size_t EXPECTED_SIZE = sizeof(int32_t) + 
                                      (6 * sizeof(double)) + 
                                      sizeof(int64_t);
    
    if (data.size() < EXPECTED_SIZE) {
        LOG_DEBUG("Invalid data size: {} (expected {})", data.size(), EXPECTED_SIZE);
        return false;
    }
    
    try {
        size_t offset = 0;
        
        // TrackId
        int32_t trackId;
        std::memcpy(&trackId, data.data() + offset, sizeof(int32_t));
        offset += sizeof(int32_t);
        trackData.setTrackId(trackId);
        
        // Velocities
        double xVel, yVel, zVel;
        std::memcpy(&xVel, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&yVel, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&zVel, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        trackData.setXVelocityECEF(xVel);
        trackData.setYVelocityECEF(yVel);
        trackData.setZVelocityECEF(zVel);
        
        // Positions
        double xPos, yPos, zPos;
        std::memcpy(&xPos, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&yPos, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&zPos, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        trackData.setXPositionECEF(xPos);
        trackData.setYPositionECEF(yPos);
        trackData.setZPositionECEF(zPos);
        
        // Timestamp
        int64_t timestamp;
        std::memcpy(&timestamp, data.data() + offset, sizeof(int64_t));
        trackData.setOriginalUpdateTime(static_cast<long>(timestamp));
        
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Deserialization error: {}", e.what());
        return false;
    }
}

} // namespace zeromq
} // namespace incoming
} // namespace adapters
