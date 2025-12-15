/**
 * @file TargetStatisticService.cpp
 * @brief Implementation of the Final Calculation Service
 * @details Implements the business logic for processing delay calculation data
 *          and producing final track statistics with hop timing information.
 *
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 *
 * @note MISRA C++ 2023 compliant implementation
 * @see TargetStatisticService.hpp
 */

#include "TargetStatisticService.hpp"
#include "utils/Logger.hpp"
#include <chrono>
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#include <cerrno>
#include <cstring>
#endif

namespace domain {
namespace logic {

/**
 * @brief Default constructor - creates service without outgoing port
 */
TargetStatisticService::TargetStatisticService()
    : outgoing_port_(nullptr)
    , running_{false} {
    LOG_DEBUG("TargetStatisticService initialized without outgoing adapter");
}

/**
 * @brief Constructor with outgoing port injection (unique_ptr)
 * @param outgoing_port Unique pointer to the outgoing port adapter
 */
TargetStatisticService::TargetStatisticService(
    std::unique_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port)
    : outgoing_port_(std::move(outgoing_port))
    , running_{false} {
    if (!outgoing_port_) {
        throw std::invalid_argument("ITrackDataStatisticOutgoingPort cannot be null");
    }
    LOG_DEBUG("TargetStatisticService initialized with outgoing adapter (unique_ptr)");
}

/**
 * @brief Constructor with outgoing port injection (shared_ptr)
 * @param outgoing_port Shared pointer to the outgoing port adapter
 */
TargetStatisticService::TargetStatisticService(
    std::shared_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port)
    : outgoing_port_(std::move(outgoing_port))
    , running_{false} {
    if (!outgoing_port_) {
        throw std::invalid_argument("ITrackDataStatisticOutgoingPort cannot be null");
    }
    LOG_DEBUG("TargetStatisticService initialized with outgoing adapter (shared_ptr)");
}

/**
 * @brief Destructor - ensures graceful shutdown
 */
TargetStatisticService::~TargetStatisticService() {
    stop();
    LOG_DEBUG("TargetStatisticService destroyed");
}

// ==================== Lifecycle Management ====================

bool TargetStatisticService::start() {
    if (running_.load()) {
        LOG_WARN("TargetStatisticService already running");
        return true;
    }

    running_.store(true);

    // Start dedicated processing thread
    processingThread_ = std::thread([this]() {
        #ifdef __linux__
        // Set real-time scheduling priority
        struct sched_param param;
        param.sched_priority = DOMAIN_THREAD_PRIORITY;
        int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        if (ret != 0) {
            LOG_DEBUG("RT scheduling not available (priority {}): {} - running with default scheduling", DOMAIN_THREAD_PRIORITY, std::strerror(ret));
        } else {
            LOG_DEBUG("Domain thread RT priority set to {}", DOMAIN_THREAD_PRIORITY);
        }

        // Set CPU affinity
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DOMAIN_CPU_CORE, &cpuset);
        ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        if (ret != 0) {
            LOG_DEBUG("CPU affinity not set (core {}): {} - running on any available core", DOMAIN_CPU_CORE, std::strerror(ret));
        } else {
            LOG_DEBUG("Domain thread pinned to CPU core {}", DOMAIN_CPU_CORE);
        }
        #endif

        process();
    });

    LOG_INFO("TargetStatisticService started with dedicated thread (CPU {}, Priority {})",
             DOMAIN_CPU_CORE, DOMAIN_THREAD_PRIORITY);
    return true;
}

void TargetStatisticService::stop() {
    if (!running_.load()) {
        return;
    }

    LOG_INFO("Stopping TargetStatisticService...");
    running_.store(false);

    // Wake up the processing thread
    queueCV_.notify_all();

    if (processingThread_.joinable()) {
        processingThread_.join();
    }

    LOG_INFO("TargetStatisticService stopped");
}

bool TargetStatisticService::isRunning() const {
    return running_.load();
}

// ==================== Event Queue Interface ====================

/**
 * @brief Submit delay calculation data to event queue (non-blocking)
 * @param delayCalcData Input data to enqueue
 */
void TargetStatisticService::submitDelayCalcTrackData(const DelayCalcTrackData& delayCalcData) {
    if (!running_.load()) {
        LOG_WARN("TargetStatisticService not running, dropping track: {}", delayCalcData.getTrackId());
        return;
    }

    // Validate input data before queueing
    if (!delayCalcData.isValid()) {
        LOG_WARN("Invalid DelayCalcTrackData received: ID={}", delayCalcData.getTrackId());
        return;
    }

    // Non-blocking enqueue (~20ns)
    enqueueMessage(delayCalcData);
}

void TargetStatisticService::enqueueMessage(const DelayCalcTrackData& data) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);

        // Prevent unbounded queue growth
        if (eventQueue_.size() >= MAX_QUEUE_SIZE) {
            LOG_WARN("Event queue full (size: {}), dropping oldest message for track: {}",
                     MAX_QUEUE_SIZE, data.getTrackId());
            eventQueue_.pop();
        }

        eventQueue_.push(data);
    }

    queueCV_.notify_one();
}

// ==================== Background Processing Loop ====================

void TargetStatisticService::process() {
    LOG_DEBUG("Domain processing thread started");

    while (running_.load()) {
        DelayCalcTrackData data;

        // Wait for message with timeout
        {
            std::unique_lock<std::mutex> lock(queueMutex_);

            if (queueCV_.wait_for(lock, std::chrono::milliseconds(QUEUE_WAIT_TIMEOUT_MS),
                [this]() { return !eventQueue_.empty() || !running_.load(); })) {

                if (!running_.load()) {
                    break;
                }

                if (!eventQueue_.empty()) {
                    data = std::move(eventQueue_.front());
                    eventQueue_.pop();
                } else {
                    continue;
                }
            } else {
                continue;  // Timeout, check running flag
            }
        }

        // Process track data (outside lock)
        processDelayCalcData(data);
    }

    LOG_DEBUG("Domain processing thread stopped");
}

/**
 * @brief Process delay calculation data and forward to outgoing port
 * @param delayCalcData Input data to be processed
 * @details Creates FinalCalcTrackData, logs results, and sends via outgoing adapter
 */
void TargetStatisticService::processDelayCalcData(const DelayCalcTrackData& delayCalcData) {
    // Business Logic: Create FinalCalcTrackData
    FinalCalcTrackData finalData = createFinalCalcTrackData(delayCalcData);

    // Log processing results
    logProcessingResults(finalData);

    // Send via outgoing port (Hexagonal Architecture)
    if (outgoing_port_ && outgoing_port_->isReady()) {
        outgoing_port_->sendFinalTrackData(finalData);
        LOG_DEBUG("Sent FinalCalcTrackData via outgoing adapter - Track ID: {}", finalData.getTrackId());
    } else {
        LOG_WARN("Custom outgoing adapter not ready");
    }
}

/**
 * @brief Create FinalCalcTrackData from DelayCalcTrackData
 * @param delayCalcData Source data with delay calculations
 * @return FinalCalcTrackData with computed hop delays and total delay
 * @details Copies track data, calculates second hop delay using current time,
 *          and computes total delay as sum of first and second hop delays
 */
FinalCalcTrackData TargetStatisticService::createFinalCalcTrackData(const DelayCalcTrackData& delayCalcData) {
    FinalCalcTrackData finalData;

    // Copy basic track data
    finalData.setTrackId(delayCalcData.getTrackId());
    finalData.setXPositionECEF(delayCalcData.getXPositionECEF());
    finalData.setYPositionECEF(delayCalcData.getYPositionECEF());
    finalData.setZPositionECEF(delayCalcData.getZPositionECEF());
    finalData.setXVelocityECEF(delayCalcData.getXVelocityECEF());
    finalData.setYVelocityECEF(delayCalcData.getYVelocityECEF());
    finalData.setZVelocityECEF(delayCalcData.getZVelocityECEF());

    // Set timing information
    auto currentTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    finalData.setThirdHopSentTime(currentTime);
    finalData.setSecondHopSentTime(delayCalcData.getSecondHopSentTime());
    finalData.setFirstHopDelayTime(delayCalcData.getFirstHopDelayTime());
    finalData.setSecondHopDelayTime(currentTime - delayCalcData.getSecondHopSentTime());
    finalData.setTotalDelayTime(finalData.getSecondHopDelayTime() + delayCalcData.getFirstHopDelayTime());

    return finalData;
}

/**
 * @brief Log processing results for monitoring and debugging
 * @param finalData Processed track data with delay information
 */
void TargetStatisticService::logProcessingResults(const FinalCalcTrackData& finalData) {
    LOG_INFO("Track ID: {} | Hop1: {} μs | Hop2: {} μs | Total: {} μs",
             finalData.getTrackId(),
             finalData.getFirstHopDelayTime(),
             finalData.getSecondHopDelayTime(),
             finalData.getFirstHopDelayTime() + finalData.getSecondHopDelayTime());
}

} // namespace logic
} // namespace domain
