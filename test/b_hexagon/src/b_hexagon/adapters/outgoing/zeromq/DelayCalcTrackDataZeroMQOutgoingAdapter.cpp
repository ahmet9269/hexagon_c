/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapter.cpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission
 * @details Thread-per-Type architecture compliant - thread-safe send operations
 *          SOLID compliant - uses IMessageSocket abstraction for testability
 */

#include "DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "adapters/common/messaging/ZeroMQSocket.hpp"
#include "../../../utils/Logger.hpp"
#include <sstream>
#include <cstring>
#include <stdexcept>

using domain::ports::DelayCalcTrackData;
using adapters::common::messaging::IMessageSocket;
using adapters::common::messaging::ZeroMQSocket;

// ==================== Constructors ====================

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter()
    : endpoint_(std::string(DEFAULT_PROTOCOL) + "://" + DEFAULT_ADDRESS + ":" + std::to_string(DEFAULT_PORT))
    , group_(DEFAULT_GROUP)
    , adapterName_(std::string(DEFAULT_GROUP) + "-OutAdapter")
    , socket_(nullptr)
    , running_{false} {
    
    socket_ = createDefaultSocket();
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created - endpoint: ", endpoint_, ", group: ", group_);
}

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter(
    const std::string& endpoint,
    const std::string& group)
    : endpoint_(endpoint)
    , group_(group)
    , adapterName_(group + "-OutAdapter")
    , socket_(nullptr)
    , running_{false} {
    
    socket_ = createDefaultSocket();
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created (custom) - endpoint: ", endpoint_, ", group: ", group_);
}

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter(
    std::unique_ptr<IMessageSocket> socket,
    const std::string& group)
    : endpoint_("injected")
    , group_(group)
    , adapterName_(group + "-OutAdapter")
    , socket_(std::move(socket))
    , running_{false} {
    
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created (injected socket) - group: ", group_);
}

DelayCalcTrackDataZeroMQOutgoingAdapter::~DelayCalcTrackDataZeroMQOutgoingAdapter() noexcept {
    stop();
    Logger::debug("DelayCalcTrackDataZeroMQOutgoingAdapter destroyed: ", adapterName_);
}

// ==================== Socket Factory ====================

std::unique_ptr<IMessageSocket> DelayCalcTrackDataZeroMQOutgoingAdapter::createDefaultSocket() {
    auto socket = std::make_unique<ZeroMQSocket>(ZeroMQSocket::SocketType::RADIO);
    
    if (!socket->connect(endpoint_)) {
        throw std::runtime_error("Failed to connect RADIO socket to: " + endpoint_);
    }
    
    Logger::debug("RADIO socket initialized via IMessageSocket - endpoint: ", endpoint_, ", group: ", group_);
    return socket;
}

// ==================== IAdapter Interface ====================

bool DelayCalcTrackDataZeroMQOutgoingAdapter::start() {
    if (running_.load()) {
        Logger::warn("Adapter already running: ", adapterName_);
        return true;
    }
    
    if (!socket_ || !socket_->isConnected()) {
        Logger::error("Cannot start adapter: socket not connected");
        return false;
    }
    
    running_.store(true);
    Logger::info("Adapter started: ", adapterName_);
    return true;
}

void DelayCalcTrackDataZeroMQOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    Logger::info("Stopping adapter: ", adapterName_);
    running_.store(false);
    
    if (socket_) {
        socket_->close();
    }
    
    Logger::info("Adapter stopped: ", adapterName_);
}

bool DelayCalcTrackDataZeroMQOutgoingAdapter::isRunning() const {
    return running_.load();
}

std::string DelayCalcTrackDataZeroMQOutgoingAdapter::getName() const noexcept {
    return adapterName_;
}

// ==================== Send Operation (Thread-Safe) ====================

void DelayCalcTrackDataZeroMQOutgoingAdapter::sendDelayCalcTrackData(const DelayCalcTrackData& data) {
    // Thread-safe send with mutex
    std::lock_guard<std::mutex> lock(sendMutex_);
    
    if (!running_.load()) {
        Logger::warn("Adapter not running, dropping message for track: ", data.getTrackId());
        return;
    }
    
    Logger::debug("Sending DelayCalcTrackData for track ID: ", data.getTrackId());
    
    // Validate input data
    if (!data.isValid()) {
        Logger::error("Invalid DelayCalcTrackData for track ID: ", data.getTrackId());
        throw std::invalid_argument("Invalid DelayCalcTrackData");
    }
    
    try {
        std::vector<uint8_t> binaryData = data.serialize();
        
        if (binaryData.empty()) {
            Logger::error("Empty payload for track ID: ", data.getTrackId());
            throw std::runtime_error("Empty binary payload");
        }
        
        // Send via IMessageSocket abstraction
        if (!socket_->send(binaryData, group_)) {
            throw std::runtime_error("Failed to send message");
        }
        
        Logger::debug("[", adapterName_, "] Sent TrackID: ", data.getTrackId(), ", Size: ", binaryData.size(), " bytes");
        
    } catch (const std::exception& e) {
        Logger::error("[", adapterName_, "] Send error: ", e.what());
        throw;
    }
}