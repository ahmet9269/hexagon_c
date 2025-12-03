/**
 * @file IMessageSocket.hpp
 * @brief Abstract interface for message socket operations
 * @details Provides abstraction layer over ZeroMQ or any other messaging library
 *          to enable dependency injection and unit testing without actual sockets.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <chrono>

namespace adapters {
namespace common {

/**
 * @brief Message receive result with metadata
 */
struct ReceivedMessage {
    std::vector<uint8_t> data;      ///< Message payload
    std::string group;               ///< Group name (for RADIO/DISH pattern)
    bool success;                    ///< True if message was received
    
    ReceivedMessage() : success(false) {}
    ReceivedMessage(std::vector<uint8_t> d, std::string g) 
        : data(std::move(d)), group(std::move(g)), success(true) {}
};

/**
 * @brief Abstract interface for message socket operations
 * @details Enables unit testing of adapters without actual network connections.
 *          Implementations can use ZeroMQ, Mock sockets, or any other backend.
 */
class IMessageSocket {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~IMessageSocket() = default;

    /**
     * @brief Bind socket to endpoint (for servers/listeners)
     * @param endpoint Address to bind to (e.g., "tcp://127.0.0.1:5555")
     * @return true if bound successfully
     */
    [[nodiscard]] virtual bool bind(const std::string& endpoint) = 0;

    /**
     * @brief Connect socket to endpoint (for clients/publishers)
     * @param endpoint Address to connect to
     * @return true if connected successfully
     */
    [[nodiscard]] virtual bool connect(const std::string& endpoint) = 0;

    /**
     * @brief Join a multicast group (for DISH pattern)
     * @param group Group name to subscribe to
     * @return true if joined successfully
     */
    [[nodiscard]] virtual bool join(const std::string& group) = 0;

    /**
     * @brief Send message with group (for RADIO pattern)
     * @param data Binary payload to send
     * @param group Group name for routing
     * @return true if sent successfully
     */
    [[nodiscard]] virtual bool send(const std::vector<uint8_t>& data, 
                                     const std::string& group) = 0;

    /**
     * @brief Receive message (non-blocking)
     * @param timeout_ms Maximum time to wait (0 = non-blocking)
     * @return ReceivedMessage with data if available
     */
    [[nodiscard]] virtual ReceivedMessage receive(int timeout_ms = 0) = 0;

    /**
     * @brief Close the socket
     */
    virtual void close() = 0;

    /**
     * @brief Check if socket is open and ready
     * @return true if ready for operations
     */
    [[nodiscard]] virtual bool isOpen() const = 0;

    /**
     * @brief Set socket option
     * @param option Option name
     * @param value Option value
     * @return true if set successfully
     */
    virtual bool setOption(const std::string& option, int value) = 0;

protected:
    IMessageSocket() = default;
    IMessageSocket(const IMessageSocket&) = default;
    IMessageSocket(IMessageSocket&&) = default;
    IMessageSocket& operator=(const IMessageSocket&) = default;
    IMessageSocket& operator=(IMessageSocket&&) = default;
};

/**
 * @brief Socket type enumeration
 */
enum class SocketType {
    RADIO,      ///< Publisher for RADIO/DISH pattern
    DISH,       ///< Subscriber for RADIO/DISH pattern
    PUB,        ///< Publisher for PUB/SUB pattern
    SUB,        ///< Subscriber for PUB/SUB pattern
    PUSH,       ///< Push side of PUSH/PULL
    PULL,       ///< Pull side of PUSH/PULL
    REQ,        ///< Request side of REQ/REP
    REP         ///< Reply side of REQ/REP
};

/**
 * @brief Factory interface for creating message sockets
 * @details Enables dependency injection of socket implementations
 */
class IMessageSocketFactory {
public:
    virtual ~IMessageSocketFactory() = default;
    
    /**
     * @brief Create a new socket of specified type
     * @param type Socket pattern type
     * @return Unique pointer to socket instance
     */
    [[nodiscard]] virtual std::unique_ptr<IMessageSocket> createSocket(SocketType type) = 0;
};

} // namespace common
} // namespace adapters
