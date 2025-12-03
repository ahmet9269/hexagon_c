/**
 * @file IMessageSocket.hpp
 * @brief Abstract messaging socket interface for Dependency Inversion Principle
 * @details Provides abstraction layer for messaging systems.
 *          Designed for ZeroMQ RADIO/DISH pattern (UDP multicast with group filtering).
 *          Enables dependency injection and mock testing without real network connections.
 * 
 * ZeroMQ Pattern Reference:
 * - RADIO socket: Sends messages with group identifier (UDP multicast publisher)
 * - DISH socket: Receives messages filtered by group (UDP multicast subscriber)
 * - Note: This is NOT PUB/SUB pattern. RADIO/DISH uses UDP and group-based filtering.
 * 
 * Design Rationale (SOLID - Dependency Inversion Principle):
 * ┌─────────────────────────────────────────────────────────────────────┐
 * │                    DEPENDENCY INVERSION PRINCIPLE                   │
 * ├─────────────────────────────────────────────────────────────────────┤
 * │                                                                     │
 * │  ┌─────────────────┐         ┌─────────────────────────────────┐   │
 * │  │     Adapter     │ ──────▶ │      IMessageSocket             │   │
 * │  │  (High Level)   │         │      (Abstraction)              │   │
 * │  └─────────────────┘         └─────────────────────────────────┘   │
 * │                                         ▲                          │
 * │                              ┌──────────┴──────────┐               │
 * │                              │                     │               │
 * │                    ┌─────────────────┐   ┌─────────────────┐       │
 * │                    │  ZeroMQSocket   │   │ MockMessageSocket│      │
 * │                    │   (Concrete)    │   │    (Test)       │       │
 * │                    └─────────────────┘   └─────────────────┘       │
 * │                                                                     │
 * └─────────────────────────────────────────────────────────────────────┘
 * 
 * Benefits:
 * - Unit tests run without network dependencies
 * - Easy to swap messaging implementations (ZeroMQ → Kafka)
 * - High test coverage with mock injection
 * - MISRA C++ 2023 compliant
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @see ZeroMQSocket.hpp
 * @see MockMessageSocket.hpp (for testing)
 */

#ifndef B_HEXAGON_ADAPTERS_COMMON_MESSAGING_IMESSAGE_SOCKET_HPP
#define B_HEXAGON_ADAPTERS_COMMON_MESSAGING_IMESSAGE_SOCKET_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include <string>

namespace adapters {
namespace common {
namespace messaging {

/**
 * @class IMessageSocket
 * @brief Abstract interface for messaging socket operations
 * @details Defines the contract for all messaging implementations.
 *          Enables unit testing via mock injection without network dependencies.
 * 
 * Usage:
 * @code
 * // Production code:
 * auto socket = std::make_unique<ZeroMQSocket>(ZeroMQSocket::SocketType::DISH);
 * adapter = std::make_unique<IncomingAdapter>(port, std::move(socket));
 * 
 * // Test code:
 * auto mockSocket = std::make_unique<MockMessageSocket>();
 * mockSocket->queueIncomingMessage(testData);
 * adapter = std::make_unique<IncomingAdapter>(mockPort, std::move(mockSocket));
 * @endcode
 * 
 * @invariant isConnected() returns true only after successful connect()
 * @invariant send() and receive() require isConnected() == true
 */
class IMessageSocket {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     * @note MISRA Rule 12-8-1: Base classes shall have virtual destructor
     */
    virtual ~IMessageSocket() = default;

    /**
     * @brief Connect or bind to messaging endpoint
     * @param endpoint URI format endpoint (e.g., "tcp://localhost:5000", "udp://239.1.1.1:9000")
     * @return true on successful connection, false on failure
     * @pre endpoint is non-empty and valid URI format
     * @post isConnected() returns true if successful
     * @thread_safe No - call before starting multi-threaded operations
     */
    [[nodiscard]] virtual bool connect(const std::string& endpoint) = 0;

    /**
     * @brief Join a message group (for DISH socket - group-based filtering)
     * @param group Group name to join/subscribe (max 16 characters in ZeroMQ DISH)
     * @return true on success, false on failure
     * @pre isConnected() == true for some implementations
     * @note For ZeroMQ DISH socket, this calls zmq_join() internally
     * @note Method named "subscribe" for interface consistency, but internally uses zmq_join()
     */
    [[nodiscard]] virtual bool subscribe(const std::string& group) = 0;

    /**
     * @brief Send binary message
     * @param data Raw bytes to send
     * @return true on successful send, false on failure
     * @pre isConnected() == true
     * @thread_safe Implementation-dependent (ZeroMQ: requires external sync)
     */
    [[nodiscard]] virtual bool send(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Send binary message with group identifier
     * @param data Raw bytes to send
     * @param group Group name for RADIO/DISH pattern (max 16 characters in ZeroMQ)
     * @return true on successful send, false on failure
     * @pre isConnected() == true
     * @thread_safe Implementation-dependent
     * @note For ZeroMQ RADIO socket, this sets the message group via zmq_msg_set_group()
     */
    [[nodiscard]] virtual bool send(const std::vector<uint8_t>& data, const std::string& group) = 0;

    /**
     * @brief Receive binary message with timeout
     * @param timeoutMs Timeout in milliseconds (0 = non-blocking, -1 = infinite)
     * @return Received data if available, std::nullopt on timeout or error
     * @pre isConnected() == true
     * @thread_safe NO - Single consumer pattern REQUIRED.
     *              This method is NOT thread-safe by design.
     *              Only ONE thread should call receive() on a socket instance.
     *              In Thread-per-Type architecture, this is guaranteed by
     *              having a dedicated worker thread per incoming adapter.
     * @warning Calling receive() from multiple threads simultaneously
     *          will result in undefined behavior.
     */
    [[nodiscard]] virtual std::optional<std::vector<uint8_t>> receive(int32_t timeoutMs) = 0;

    /**
     * @brief Close socket and release resources
     * @post isConnected() returns false
     * @thread_safe Yes - idempotent operation
     */
    virtual void close() noexcept = 0;

    /**
     * @brief Check if socket is connected and ready
     * @return true if connected and ready for send/receive operations
     * @thread_safe Yes
     */
    [[nodiscard]] virtual bool isConnected() const noexcept = 0;

    /**
     * @brief Get socket type identifier
     * @return Socket type string (e.g., "ZeroMQ_DISH", "ZeroMQ_RADIO", "Mock")
     * @thread_safe Yes
     */
    [[nodiscard]] virtual std::string getSocketType() const noexcept = 0;

protected:
    /**
     * @brief Protected default constructor
     * @details Prevents direct instantiation of interface
     */
    IMessageSocket() = default;

    /**
     * @brief Protected copy constructor
     */
    IMessageSocket(const IMessageSocket&) = default;

    /**
     * @brief Protected move constructor
     */
    IMessageSocket(IMessageSocket&&) = default;

    /**
     * @brief Protected copy assignment
     */
    IMessageSocket& operator=(const IMessageSocket&) = default;

    /**
     * @brief Protected move assignment
     */
    IMessageSocket& operator=(IMessageSocket&&) = default;
};

} // namespace messaging
} // namespace common
} // namespace adapters

#endif // B_HEXAGON_ADAPTERS_COMMON_MESSAGING_IMESSAGE_SOCKET_HPP
