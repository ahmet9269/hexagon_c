/**
 * @file ProcessTrackUseCase.hpp
 * @brief Domain use case for processing track data with delay calculations
 * @details SOLID compliant use case that orchestrates the processing flow
 *          from incoming ExtrapTrackData to outgoing DelayCalcTrackData.
 *          Event queue based with dedicated processing thread.
 * 
 * @author b_hexagon Team
 * @version 2.0 - Event Queue Pattern with Dedicated Thread
 * @date 2025
 */

#pragma once

#include "domain/logic/ICalculatorService.hpp"
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "domain/ports/incoming/ExtrapTrackData.hpp"
#include "domain/ports/outgoing/DelayCalcTrackData.hpp"
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

namespace domain {
namespace logic {

/**
 * @class ProcessTrackUseCase
 * @brief Domain use case for processing track data with dedicated thread
 * @details Event queue based architecture with isolated processing thread.
 *          Orchestrates the flow: ExtrapTrackData → CalculatorService → DelayCalcTrackData
 *          SOLID compliant - depends on abstractions (ICalculatorService, IDelayCalcTrackDataOutgoingPort)
 * 
 * Responsibilities:
 * 1. Receive ExtrapTrackData from incoming adapters (via submitExtrapTrackData)
 * 2. Queue data for background processing (non-blocking enqueue)
 * 3. Process data through calculator service (delay calculation)
 * 4. Forward results to outgoing adapters (via sendDelayCalcTrackData)
 * 
 * Thread Architecture:
 * ┌──────────────────────────────────────────────────────────────┐
 * │  Incoming Adapter Thread    Domain Thread                   │
 * │  ───────────────────────    ─────────────                   │
 * │  submitExtrapTrackData() ──→ [Event Queue] ──→ process()    │
 * │         (~20ns enqueue)        (FIFO)         (while loop)  │
 * │                                                    │         │
 * │                                       CalculatorService      │
 * │                                                    │         │
 * │                                       sendDelayCalcTrackData │
 * └──────────────────────────────────────────────────────────────┘
 * 
 * Performance Characteristics:
 * - Enqueue latency: ~20ns (non-blocking for incoming adapter)
 * - Processing latency: Variable (depends on calculator service)
 * - Queue size: 500 messages max (prevents memory exhaustion)
 * - Overflow strategy: Drop oldest message (FIFO eviction)
 * 
 * Dependency Inversion:
 * - ICalculatorService: Abstraction for delay calculation (mockable)
 * - IDelayCalcTrackDataOutgoingPort: Abstraction for outbound data (mockable)
 */
class ProcessTrackUseCase final : public ports::incoming::IExtrapTrackDataIncomingPort {
public:
   /**
    * @brief Construct with unique_ptr ownership (legacy support)
    * @param calculator Calculator service (ownership transferred)
    * @param dataSender Outgoing port (ownership transferred)
    * @details Legacy constructor for single outgoing adapter
    *          Use shared_ptr constructor for multiple adapters
    */
   explicit ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::unique_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender);

   /**
    * @brief Construct with shared_ptr ownership (Event Queue architecture)
    * @param calculator Calculator service (ownership transferred via unique_ptr)
    * @param dataSender Outgoing port (shared ownership via shared_ptr)
    * @details Recommended constructor - allows multiple adapters to share port
    *          Example: ZeroMQ adapter + Custom analytics adapter
    */
   explicit ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::shared_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender);

    /**
     * @brief Destructor - ensures graceful shutdown
     */
    ~ProcessTrackUseCase() override;

    ProcessTrackUseCase(const ProcessTrackUseCase&) = delete;
    ProcessTrackUseCase& operator=(const ProcessTrackUseCase&) = delete;
    ProcessTrackUseCase(ProcessTrackUseCase&&) = delete;
    ProcessTrackUseCase& operator=(ProcessTrackUseCase&&) = delete;

    /**
     * @brief Submit incoming track data to event queue (non-blocking)
     * @param data Received ExtrapTrackData to process
     * @details Enqueues message for background processing (~20ns latency)
     *          Incoming adapter thread returns immediately
     * @thread_safe Yes - uses mutex-protected queue
     */
    void submitExtrapTrackData(const ports::ExtrapTrackData& data) override;

    /**
     * @brief Start the domain processing thread
     * @return true if started successfully, false otherwise
     */
    [[nodiscard]] bool start();

    /**
     * @brief Stop the domain processing thread gracefully
     */
    void stop();

    /**
     * @brief Check if domain processor is running
     * @return true if running, false otherwise
     */
    [[nodiscard]] bool isRunning() const;

private:
    // Queue configuration
    // 500 messages chosen for balance between buffering and memory usage:
    // - 500 * 76 bytes = 38 KB memory footprint
    // - At 1000 Hz input rate, 500 messages = 0.5 seconds of buffering
    static constexpr std::size_t MAX_QUEUE_SIZE = 500;    ///< Prevent unbounded growth
    static constexpr int QUEUE_WAIT_TIMEOUT_MS = 100;     ///< Graceful shutdown timeout (allows loop exit check)

    /**
     * @brief Background processing loop (runs in dedicated thread)
     * @details Continuously dequeues messages and processes them
     *          Loop exits when running_ flag is set to false
     */
    void process();

    /**
     * @brief Enqueue message for background processing
     * @param data Track data to queue
     * @details Non-blocking operation (~20ns)
     *          Uses mutex-protected queue with overflow prevention
     */
    void enqueueMessage(const ports::ExtrapTrackData& data);

    /**
     * @brief Process single track data item
     * @param data Track data to process
     * @details Orchestrates: calculateDelay() → sendDelayCalcTrackData()
     *          Runs in background thread (processTrackData called from process())
     */
    void processTrackData(const ports::ExtrapTrackData& data);
    
    // Dependencies (SOLID - Dependency Inversion)
    std::unique_ptr<ICalculatorService> calculator_;                           ///< Calculator service (abstract)
    std::shared_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender_; ///< Outgoing port (abstract)

    // Event queue infrastructure
    std::queue<ports::ExtrapTrackData> eventQueue_;  ///< FIFO event queue
    std::mutex queueMutex_;                          ///< Queue protection
    std::condition_variable queueCV_;                ///< Queue notification
    
    // Thread management
    std::thread processingThread_;                   ///< Dedicated processing thread
    std::atomic<bool> running_{false};               ///< Thread-safe running flag
};

} // namespace logic
} // namespace domain