/**
 * @file ZeroMQSocket.cpp
 * @brief ZeroMQ concrete implementation of IMessageSocket
 * @details Thread-safe wrapper around zmq::socket_t with RAII resource management
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#include "ZeroMQSocket.hpp"
#include "utils/Logger.hpp"
#include "utils/Logger.hpp"

#include <cstring>
#include <stdexcept>

namespace adapters {
namespace common {
namespace messaging {

// ==================== Helper Function ====================

namespace {
    int socketTypeEnumToZmq(ZeroMQSocket::SocketType type) {
        switch (type) {
            case ZeroMQSocket::SocketType::SUB:   return ZMQ_SUB;
            case ZeroMQSocket::SocketType::PUB:   return ZMQ_PUB;
            case ZeroMQSocket::SocketType::DISH:  return ZMQ_DISH;
            case ZeroMQSocket::SocketType::RADIO: return ZMQ_RADIO;
            default: return ZMQ_PUB;
        }
    }
}

// ==================== SocketType Enum Constructor ====================

ZeroMQSocket::ZeroMQSocket(SocketType socketType, int ioThreads)
    : socketType_{socketTypeEnumToZmq(socketType)}
    , mode_{ConnectionMode::Connect}  // Default, can be changed via connect() overload
{
    try {
        context_ = std::make_unique<zmq::context_t>(ioThreads);
        LOG_DEBUG("ZeroMQSocket created - type: {}", getSocketTypeName());
    } catch (const zmq::error_t& e) {
        LOG_ERROR("Failed to create ZeroMQ context: {}", e.what());
        throw;
    }
}

// ==================== Legacy Int Constructor ====================

ZeroMQSocket::ZeroMQSocket(int socketType, ConnectionMode mode, int ioThreads)
    : socketType_{socketType}
    , mode_{mode}
{
    try {
        context_ = std::make_unique<zmq::context_t>(ioThreads);
        LOG_DEBUG("ZeroMQSocket created - type: {}, mode: {}", 
                  getSocketTypeName(),
                  (mode_ == ConnectionMode::Connect) ? "Connect" : "Bind");
    } catch (const zmq::error_t& e) {
        LOG_ERROR("Failed to create ZeroMQ context: {}", e.what());
        throw;
    }
}

ZeroMQSocket::~ZeroMQSocket() {
    close();
    LOG_DEBUG("ZeroMQSocket destroyed");
}

// ==================== Connect Methods ====================

bool ZeroMQSocket::connect(const std::string& endpoint) {
    return connect(endpoint, mode_);
}

bool ZeroMQSocket::connect(const std::string& endpoint, ConnectionMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_.load()) {
        LOG_WARN("ZeroMQSocket already connected to: {}", endpoint_);
        return true;
    }
    
    try {
        socket_ = std::make_unique<zmq::socket_t>(*context_, socketType_);
        
        // Set common socket options
        socket_->set(zmq::sockopt::linger, 0);
        
        // Connect or Bind based on mode
        if (mode == ConnectionMode::Connect) {
            socket_->connect(endpoint);
            LOG_INFO("ZeroMQ socket connected to: {}", endpoint);
        } else {
            socket_->bind(endpoint);
            LOG_INFO("ZeroMQ socket bound to: {}", endpoint);
        }
        
        endpoint_ = endpoint;
        mode_ = mode;
        connected_.store(true);
        return true;
        
    } catch (const zmq::error_t& e) {
        LOG_ERROR("ZeroMQ connect/bind failed: {} - {}", endpoint, e.what());
        socket_.reset();
        return false;
    }
}

bool ZeroMQSocket::send(const std::vector<uint8_t>& data) {
    return send(data, "");
}

bool ZeroMQSocket::send(const std::vector<uint8_t>& data, const std::string& group) {
    if (!connected_.load()) {
        LOG_WARN("Cannot send - socket not connected");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!socket_) {
        LOG_WARN("Cannot send - socket is null");
        return false;
    }
    
    try {
        zmq::message_t message(data.size());
        std::memcpy(message.data(), data.data(), data.size());
        
        // RADIO socket requires set_group for UDP multicast (Draft API)
        if (!group.empty() && (socketType_ == ZMQ_RADIO)) {
            message.set_group(group.c_str());
            LOG_TRACE("Set RADIO message group: {}", group);
        }
        
        auto result = socket_->send(message, zmq::send_flags::dontwait);
        
        if (!result.has_value()) {
            LOG_WARN("ZeroMQ send returned no result");
            return false;
        }
        
        LOG_TRACE("Sent {} bytes via ZeroMQ", data.size());
        return true;
        
    } catch (const zmq::error_t& e) {
        LOG_ERROR("ZeroMQ send failed: {}", e.what());
        return false;
    }
}

std::optional<std::vector<uint8_t>> ZeroMQSocket::receive(int32_t timeoutMs) {
    if (!connected_.load()) {
        LOG_WARN("Cannot receive - socket not connected");
        return std::nullopt;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!socket_) {
        LOG_WARN("Cannot receive - socket is null");
        return std::nullopt;
    }
    
    try {
        // Set receive timeout
        socket_->set(zmq::sockopt::rcvtimeo, timeoutMs);
        
        zmq::message_t message;
        auto result = socket_->recv(message, zmq::recv_flags::none);
        
        if (!result.has_value()) {
            // Timeout - normal condition, don't log
            return std::nullopt;
        }
        
        // Copy message data to vector
        std::vector<uint8_t> data(message.size());
        std::memcpy(data.data(), message.data(), message.size());
        
        LOG_TRACE("Received {} bytes via ZeroMQ", data.size());
        return data;
        
    } catch (const zmq::error_t& e) {
        if (e.num() == EAGAIN || e.num() == ETIMEDOUT) {
            // Timeout - normal condition
            return std::nullopt;
        }
        LOG_ERROR("ZeroMQ receive failed: {}", e.what());
        return std::nullopt;
    }
}

void ZeroMQSocket::close() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!connected_.load()) {
        return;
    }
    
    connected_.store(false);
    
    if (socket_) {
        try {
            socket_->close();
        } catch (...) {
            // Suppress exceptions in close
        }
        socket_.reset();
    }
    
    LOG_DEBUG("ZeroMQ socket closed");
}

bool ZeroMQSocket::isConnected() const noexcept {
    return connected_.load();
}

std::string ZeroMQSocket::getSocketType() const noexcept {
    return "ZeroMQ_" + getSocketTypeName();
}

void ZeroMQSocket::subscribe(const std::string& filter) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!socket_) {
        LOG_WARN("Cannot subscribe - socket not initialized");
        return;
    }
    
    if (socketType_ != ZMQ_SUB) {
        LOG_WARN("subscribe() only valid for SUB sockets");
        return;
    }
    
    try {
        socket_->set(zmq::sockopt::subscribe, filter);
        LOG_DEBUG("Subscribed to filter: '{}'", filter.empty() ? "(all)" : filter);
    } catch (const zmq::error_t& e) {
        LOG_ERROR("Failed to subscribe: {}", e.what());
    }
}

void ZeroMQSocket::joinGroup(const std::string& group) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!socket_) {
        LOG_WARN("Cannot join group - socket not initialized");
        return;
    }
    
    if (socketType_ != ZMQ_DISH) {
        LOG_WARN("joinGroup() only valid for DISH sockets");
        return;
    }
    
    try {
        // DISH socket uses zmq_join() for group membership (Draft API)
        socket_->join(group.c_str());
        LOG_DEBUG("Joined DISH group: '{}'", group);
    } catch (const zmq::error_t& e) {
        LOG_ERROR("Failed to join group '{}': {}", group, e.what());
    }
}

std::string ZeroMQSocket::getSocketTypeName() const noexcept {
    switch (socketType_) {
        case ZMQ_SUB:   return "SUB";
        case ZMQ_PUB:   return "PUB";
        case ZMQ_DISH:  return "DISH";
        case ZMQ_RADIO: return "RADIO";
        case ZMQ_PULL:  return "PULL";
        case ZMQ_PUSH:  return "PUSH";
        case ZMQ_REQ:   return "REQ";
        case ZMQ_REP:   return "REP";
        default:        return "UNKNOWN";
    }
}

} // namespace messaging
} // namespace common
} // namespace adapters
