/**
 * @file ZeroMQSocket.hpp
 * @brief ZeroMQ concrete implementation of IMessageSocket
 * @details Wraps ZeroMQ socket operations behind the IMessageSocket abstraction.
 *          Supports DISH/RADIO socket types for UDP multicast protocols.
 * 
 * SOLID Compliance:
 * - Single Responsibility: Only handles ZeroMQ socket operations
 * - Open/Closed: Extends IMessageSocket, closed for modification
 * - Liskov Substitution: Can replace any IMessageSocket
 * - Interface Segregation: Implements focused IMessageSocket interface
 * - Dependency Inversion: High-level adapters depend on IMessageSocket abstraction
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IMessageSocket.hpp
 */

#ifndef B_HEXAGON_ADAPTERS_COMMON_MESSAGING_ZEROMQ_SOCKET_HPP
#define B_HEXAGON_ADAPTERS_COMMON_MESSAGING_ZEROMQ_SOCKET_HPP

#include "IMessageSocket.hpp"
#include <zmq_config.hpp>
#include <zmq.hpp>
#include <memory>
#include <atomic>
#include <string>
#include <mutex>

namespace adapters {
namespace common {
namespace messaging {

/**
 * @class ZeroMQSocket
 * @brief ZeroMQ implementation of IMessageSocket interface
 * @details Provides thread-safe wrapper around zmq::socket_t with
 *          proper resource management via RAII.
 * 
 * Supported socket types:
 * - ZMQ_DISH: UDP multicast subscribe (Draft API)
 * - ZMQ_RADIO: UDP multicast publish (Draft API)
 * 
 * @invariant context_ is always valid after construction
 * @invariant socket_ is null until connect() succeeds
 */
class ZeroMQSocket final : public IMessageSocket {
public:
    /**
     * @brief Socket type enumeration for type-safe construction
     */
    enum class SocketType {
        DISH,   ///< UDP multicast subscribe (binds to endpoint)
        RADIO   ///< UDP multicast publish (connects to endpoint)
    };

    /**
     * @brief Construct ZeroMQ socket with specified type
     * @param socketType Socket type enum (DISH or RADIO)
     * @param ioThreads Number of IO threads (default: 1)
     */
    explicit ZeroMQSocket(SocketType socketType, int ioThreads = 1);

    /**
     * @brief Destructor - closes socket and releases resources
     */
    ~ZeroMQSocket() override;

    // Delete copy operations (ZeroMQ sockets are not copyable)
    ZeroMQSocket(const ZeroMQSocket&) = delete;
    ZeroMQSocket& operator=(const ZeroMQSocket&) = delete;

    // Delete move operations for thread safety
    ZeroMQSocket(ZeroMQSocket&&) = delete;
    ZeroMQSocket& operator=(ZeroMQSocket&&) = delete;

    // ==================== IMessageSocket Interface ====================

    /**
     * @brief Connect or bind to ZeroMQ endpoint
     * @param endpoint ZeroMQ endpoint (e.g., "udp://239.1.1.2:9001")
     * @return true on success, false on failure
     * @note DISH sockets bind, RADIO sockets connect
     */
    [[nodiscard]] bool connect(const std::string& endpoint) override;

    /**
     * @brief Subscribe to a group (DISH socket only)
     * @param group Group name to subscribe to
     * @return true on success, false on failure
     */
    [[nodiscard]] bool subscribe(const std::string& group) override;

    /**
     * @brief Send binary message (no group)
     * @param data Raw bytes to send
     * @return true on success, false on failure
     */
    [[nodiscard]] bool send(const std::vector<uint8_t>& data) override;

    /**
     * @brief Send binary message with group (RADIO socket)
     * @param data Raw bytes to send
     * @param group Group name for the message
     * @return true on success, false on failure
     */
    [[nodiscard]] bool send(const std::vector<uint8_t>& data, const std::string& group) override;

    /**
     * @brief Receive binary message with timeout
     * @param timeoutMs Timeout in milliseconds
     * @return Received data or nullopt on timeout/error
     */
    [[nodiscard]] std::optional<std::vector<uint8_t>> receive(int32_t timeoutMs) override;

    /**
     * @brief Close socket and release resources
     */
    void close() noexcept override;

    /**
     * @brief Check if socket is connected
     * @return true if connected
     */
    [[nodiscard]] bool isConnected() const noexcept override;

    /**
     * @brief Get socket type string
     * @return "ZeroMQ_DISH" or "ZeroMQ_RADIO"
     */
    [[nodiscard]] std::string getSocketType() const noexcept override;

private:
    /**
     * @brief Convert enum to ZMQ socket type constant
     */
    static int toZmqSocketType(SocketType type) noexcept;

    /**
     * @brief Configure socket options based on type
     */
    void configureSocketOptions();

private:
    SocketType socketType_;                         ///< Socket type (DISH/RADIO)
    zmq::context_t context_;                        ///< ZeroMQ context
    std::unique_ptr<zmq::socket_t> socket_;         ///< ZeroMQ socket
    std::atomic<bool> connected_{false};            ///< Connection state
    std::string endpoint_;                          ///< Connected endpoint
    mutable std::mutex mutex_;                      ///< Thread safety mutex
};

} // namespace messaging
} // namespace common
} // namespace adapters

#endif // B_HEXAGON_ADAPTERS_COMMON_MESSAGING_ZEROMQ_SOCKET_HPP
