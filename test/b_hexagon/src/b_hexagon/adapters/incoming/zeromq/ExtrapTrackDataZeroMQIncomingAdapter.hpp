/**
 * @file ExtrapTrackDataZeroMQIncomingAdapter.hpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 * @details Thread-per-Type architecture compliant - runs in dedicated thread
 *          SOLID compliant - uses direct ZeroMQ socket for simplicity
 * 
 * Dependency Inversion:
 * - IExtrapTrackDataIncomingPort: Domain port abstraction
 */

#pragma once

#include "adapters/common/IAdapter.hpp"                           // IAdapter interface
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp" // Inbound port interface
#include "domain/ports/incoming/ExtrapTrackData.hpp"              // Domain data model
#include <zmq_config.hpp>
#include <zmq.hpp>
#include <string>
#include <memory>
#include <stdexcept>
#include <atomic>
#include <thread>

// Using declarations for convenience
using domain::ports::ExtrapTrackData;

/**
 * @class ExtrapTrackDataZeroMQIncomingAdapter
 * @brief ZeroMQ DISH adapter for receiving ExtrapTrackData via UDP multicast
 * 
 * SOLID Compliance:
 * - Single Responsibility: Only handles incoming message reception
 * - Open/Closed: Extends IAdapter, closed for modification
 * - Liskov Substitution: Can replace any IAdapter
 * - Interface Segregation: Implements focused IAdapter interface
 * - Dependency Inversion: Depends on domain port abstractions
 * 
 * Test Coverage:
 * - Direct ZeroMQ socket usage for production simplicity
 * - Verify data forwarding to domain port
 */
class ExtrapTrackDataZeroMQIncomingAdapter final : public adapters::IAdapter {
public:
    /**
     * @brief Construct with default socket (production use)
     * @param dataReceiver Domain port to forward received data
     */
    explicit ExtrapTrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver);
    
    /**
     * @brief Construct with custom endpoint and group (production use)
     * @param dataReceiver Domain port to forward received data
     * @param endpoint ZeroMQ endpoint (e.g., "udp://239.1.1.2:9001")
     * @param group Message group filter
     */
    ExtrapTrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver,
        const std::string& endpoint,
        const std::string& group);

    // Destructor - ensures graceful shutdown
    ~ExtrapTrackDataZeroMQIncomingAdapter() noexcept override;

    // Disable copy operations
    ExtrapTrackDataZeroMQIncomingAdapter(const ExtrapTrackDataZeroMQIncomingAdapter& other) = delete;
    ExtrapTrackDataZeroMQIncomingAdapter& operator=(const ExtrapTrackDataZeroMQIncomingAdapter& other) = delete;

    // Disable move operations for thread safety
    ExtrapTrackDataZeroMQIncomingAdapter(ExtrapTrackDataZeroMQIncomingAdapter&& other) = delete;
    ExtrapTrackDataZeroMQIncomingAdapter& operator=(ExtrapTrackDataZeroMQIncomingAdapter&& other) = delete;

    // ==================== IAdapter Interface ====================
    
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const noexcept override;
    [[nodiscard]] std::string getName() const noexcept override;

private:
    // Thread configuration
    static constexpr int RECEIVE_TIMEOUT_MS = 100;        ///< Socket receive timeout (allows graceful shutdown check)
    
    // Network configuration constants
    // ZeroMQ DISH Pattern: Distributed subscriber pattern for UDP multicast
    // - Allows multiple subscribers to receive same multicast messages
    // - Group filtering enables selective message reception
    // - UDP provides low-latency, connectionless communication
    static constexpr const char* ZMQ_MULTICAST_ADDRESS = "239.1.1.2";  ///< Multicast group address
    static constexpr int ZMQ_PORT = 9001;                               ///< UDP port number
    static constexpr const char* ZMQ_PROTOCOL = "udp";
    static constexpr const char* ZMQ_GROUP = "ExtrapTrackData";

    /**
     * @brief Background processing loop (runs in dedicated thread)
     * @details Continuously receives messages from ZeroMQ, deserializes them,
     *          and submits to domain layer via submitExtrapTrackData().
     *          Flow: ZeroMQ recv() → deserialize binary → submit to domain
     *          Thread-safe: Runs in isolated thread, no shared state except atomic flags
     */
    void process();

    // Deserialize binary data to ExtrapTrackData object
    // Binary format: Little-endian packed struct (4-byte int32, 8-byte doubles, 8-byte int64s)
    // Total size: 76 bytes (1x int32 + 6x double + 3x int64)
    static ExtrapTrackData deserializeBinary(const uint8_t* data, std::size_t size);

    // Configuration
    std::string endpoint_;             // ZeroMQ endpoint
    std::string group_;                // Group identifier for filtering
    std::string adapterName_;          // Adapter name for logging
    
    // ZeroMQ socket (direct implementation - no abstraction layer)
    zmq::context_t zmqContext_;
    std::unique_ptr<zmq::socket_t> zmqSocket_;
    
    // Thread-safe lifecycle management
    std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver_;
    std::thread workerThread_;         // Dedicated worker thread
    std::atomic<bool> running_{false}; // Thread-safe running flag
};