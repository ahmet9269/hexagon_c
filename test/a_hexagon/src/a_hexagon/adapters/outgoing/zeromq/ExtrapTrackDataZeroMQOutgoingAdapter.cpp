/**
 * @file ExtrapTrackDataZeroMQOutgoingAdapter.cpp
 * @brief ZeroMQ UDP RADIO Outgoing Adapter implementation
 * @details Implements IAdapter lifecycle and message sending for ExtrapTrackData
 * 
 * @author a_hexagon Team
 * @version 1.1
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#include "adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.hpp"
#include "utils/Logger.hpp"

#include <iostream>
#include <sstream>

namespace adapters {
namespace outgoing {
namespace zeromq {

ExtrapTrackDataZeroMQOutgoingAdapter::ExtrapTrackDataZeroMQOutgoingAdapter()
    : protocol_("udp")
    , endpoint_("")
    , groupName_(DEFAULT_GROUP)
    , socketType_(ZMQ_RADIO)
    , context_(1)
    , socket_(nullptr)
    , running_(false) {
    
    loadConfiguration();
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter created");
}

ExtrapTrackDataZeroMQOutgoingAdapter::~ExtrapTrackDataZeroMQOutgoingAdapter() {
    stop();
    LOG_DEBUG("ExtrapTrackDataZeroMQOutgoingAdapter destroyed");
}

void ExtrapTrackDataZeroMQOutgoingAdapter::loadConfiguration() {
    // Default configuration
    
    // Original UDP multicast configuration (for production environment)
    // protocol_ = "udp";
    // socketType_ = ZMQ_RADIO;
    // endpoint_ = "udp://udn;239.1.1.1:9001";
    
    // TCP localhost configuration (for development/container environment)
    protocol_ = "tcp";
    socketType_ = ZMQ_RADIO;
    endpoint_ = "tcp://127.0.0.1:15001";
    groupName_ = DEFAULT_GROUP;
    
    LOG_DEBUG("Configuration loaded - endpoint: {}, group: {}", endpoint_, groupName_);
}

bool ExtrapTrackDataZeroMQOutgoingAdapter::initializeSocket() {
    try {
        socket_ = std::make_unique<zmq::socket_t>(context_, socketType_);
        
        // Configure socket options
        socket_->set(zmq::sockopt::linger, 0);
        
        // RADIO connects to DISH (b_hexagon binds)
        socket_->connect(endpoint_);
        
        LOG_INFO("ZeroMQ RADIO socket initialized - endpoint: {}, group: {}", 
                 endpoint_, groupName_);
        return true;
        
    } catch (const zmq::error_t& e) {
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
    
    if (!initializeSocket()) {
        LOG_ERROR("Failed to initialize socket, cannot start adapter");
        return false;
    }
    
    running_.store(true);
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter started successfully");
    return true;
}

void ExtrapTrackDataZeroMQOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    LOG_INFO("Stopping ExtrapTrackDataZeroMQOutgoingAdapter...");
    
    running_.store(false);
    
    // Close socket
    if (socket_) {
        try {
            socket_->close();
        } catch (const zmq::error_t& e) {
            LOG_WARN("Error closing socket: {}", e.what());
        }
        socket_.reset();
    }
    
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter stopped");
}

bool ExtrapTrackDataZeroMQOutgoingAdapter::isRunning() const {
    return running_.load();
}

std::string ExtrapTrackDataZeroMQOutgoingAdapter::getName() const noexcept {
    return "ExtrapTrackDataZeroMQOutgoingAdapter";
}

void ExtrapTrackDataZeroMQOutgoingAdapter::sendExtrapTrackData(
    const std::vector<domain::model::ExtrapTrackData>& data) {
    
    if (!running_.load() || !socket_) {
        LOG_WARN("Cannot send: adapter not running");
        return;
    }
    
    for (const auto& item : data) {
        sendExtrapTrackData(item);
    }
}

void ExtrapTrackDataZeroMQOutgoingAdapter::sendExtrapTrackData(
    const domain::model::ExtrapTrackData& data) {
    
    if (!running_.load() || !socket_) {
        LOG_WARN("Cannot send: adapter not running");
        return;
    }
    
    try {
        // Use model's binary serialization
        std::vector<uint8_t> binaryData = data.serialize();
        zmq::message_t message(binaryData.begin(), binaryData.end());
        
        // Set group for RADIO socket (required for RADIO/DISH pattern)
        message.set_group(groupName_.c_str());
        
        socket_->send(message, zmq::send_flags::none);
        
        LOG_INFO("[a_hexagon] ExtrapTrackData sent - TrackID: {}, Size: {} bytes", 
                 data.getTrackId(), binaryData.size());
        
    } catch (const zmq::error_t& e) {
        LOG_ERROR("Failed to send message: {}", e.what());
    }
}

} // namespace zeromq
} // namespace outgoing
} // namespace adapters
