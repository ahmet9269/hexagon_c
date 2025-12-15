/**
 * @file DelayCalcTrackDataCustomOutgoingAdapter.hpp
 * @brief Custom adapter for DelayCalcTrackData processing with moving average calculation
 * @details Event queue based architecture with dedicated processing thread
 *          Calculates moving average of FirstHopDelayTime from last 100 samples
 */

#pragma once

#include "adapters/common/IAdapter.hpp"                              // IAdapter interface
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp" // Outbound port interface
#include "domain/ports/outgoing/DelayCalcTrackData.hpp"              // Domain data model
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <deque>
#include <condition_variable>
#include <vector>
#include <cstdint>

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;

/**
 * @class DelayCalcTrackDataCustomOutgoingAdapter
 * @brief Custom adapter for DelayCalcTrackData processing with moving average calculation
 * 
 * SOLID Compliance:
 * - Single Responsibility: Only handles DelayCalcTrackData processing and moving average
 * - Open/Closed: Extends IAdapter and port, closed for modification
 * - Liskov Substitution: Can replace any IAdapter or IDelayCalcTrackDataOutgoingPort
 * - Interface Segregation: Implements focused interfaces
 * - Dependency Inversion: Depends on port abstractions
 * 
 * Real-time Features:
 * - Non-blocking sendDelayCalcTrackData() (~20ns enqueue)
 * - Background worker thread for moving average calculation
 * - Bounded queue with overflow protection
 * - Circular buffer of 100 samples for moving average
 * 
 * Processing Logic:
 * - Maintains circular buffer of last 100 FirstHopDelayTime values
 * - Calculates moving average on each new sample
 * - Thread-safe access to moving average value
 */
class DelayCalcTrackDataCustomOutgoingAdapter final 
    : public adapters::IAdapter
    , public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
public:
    /**
     * @brief Default constructor
     */
    explicit DelayCalcTrackDataCustomOutgoingAdapter();

    // Destructor - RAII cleanup
    ~DelayCalcTrackDataCustomOutgoingAdapter() noexcept override;

    // Disable copy operations
    DelayCalcTrackDataCustomOutgoingAdapter(const DelayCalcTrackDataCustomOutgoingAdapter& other) = delete;
    DelayCalcTrackDataCustomOutgoingAdapter& operator=(const DelayCalcTrackDataCustomOutgoingAdapter& other) = delete;

    // Disable move operations for thread safety
    DelayCalcTrackDataCustomOutgoingAdapter(DelayCalcTrackDataCustomOutgoingAdapter&& other) = delete;
    DelayCalcTrackDataCustomOutgoingAdapter& operator=(DelayCalcTrackDataCustomOutgoingAdapter&& other) = delete;

    // ==================== IAdapter Interface ====================
    
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const noexcept override;

    // ==================== IDelayCalcTrackDataOutgoingPort Interface ====================
    
    /**
     * @brief Send data for processing (non-blocking)
     * @param data Data to process
     * @details Enqueues message for background moving average calculation (~20ns latency)
     *          Domain thread returns immediately without blocking
     * @thread_safe Yes - uses mutex-protected queue
     */
    void sendDelayCalcTrackData(const DelayCalcTrackData& data) override;

    /**
     * @brief Check if adapter is ready to accept messages
     * @return true if running and ready
     */
    [[nodiscard]] bool isReady() const noexcept;

    /**
     * @brief Get current moving average of FirstHopDelayTime
     * @return Moving average in microseconds (0.0 if no samples)
     * @thread_safe Yes - uses mutex-protected access
     */
    [[nodiscard]] double getMovingAverage() const noexcept;

    /**
     * @brief Get number of samples in circular buffer
     * @return Sample count (0-100)
     * @thread_safe Yes - uses mutex-protected access
     */
    [[nodiscard]] std::size_t getSampleCount() const noexcept;

private:
    // Queue configuration
    static constexpr std::size_t MAX_QUEUE_SIZE = 200;   ///< Prevent unbounded growth
    static constexpr int QUEUE_WAIT_TIMEOUT_MS = 100;    ///< Graceful shutdown timeout
    
    // Moving average configuration
    // 100 samples chosen for balance between responsiveness and stability:
    // - Smaller window: More responsive to changes, but noisier
    // - Larger window: More stable, but slower to react to trend changes
    // At 100 Hz input rate, 100 samples = 1 second of history
    static constexpr std::size_t SAMPLE_BUFFER_SIZE = 100; ///< Circular buffer size

    /**
     * @brief Background processing loop for moving average calculation
     * @details Runs in dedicated thread, continuously dequeues messages
     *          and updates moving average
     */
    void process();

    /**
     * @brief Enqueue message for background processing
     * @param data Track data to queue
     */
    void enqueueMessage(const DelayCalcTrackData& data);

    /**
     * @brief Update moving average with new sample
     * @param firstHopDelay New FirstHopDelayTime value in microseconds
     */
    void updateMovingAverage(int64_t firstHopDelay);

    // Configuration
    std::string adapterName_;          ///< Adapter name for logging
    
    // Thread management
    std::thread publisherThread_;      ///< Background worker thread
    std::atomic<bool> running_{false}; ///< Running state
    std::atomic<bool> ready_{false};   ///< Ready state

    // Thread-safe message queue
    mutable std::mutex queueMutex_;              ///< Queue protection
    std::condition_variable queueCV_;            ///< Queue notification
    std::queue<DelayCalcTrackData> messageQueue_; ///< Pending messages
    
    // Moving average calculation
    mutable std::mutex sampleMutex_;             ///< Sample buffer protection
    std::deque<int64_t> sampleBuffer_;           ///< Circular buffer (max 100 samples)
    double movingAverage_{0.0};                  ///< Current moving average (microseconds)
};
