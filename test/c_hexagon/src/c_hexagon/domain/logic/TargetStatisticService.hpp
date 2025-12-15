/**
 * @file TargetStatisticService.hpp
 * @brief Domain service for final track data calculations
 * @details Implements the business logic for processing delay calculation data
 *          and producing final track statistics with multi-hop timing information.
 *
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 *
 * @note MISRA C++ 2023 compliant implementation
 * @see IDelayCalcTrackDataIncomingPort
 * @see ITrackDataStatisticOutgoingPort
 */

#pragma once

#include "domain/ports/incoming/DelayCalcTrackData.hpp"
#include "domain/ports/outgoing/FinalCalcTrackData.hpp"
#include "domain/ports/outgoing/ITrackDataStatisticOutgoingPort.hpp"
#include "domain/ports/incoming/IDelayCalcTrackDataIncomingPort.hpp"
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

namespace domain {
namespace logic {

using domain::ports::DelayCalcTrackData;
using domain::ports::FinalCalcTrackData;

/**
 * @class TargetStatisticService
 * @brief Domain service implementing final track data calculation logic with event queue
 * @details Processes incoming DelayCalcTrackData and produces FinalCalcTrackData
 *          with complete multi-hop delay analysis using dedicated processing thread.
 *          Event queue based architecture with isolated processing thread.
 *
 * @par Thread Architecture
 * - Incoming adapter thread: submitDelayCalcTrackData() (~20ns non-blocking enqueue)
 * - Domain processing thread: Dedicated background thread for processDelayCalcData()
 * - CPU affinity: Core 3, RT Priority: 90 (SCHED_FIFO)
 *
 * @par Data Flow
 * 1. submitDelayCalcTrackData() → non-blocking enqueue to event queue
 * 2. Background thread: dequeue → processDelayCalcData()
 * 3. Creates FinalCalcTrackData with complete timing information
 * 4. Sends result via outgoing port to external systems
 *
 * @invariant outgoing_port_ may be null (standalone mode)
 * @see IDelayCalcTrackDataIncomingPort
 * @see ITrackDataStatisticOutgoingPort
 */
class TargetStatisticService : public ports::incoming::IDelayCalcTrackDataIncomingPort {
private:
    // ==================== Configuration Constants ====================
    static constexpr std::size_t MAX_QUEUE_SIZE = 500;
    static constexpr int QUEUE_WAIT_TIMEOUT_MS = 100;
    static constexpr int DOMAIN_THREAD_PRIORITY = 90;
    static constexpr int DOMAIN_CPU_CORE = 3;

    // ==================== Dependencies ====================
    std::shared_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port_;  ///< Outgoing port for sending results

    // ==================== Event Queue Infrastructure ====================
    std::queue<ports::DelayCalcTrackData> eventQueue_;  ///< FIFO event queue
    std::mutex queueMutex_;                              ///< Queue protection
    std::condition_variable queueCV_;                    ///< Queue notification
    std::thread processingThread_;                       ///< Dedicated processing thread
    std::atomic<bool> running_{false};                   ///< Thread-safe running flag

public:
    /**
     * @brief Default constructor - operates without outgoing adapter
     */
    TargetStatisticService();

    /**
     * @brief Constructor with outgoing port (unique_ptr - takes ownership)
     * @param outgoing_port Port for sending processed data
     */
    explicit TargetStatisticService(
        std::unique_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port);

    /**
     * @brief Constructor with outgoing port (shared_ptr - shared ownership)
     * @param outgoing_port Port for sending processed data
     */
    explicit TargetStatisticService(
        std::shared_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port);

    /**
     * @brief Destructor - ensures graceful shutdown
     */
    ~TargetStatisticService() override;

    // Delete copy/move semantics
    TargetStatisticService(const TargetStatisticService&) = delete;
    TargetStatisticService& operator=(const TargetStatisticService&) = delete;
    TargetStatisticService(TargetStatisticService&&) = delete;
    TargetStatisticService& operator=(TargetStatisticService&&) = delete;

    // ==================== IDelayCalcTrackDataIncomingPort Interface ====================
    /**
     * @brief Submit delay calculation data to event queue (non-blocking)
     * @param delayCalcData Input data to enqueue
     * @details Enqueues message for background processing (~20ns latency)
     */
    void submitDelayCalcTrackData(const DelayCalcTrackData& delayCalcData) override;

    // ==================== Lifecycle Management ====================
    /**
     * @brief Start the domain processing thread
     * @return true if started successfully
     */
    [[nodiscard]] bool start();

    /**
     * @brief Stop the domain processing thread gracefully
     */
    void stop();

    /**
     * @brief Check if domain processor is running
     * @return true if running
     */
    [[nodiscard]] bool isRunning() const;

private:
    /**
     * @brief Background processing loop (dedicated thread)
     */
    void process();

    /**
     * @brief Enqueue message for background processing
     * @param data Track data to queue
     */
    void enqueueMessage(const DelayCalcTrackData& data);

    /**
     * @brief Process single delay calculation data item
     * @param delayCalcData Input data from B_hexagon
     * @details Calls createFinalCalcTrackData() and sends via outgoing port
     */
    void processDelayCalcData(const DelayCalcTrackData& delayCalcData);

    /**
     * @brief Create FinalCalcTrackData from DelayCalcTrackData with timing calculations
     * @param delayCalcData Input data from B_hexagon
     * @return Processed final track data with complete delay analysis
     */
    FinalCalcTrackData createFinalCalcTrackData(const DelayCalcTrackData& delayCalcData);

    /**
     * @brief Log processing results for monitoring and debugging
     * @param finalData Final calculated track data to log
     */
    void logProcessingResults(const FinalCalcTrackData& finalData);
};

} // namespace logic
} // namespace domain
