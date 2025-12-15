/**
 * @file ExtrapTrackDataZeroMQIncomingAdapter.cpp
 * @brief ZeroMQ DISH adapter - inline implementation
 */

#include <zmq_config.hpp>
#include <zmq.hpp>
#include "ExtrapTrackDataZeroMQIncomingAdapter.hpp"
#include "../../../utils/Logger.hpp"
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <chrono>

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

using domain::ports::ExtrapTrackData;

// ====================Constructors ====================

ExtrapTrackDataZeroMQIncomingAdapter::ExtrapTrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver)
    : endpoint_(std::string(ZMQ_PROTOCOL) + "://" + ZMQ_MULTICAST_ADDRESS + ":" + std::to_string(ZMQ_PORT))
    , group_(ZMQ_GROUP)
    , adapterName_(std::string(ZMQ_GROUP) + "-InAdapter")
    , zmqContext_(1)
    , zmqSocket_(nullptr)
    , dataReceiver_(std::move(dataReceiver))
    , running_{false} {
    
    Logger::info("ExtrapTrackDataZeroMQIncomingAdapter created - endpoint: ", endpoint_, ", group: ", group_);
}

ExtrapTrackDataZeroMQIncomingAdapter::ExtrapTrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver,
    const std::string& endpoint,
    const std::string& group)
    : endpoint_(endpoint)
    , group_(group)
    , adapterName_(group + "-InAdapter")
    , zmqContext_(1)
    , zmqSocket_(nullptr)
    , dataReceiver_(std::move(dataReceiver))
    , running_{false} {
    
    Logger::info("ExtrapTrackDataZeroMQIncomingAdapter created (custom) - endpoint: ", endpoint_, ", group: ", group_);
}

ExtrapTrackDataZeroMQIncomingAdapter::~ExtrapTrackDataZeroMQIncomingAdapter() noexcept {
    stop();
    Logger::debug("ExtrapTrackDataZeroMQIncomingAdapter destroyed: ", adapterName_);
}

// ==================== IAdapter Interface ====================

bool ExtrapTrackDataZeroMQIncomingAdapter::start() {
    if (running_.load()) {
        Logger::warn("Adapter already running: ", adapterName_);
        return true;
    }
    
    if (!dataReceiver_) {
        Logger::error("Cannot start adapter: dataReceiver is null");
        return false;
    }
    
    try {
        // Create and configure ZeroMQ DISH socket for UDP multicast reception
        // DISH (Distributed Subscriber) pattern characteristics:
        // - Receives messages from RADIO publishers
        // - Supports group-based filtering (like topic subscription)
        // - UDP multicast for efficient one-to-many communication
        zmqSocket_ = std::make_unique<zmq::socket_t>(zmqContext_, ZMQ_DISH);
        
        // Set receive high water mark to prevent unbounded memory growth
        // If queue fills, oldest messages are dropped (prevents out-of-memory)
        zmqSocket_->set(zmq::sockopt::rcvhwm, 1000);
        
        // Bind to UDP multicast endpoint (e.g., "udp://239.1.1.2:9001")
        zmqSocket_->bind(endpoint_);
        
        // Join multicast group for message filtering
        // Only messages sent to this group will be received
        zmqSocket_->join(group_.c_str());
        
        Logger::info("DISH socket bound to: ", endpoint_, ", group: ", group_);
        
    } catch (const zmq::error_t& e) {
        Logger::error("Failed to setup DISH socket: ", e.what());
        return false;
    }
    
    running_.store(true);
    
    // Start dedicated worker thread
    workerThread_ = std::thread([this]() {
        process();
    });
    
    Logger::info("Adapter started: ", adapterName_);
    return true;
}

void ExtrapTrackDataZeroMQIncomingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    running_.store(false);
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
    
    if (zmqSocket_) {
        zmqSocket_->set(zmq::sockopt::linger, 0);
        zmqSocket_->close();
        zmqSocket_.reset();
    }
    
    Logger::info("Adapter stopped: ", adapterName_);
}

bool ExtrapTrackDataZeroMQIncomingAdapter::isRunning() const noexcept {
    return running_.load();
}

std::string ExtrapTrackDataZeroMQIncomingAdapter::getName() const noexcept {
    return adapterName_;
}

// ==================== Worker Thread ====================

void ExtrapTrackDataZeroMQIncomingAdapter::process() {
    Logger::debug("Worker thread started: ", adapterName_);
    
    // Main reception loop - continues until stop() is called
    while (running_.load()) {
        try {
            // Set receive timeout to allow periodic shutdown check
            // Without timeout, recv() would block indefinitely
            zmqSocket_->set(zmq::sockopt::rcvtimeo, RECEIVE_TIMEOUT_MS);
            
            // Receive message from ZeroMQ (blocking with timeout)
            zmq::message_t msg;
            auto result = zmqSocket_->recv(msg, zmq::recv_flags::none);
            
            // Check if message was actually received (timeout returns nullopt)
            if (!result || msg.size() == 0) {
                continue;  // Timeout or empty message - continue loop
            }
            
            // Deserialize binary payload to domain object
            auto data = deserializeBinary(static_cast<const uint8_t*>(msg.data()), msg.size());
            
            // Submit to domain layer for processing (via IExtrapTrackDataIncomingPort)
            // This call is non-blocking (~20ns) - data is queued for processing
            dataReceiver_->submitExtrapTrackData(data);
            
        } catch (const zmq::error_t& e) {
            // ZeroMQ errors - typically timeout (EAGAIN) during normal operation
            // Only log non-timeout errors to avoid spam
            if (e.num() != EAGAIN && running_.load()) {
                Logger::error("ZMQ receive error: ", e.what());
            }
        } catch (const std::exception& e) {
            // Deserialization or other errors - log but continue receiving
            Logger::error("Processing error: ", e.what());
        }
    }
    
    Logger::debug("Worker thread stopped: ", adapterName_);
}

// ==================== Deserialization ====================

ExtrapTrackData ExtrapTrackDataZeroMQIncomingAdapter::deserializeBinary(
    const uint8_t* data, std::size_t size) {
    
    // Convert raw pointer to vector for deserialize method
    std::vector<uint8_t> buffer(data, data + size);
    
    ExtrapTrackData trackData;
    
    // Attempt deserialization - validates size and data integrity
    // Expected size: 76 bytes (1x int32 + 6x double + 3x int64)
    if (!trackData.deserialize(buffer)) {
        // Size mismatch or corrupted data - throw exception
        std::string errorMsg = "Failed to deserialize ExtrapTrackData - received " + 
                               std::to_string(size) + " bytes, expected " + 
                               std::to_string(trackData.getSerializedSize()) + " bytes";
        throw std::runtime_error(errorMsg);
    }
    
    return trackData;
}
