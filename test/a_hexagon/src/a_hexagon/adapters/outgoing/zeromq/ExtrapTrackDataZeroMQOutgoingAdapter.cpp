/**
 * @file ExtrapTrackDataZeroMQOutgoingAdapter.cpp
 * @brief ZeroMQ UDP RADIO Outgoing Adapter implementation
 * @details Implements IAdapter lifecycle and message sending for ExtrapTrackData
 * 
 * @author a_hexagon Team
 * @version 1.2
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
    , ownsSocket_(false) {  // Socket is already configured externally
    
    LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter created with injected socket (DIP)");
}

// ==================== Legacy Constructor ====================

ExtrapTrackDataZeroMQOutgoingAdapter::ExtrapTrackDataZeroMQOutgoingAdapter()
    : endpoint_("")
    , group_(DEFAULT_GROUP)
    , socket_(nullptr)
    , running_(false)
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
    if (socket_ && socket_->isConnected()) {
        running_.store(true);
        LOG_INFO("ExtrapTrackDataZeroMQOutgoingAdapter started with pre-connected socket");
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
        socket_->close();
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
        
        // Send via IMessageSocket abstraction
        if (socket_->send(binaryData, group_)) {
            LOG_INFO("[a_hexagon] ExtrapTrackData sent - TrackID: {}, Size: {} bytes", 
                     data.getTrackId(), binaryData.size());
        } else {
            LOG_ERROR("Failed to send ExtrapTrackData - TrackID: {}", data.getTrackId());
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to send message: {}", e.what());
    }
}

} // namespace zeromq
} // namespace outgoing
} // namespace adapters
