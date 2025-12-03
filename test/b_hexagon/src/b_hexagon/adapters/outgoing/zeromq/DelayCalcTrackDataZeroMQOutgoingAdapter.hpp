/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 * @details Thread-per-Type architecture compliant - implements IAdapter interface
 *          SOLID compliant - depends on IMessageSocket abstraction for testability
 *          Uses background worker thread for non-blocking sends (~20ns enqueue latency)
 * 
 * Dependency Inversion:
 * - IMessageSocket: Abstraction for messaging (enables mock testing)
 * - IDelayCalcTrackDataOutgoingPort: Domain port abstraction
 * 
 * Thread Architecture:
 * ┌─────────────────────────────────────────────────────────────────┐
 * │  Domain Thread          Background Worker Thread               │
 * │  ─────────────          ────────────────────────                │
 * │  sendDelayCalcTrackData() ──┐                                   │
 * │       │                     │                                   │
 * │       ▼                     ▼                                   │
 * │  [Enqueue ~20ns]     [Dequeue + ZMQ Send]                      │
 * │       │                     │                                   │
 * │  Return immediately    Background transmission                  │
 * └─────────────────────────────────────────────────────────────────┘
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
#include <thread>
#include <queue>
#include <condition_variable>

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
 * Real-time Features:
 * - Non-blocking sendDelayCalcTrackData() (~20ns enqueue)
 * - Background worker thread for actual ZMQ transmission
 * - Bounded queue with overflow protection
 * - SCHED_FIFO priority for worker thread
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
     * @brief Send data via RADIO socket (non-blocking)
     * @param data Data to send
     * @details Enqueues message for background transmission (~20ns latency)
     *          Domain thread returns immediately without blocking on ZMQ I/O
     * @thread_safe Yes - uses mutex-protected queue
     */
    void sendDelayCalcTrackData(const DelayCalcTrackData& data) override;

    /**
     * @brief Check if adapter is ready to accept messages
     * @return true if running and socket connected
     */
    [[nodiscard]] bool isReady() const noexcept;

private:
    // Real-time thread configuration
    static constexpr int REALTIME_THREAD_PRIORITY = 95;
    static constexpr int DEDICATED_CPU_CORE = 2;  // Different from incoming adapter
    
    // Network configuration constants
    static constexpr const char* DEFAULT_ADDRESS = "127.0.0.1";
    static constexpr int DEFAULT_PORT = 15002;
    static constexpr const char* DEFAULT_PROTOCOL = "tcp";
    static constexpr const char* DEFAULT_GROUP = "DelayCalcTrackData";
    
    // Queue configuration
    static constexpr std::size_t MAX_QUEUE_SIZE = 1000;  ///< Prevent unbounded growth

    /**
     * @brief Create default ZeroMQ socket
     */
    std::unique_ptr<adapters::common::messaging::IMessageSocket> createDefaultSocket();

    /**
     * @brief Background worker thread for message transmission
     */
    void publisherWorker();

    /**
     * @brief Enqueue message for background transmission
     * @param data Track data to queue
     */
    void enqueueMessage(const DelayCalcTrackData& data);

    // Configuration
    std::string endpoint_;             ///< ZeroMQ endpoint
    std::string group_;                ///< Group identifier for DISH filtering
    std::string adapterName_;          ///< Adapter name for logging
    
    // Socket abstraction (DIP - enables mock injection)
    std::unique_ptr<adapters::common::messaging::IMessageSocket> socket_;
    
    // Thread management
    std::thread publisherThread_;      ///< Background worker thread
    std::atomic<bool> running_{false}; ///< Running state
    std::atomic<bool> ready_{false};   ///< Socket ready state

    // Thread-safe message queue
    mutable std::mutex queueMutex_;              ///< Queue protection
    std::condition_variable queueCV_;   ///< Queue notification
    std::queue<DelayCalcTrackData> messageQueue_; ///< Pending messages
};