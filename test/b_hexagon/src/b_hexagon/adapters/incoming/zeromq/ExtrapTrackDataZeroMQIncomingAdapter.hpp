/**
 * @file ExtrapTrackDataZeroMQIncomingAdapter.hpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 * @details Thread-per-Type architecture compliant - runs in dedicated thread
 *          SOLID compliant - depends on IMessageSocket abstraction for testability
 * 
 * Dependency Inversion:
 * - IMessageSocket: Abstraction for messaging (enables mock testing)
 * - IExtrapTrackDataIncomingPort: Domain port abstraction
 */

#pragma once

#include "adapters/common/IAdapter.hpp"                           // IAdapter interface
#include "adapters/common/messaging/IMessageSocket.hpp"           // Socket abstraction (DIP)
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp" // Inbound port interface
#include "domain/ports/ExtrapTrackData.hpp"                       // Domain data model
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
 * - Dependency Inversion: Depends on IMessageSocket abstraction
 * 
 * Test Coverage:
 * - Inject MockMessageSocket to test without network
 * - Control message flow via mock
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

    /**
     * @brief Construct with injected socket (for testing with mocks)
     * @param dataReceiver Domain port to forward received data
     * @param socket Pre-configured socket (can be MockMessageSocket)
     * @param group Message group filter
     */
    ExtrapTrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver,
        std::unique_ptr<adapters::common::messaging::IMessageSocket> socket,
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
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const noexcept override;

private:
    // Real-time thread configuration
    static constexpr int REALTIME_THREAD_PRIORITY = 95;
    static constexpr int DEDICATED_CPU_CORE = 1;
    static constexpr int RECEIVE_TIMEOUT_MS = 100;
    
    // Network configuration constants
    static constexpr const char* DEFAULT_MULTICAST_ADDRESS = "239.1.1.2";
    static constexpr int DEFAULT_PORT = 9001;
    static constexpr const char* DEFAULT_PROTOCOL = "udp";
    static constexpr const char* DEFAULT_GROUP = "ExtrapTrackData";

    /**
     * @brief Worker thread function
     */
    void workerThread();

    /**
     * @brief Create default ZeroMQ socket
     */
    std::unique_ptr<adapters::common::messaging::IMessageSocket> createDefaultSocket();

    // Deserialize binary data to ExtrapTrackData object
    static ExtrapTrackData deserializeBinary(const uint8_t* data, std::size_t size);

    // Configuration
    std::string endpoint_;             // ZeroMQ endpoint
    std::string group_;                // Group identifier for filtering
    std::string adapterName_;          // Adapter name for logging
    
    // Socket abstraction (DIP - enables mock injection)
    std::unique_ptr<adapters::common::messaging::IMessageSocket> socket_;
    
    // Thread-safe lifecycle management
    std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver_;
    std::thread workerThread_;         // Dedicated worker thread
    std::atomic<bool> running_{false}; // Thread-safe running flag
};