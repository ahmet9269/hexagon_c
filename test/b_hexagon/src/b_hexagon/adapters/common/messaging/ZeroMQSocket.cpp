/**
 * @file ZeroMQSocket.cpp
 * @brief ZeroMQ socket implementation
 * @details Concrete implementation of IMessageSocket for ZeroMQ DISH/RADIO sockets
 */

#include "ZeroMQSocket.hpp"
#include "../../../utils/Logger.hpp"
#include <stdexcept>

namespace adapters {
namespace common {
namespace messaging {

// ==================== Constructor ====================

ZeroMQSocket::ZeroMQSocket(SocketType socketType, int ioThreads)
    : socketType_(socketType)
    , context_(ioThreads)
    , socket_(nullptr)
    , connected_{false}
    , endpoint_()
    , mutex_() {
    
    try {
        int zmqType = toZmqSocketType(socketType);
        socket_ = std::make_unique<zmq::socket_t>(context_, zmqType);
        configureSocketOptions();
        
        Logger::debug("ZeroMQSocket created - type: ", getSocketType());
        
    } catch (const zmq::error_t& e) {
        throw std::runtime_error("ZeroMQSocket construction failed: " + std::string(e.what()));
    }
}

ZeroMQSocket::~ZeroMQSocket() {
    close();
    Logger::debug("ZeroMQSocket destroyed - type: ", getSocketType());
}

// ==================== IMessageSocket Interface ====================

bool ZeroMQSocket::connect(const std::string& endpoint) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_.load()) {
        Logger::warn("ZeroMQSocket already connected to: ", endpoint_);
        return true;
    }
    
    try {
        if (socketType_ == SocketType::DISH) {
            // DISH sockets bind to receive multicast
            socket_->bind(endpoint);
            Logger::info("ZeroMQSocket DISH bound to: ", endpoint);
        } else {
            // RADIO sockets connect to send multicast
            socket_->connect(endpoint);
            Logger::info("ZeroMQSocket RADIO connected to: ", endpoint);
        }
        
        endpoint_ = endpoint;
        connected_.store(true);
        return true;
        
    } catch (const zmq::error_t& e) {
        Logger::error("ZeroMQSocket connect failed: ", e.what());
        return false;
    }
}

bool ZeroMQSocket::subscribe(const std::string& group) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (socketType_ != SocketType::DISH) {
        Logger::warn("Subscribe only valid for DISH sockets");
        return false;
    }
    
    try {
        // DISH uses join() instead of subscribe()
        socket_->join(group.c_str());
        Logger::debug("ZeroMQSocket joined group: ", group);
        return true;
        
    } catch (const zmq::error_t& e) {
        Logger::error("ZeroMQSocket subscribe failed: ", e.what());
        return false;
    }
}

bool ZeroMQSocket::send(const std::vector<uint8_t>& data) {
    return send(data, "");
}

bool ZeroMQSocket::send(const std::vector<uint8_t>& data, const std::string& group) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!connected_.load()) {
        Logger::warn("ZeroMQSocket not connected, cannot send");
        return false;
    }
    
    try {
        zmq::message_t msg(data.begin(), data.end());
        
        if (!group.empty() && socketType_ == SocketType::RADIO) {
            msg.set_group(group.c_str());
        }
        
        auto result = socket_->send(msg, zmq::send_flags::none);
        
        if (!result || *result != data.size()) {
            Logger::error("ZeroMQSocket partial send");
            return false;
        }
        
        Logger::debug("ZeroMQSocket sent ", *result, " bytes");
        return true;
        
    } catch (const zmq::error_t& e) {
        Logger::error("ZeroMQSocket send failed: ", e.what());
        return false;
    }
}

std::optional<std::vector<uint8_t>> ZeroMQSocket::receive(int32_t timeoutMs) {
    // IMPORTANT: This method is intentionally NOT protected by mutex.
    // Single-consumer pattern is REQUIRED - only one thread should call receive().
    // This is guaranteed by Thread-per-Type architecture where each incoming
    // adapter has a dedicated worker thread.
    // 
    // Rationale: ZeroMQ sockets are not thread-safe for concurrent recv() calls.
    // Adding mutex here would create lock contention with send() which is
    // unnecessary if the single-consumer contract is followed.
    
    if (!connected_.load()) {
        return std::nullopt;
    }
    
    try {
        // Set receive timeout
        socket_->set(zmq::sockopt::rcvtimeo, timeoutMs);
        
        zmq::message_t msg;
        auto result = socket_->recv(msg, zmq::recv_flags::none);
        
        if (!result || msg.size() == 0) {
            // Timeout or empty message
            return std::nullopt;
        }
        
        // Copy message data to vector
        std::vector<uint8_t> data(
            static_cast<uint8_t*>(msg.data()),
            static_cast<uint8_t*>(msg.data()) + msg.size()
        );
        
        Logger::debug("ZeroMQSocket received ", data.size(), " bytes");
        return data;
        
    } catch (const zmq::error_t& e) {
        if (e.num() != EAGAIN) {
            Logger::error("ZeroMQSocket receive failed: ", e.what());
        }
        return std::nullopt;
    }
}

void ZeroMQSocket::close() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!connected_.load()) {
        return;
    }
    
    try {
        if (socket_) {
            socket_->set(zmq::sockopt::linger, 0);
            socket_->close();
        }
        connected_.store(false);
        Logger::debug("ZeroMQSocket closed");
        
    } catch (...) {
        // Suppress exceptions in noexcept function
        connected_.store(false);
    }
}

bool ZeroMQSocket::isConnected() const noexcept {
    return connected_.load();
}

std::string ZeroMQSocket::getSocketType() const noexcept {
    switch (socketType_) {
        case SocketType::DISH:  return "ZeroMQ_DISH";
        case SocketType::RADIO: return "ZeroMQ_RADIO";
        default:                return "ZeroMQ_Unknown";
    }
}

// ==================== Private Methods ====================

int ZeroMQSocket::toZmqSocketType(SocketType type) noexcept {
    switch (type) {
        case SocketType::DISH:  return ZMQ_DISH;
        case SocketType::RADIO: return ZMQ_RADIO;
        default:                return ZMQ_DISH;
    }
}

void ZeroMQSocket::configureSocketOptions() {
    if (socketType_ == SocketType::DISH) {
        // DISH socket options
        socket_->set(zmq::sockopt::rcvhwm, 1000);   // Receive high water mark
    } else {
        // RADIO socket options
        socket_->set(zmq::sockopt::sndhwm, 0);      // Unlimited send buffer
        socket_->set(zmq::sockopt::linger, 0);      // No linger on close
    }
}

} // namespace messaging
} // namespace common
} // namespace adapters
