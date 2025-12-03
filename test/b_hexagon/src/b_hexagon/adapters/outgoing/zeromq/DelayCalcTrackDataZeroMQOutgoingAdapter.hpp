/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 * @details Thread-per-Type architecture compliant - implements IAdapter interface
 *          SOLID compliant - depends on IMessageSocket abstraction for testability
 * 
 * Dependency Inversion:
 * - IMessageSocket: Abstraction for messaging (enables mock testing)
 * - IDelayCalcTrackDataOutgoingPort: Domain port abstraction
 */

#pragma once

#include "adapters/common/IAdapter.hpp"                              // IAdapter interface
#include "adapters/common/messaging/IMessageSocket.hpp"              // Socket abstraction (DIP)
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp" // Outbound port interface
#include "domain/ports/DelayCalcTrackData.hpp"                       // Domain data model
#include <string>
#include <memory>
#include <atomic>
#include <mutex>

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;

/**
 * @class DelayCalcTrackDataZeroMQOutgoingAdapter
 * @brief ZeroMQ RADIO adapter for broadcasting DelayCalcTrackData
 * 
 * SOLID Compliance:
 * - Single Responsibility: Only handles outgoing message transmission
 * - Open/Closed: Extends IAdapter and port, closed for modification
 * - Liskov Substitution: Can replace any IAdapter or IDelayCalcTrackDataOutgoingPort
 * - Interface Segregation: Implements focused interfaces
 * - Dependency Inversion: Depends on IMessageSocket abstraction
 * 
 * Test Coverage:
 * - Inject MockMessageSocket to test without network
 * - Verify serialization and send calls
 */
class DelayCalcTrackDataZeroMQOutgoingAdapter final 
    : public adapters::IAdapter
    , public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
public:
    /**
     * @brief Default constructor (production use)
     */
    explicit DelayCalcTrackDataZeroMQOutgoingAdapter();
    
    /**
     * @brief Construct with custom endpoint and group (production use)
     */
    DelayCalcTrackDataZeroMQOutgoingAdapter(
        const std::string& endpoint,
        const std::string& group);

    /**
     * @brief Construct with injected socket (for testing with mocks)
     * @param socket Pre-configured socket (can be MockMessageSocket)
     * @param group Message group for RADIO
     */
    DelayCalcTrackDataZeroMQOutgoingAdapter(
        std::unique_ptr<adapters::common::messaging::IMessageSocket> socket,
        const std::string& group);

    // Destructor - RAII cleanup
    ~DelayCalcTrackDataZeroMQOutgoingAdapter() noexcept override;

    // Disable copy operations
    DelayCalcTrackDataZeroMQOutgoingAdapter(const DelayCalcTrackDataZeroMQOutgoingAdapter& other) = delete;
    DelayCalcTrackDataZeroMQOutgoingAdapter& operator=(const DelayCalcTrackDataZeroMQOutgoingAdapter& other) = delete;

    // Disable move operations for thread safety
    DelayCalcTrackDataZeroMQOutgoingAdapter(DelayCalcTrackDataZeroMQOutgoingAdapter&& other) = delete;
    DelayCalcTrackDataZeroMQOutgoingAdapter& operator=(DelayCalcTrackDataZeroMQOutgoingAdapter&& other) = delete;

    // ==================== IAdapter Interface ====================
    
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const noexcept override;

    // ==================== IDelayCalcTrackDataOutgoingPort Interface ====================
    
    /**
     * @brief Send data via RADIO socket (thread-safe)
     * @param data Data to send
     * @thread_safe Yes - IMessageSocket::send() is internally synchronized.
     *              Adapter-level mutex removed to avoid double-locking overhead.
     *              Multiple threads can safely call this method concurrently.
     */
    void sendDelayCalcTrackData(const DelayCalcTrackData& data) override;

private:
    // Network configuration constants
    static constexpr const char* DEFAULT_ADDRESS = "127.0.0.1";
    static constexpr int DEFAULT_PORT = 15002;
    static constexpr const char* DEFAULT_PROTOCOL = "tcp";
    static constexpr const char* DEFAULT_GROUP = "DelayCalcTrackData";

    /**
     * @brief Create default ZeroMQ socket
     */
    std::unique_ptr<adapters::common::messaging::IMessageSocket> createDefaultSocket();

    // Configuration
    std::string endpoint_;             // ZeroMQ endpoint
    std::string group_;                // Group identifier for DISH filtering
    std::string adapterName_;          // Adapter name for logging
    
    // Socket abstraction (DIP - enables mock injection)
    std::unique_ptr<adapters::common::messaging::IMessageSocket> socket_;
    
    // Thread safety
    std::atomic<bool> running_{false}; // Running state
    // Note: sendMutex_ removed - IMessageSocket::send() is internally thread-safe
    //       This eliminates double-locking overhead (adapter mutex + socket mutex)
};