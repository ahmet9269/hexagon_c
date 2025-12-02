/**
 * @file ZeroMQSocket.hpp
 * @brief ZeroMQ concrete implementation of IMessageSocket
 * @details Wraps ZeroMQ socket operations behind the IMessageSocket abstraction.
 *          Supports SUB/PUB, DISH/RADIO socket types for UDP and TCP protocols.
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IMessageSocket.hpp
 */

#ifndef A_HEXAGON_ADAPTERS_COMMON_MESSAGING_ZEROMQ_SOCKET_HPP
#define A_HEXAGON_ADAPTERS_COMMON_MESSAGING_ZEROMQ_SOCKET_HPP

#include "IMessageSocket.hpp"

// ZMQ Draft API is enabled via -DZMQ_BUILD_DRAFT_API compiler flag in Makefile
#include <zmq.hpp>
#include <memory>
#include <atomic>
#include <string>
#include <mutex>

// ZMQ_DISH/ZMQ_RADIO draft API fallback
#ifndef ZMQ_DISH
#define ZMQ_DISH 15
#endif
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif

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
 * - ZMQ_SUB: Subscribe socket (connects to PUB)
 * - ZMQ_PUB: Publish socket (binds, others connect)
 * - ZMQ_DISH: UDP multicast subscribe (Draft API)
 * - ZMQ_RADIO: UDP multicast publish (Draft API)
 * 
 * @invariant context_ is always valid after construction
 * @invariant socket_ is null until connect() succeeds
 */
class ZeroMQSocket : public IMessageSocket {
public:
    /**
     * @brief Socket type enumeration for type-safe construction
     */
    enum class SocketType {
        SUB,    ///< Subscribe socket (TCP/UDP)
        PUB,    ///< Publish socket (TCP/UDP)
        DISH,   ///< UDP multicast subscribe (Draft API)
        RADIO   ///< UDP multicast publish (Draft API)
    };

    /**
     * @brief Connection mode for socket
     */
    enum class ConnectionMode {
        Connect,  ///< Client mode (connect to endpoint)
        Bind      ///< Server mode (bind to endpoint)
    };

    /**
     * @brief Construct ZeroMQ socket with specified type (enum version - preferred)
     * @param socketType Socket type enum
     * @param ioThreads Number of IO threads (default: 1)
     */
    explicit ZeroMQSocket(SocketType socketType, int ioThreads = 1);

    /**
     * @brief Construct ZeroMQ socket with specified type (int version - legacy)
     * @param socketType ZeroMQ socket type (ZMQ_SUB, ZMQ_PUB, ZMQ_DISH, ZMQ_RADIO)
     * @param mode Connection mode (Connect or Bind)
     * @param ioThreads Number of IO threads (default: 1)
     */
    explicit ZeroMQSocket(int socketType, 
                          ConnectionMode mode = ConnectionMode::Connect,
                          int ioThreads = 1);

    /**
     * @brief Destructor - closes socket and releases resources
     */
    ~ZeroMQSocket() override;

    // ==================== IMessageSocket Interface ====================

    /**
     * @brief Connect or bind to ZeroMQ endpoint
     * @param endpoint ZeroMQ endpoint (e.g., "tcp://localhost:5000")
     * @return true on success, false on failure
     */
    [[nodiscard]] bool connect(const std::string& endpoint) override;

    /**
     * @brief Send binary message
     * @param data Raw bytes to send
     * @return true on success, false on failure
     */
    [[nodiscard]] bool send(const std::vector<uint8_t>& data) override;

    /**
     * @brief Send binary message with group
     * @param data Raw bytes to send
     * @param group Group name (for RADIO socket)
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
     * @brief Close socket
     */
    void close() noexcept override;

    /**
     * @brief Check connection status
     * @return true if connected
     */
    [[nodiscard]] bool isConnected() const noexcept override;

    /**
     * @brief Get socket type string
     * @return Socket type identifier
     */
    [[nodiscard]] std::string getSocketType() const noexcept override;

    // ==================== ZeroMQ-Specific Methods ====================

    /**
     * @brief Connect with explicit connection mode
     * @param endpoint ZeroMQ endpoint
     * @param mode Connection mode (Connect or Bind)
     * @return true on success, false on failure
     */
    [[nodiscard]] bool connect(const std::string& endpoint, ConnectionMode mode);

    /**
     * @brief Subscribe to messages (for SUB socket)
     * @param filter Subscription filter (empty = all messages)
     */
    void subscribe(const std::string& filter = "");

    /**
     * @brief Join group (for DISH socket)
     * @param group Group name to join
     */
    void joinGroup(const std::string& group);

    // Delete copy/move for thread safety
    ZeroMQSocket(const ZeroMQSocket&) = delete;
    ZeroMQSocket& operator=(const ZeroMQSocket&) = delete;
    ZeroMQSocket(ZeroMQSocket&&) = delete;
    ZeroMQSocket& operator=(ZeroMQSocket&&) = delete;

private:
    /**
     * @brief Get socket type name
     * @return Human-readable socket type
     */
    [[nodiscard]] std::string getSocketTypeName() const noexcept;

    std::unique_ptr<zmq::context_t> context_;   ///< ZeroMQ context
    std::unique_ptr<zmq::socket_t> socket_;     ///< ZeroMQ socket
    int socketType_;                             ///< Socket type (ZMQ_SUB, ZMQ_PUB, etc.)
    ConnectionMode mode_;                        ///< Connect or Bind mode
    std::atomic<bool> connected_{false};         ///< Connection state
    std::string endpoint_;                       ///< Current endpoint
    mutable std::mutex mutex_;                   ///< Thread safety mutex
};

} // namespace messaging
} // namespace common
} // namespace adapters

#endif // A_HEXAGON_ADAPTERS_COMMON_MESSAGING_ZEROMQ_SOCKET_HPP
