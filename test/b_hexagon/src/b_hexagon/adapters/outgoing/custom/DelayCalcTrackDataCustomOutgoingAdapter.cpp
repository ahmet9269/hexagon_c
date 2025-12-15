/**
 * @file DelayCalcTrackDataCustomOutgoingAdapter.cpp
 * @brief Custom adapter for DelayCalcTrackData processing with moving average calculation
 * @details Event queue based with background worker thread
 *          SOLID compliant - calculates moving average of FirstHopDelayTime
 *          Non-blocking send (~20ns enqueue) for real-time performance
 */

#include "DelayCalcTrackDataCustomOutgoingAdapter.hpp"
#include "../../../utils/Logger.hpp"
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <numeric>

using domain::ports::DelayCalcTrackData;

// ==================== Constructors ====================

DelayCalcTrackDataCustomOutgoingAdapter::DelayCalcTrackDataCustomOutgoingAdapter()
    : adapterName_("DelayCalcTrackData-Custom-OutAdapter")
    , running_{false}
    , ready_{false}
    , movingAverage_{0.0} {
    
    Logger::info("DelayCalcTrackDataCustomOutgoingAdapter created - moving average buffer size: ", SAMPLE_BUFFER_SIZE);
}



DelayCalcTrackDataCustomOutgoingAdapter::~DelayCalcTrackDataCustomOutgoingAdapter() noexcept {
    stop();
    Logger::debug("DelayCalcTrackDataCustomOutgoingAdapter destroyed: ", adapterName_);
}



// ==================== IAdapter Interface ====================

bool DelayCalcTrackDataCustomOutgoingAdapter::start() {
    if (running_.load()) {
        Logger::warn("Adapter already running: ", adapterName_);
        return true;
    }
    
    running_.store(true);
    ready_.store(true);
    
    // Start background processing thread
    publisherThread_ = std::thread([this]() {
        process();
    });
    
    Logger::info("Adapter started with background worker: ", adapterName_);
    return true;
}

void DelayCalcTrackDataCustomOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    Logger::info("Stopping adapter: ", adapterName_);
    running_.store(false);
    ready_.store(false);
    
    // Wake up the worker thread
    queueCV_.notify_all();
    
    if (publisherThread_.joinable()) {
        publisherThread_.join();
    }
    
    Logger::info("Adapter stopped: ", adapterName_);
}

bool DelayCalcTrackDataCustomOutgoingAdapter::isRunning() const {
    return running_.load();
}

bool DelayCalcTrackDataCustomOutgoingAdapter::isReady() const noexcept {
    return ready_.load() && running_.load();
}

std::string DelayCalcTrackDataCustomOutgoingAdapter::getName() const noexcept {
    return adapterName_;
}

// ==================== Non-blocking Send (~20ns enqueue) ====================

void DelayCalcTrackDataCustomOutgoingAdapter::sendDelayCalcTrackData(const DelayCalcTrackData& data) {
    if (!isReady()) {
        Logger::warn("Adapter not ready, dropping message for track: ", data.getTrackId());
        return;
    }
    
    // Validate input data
    if (!data.isValid()) {
        Logger::error("Invalid DelayCalcTrackData for track ID: ", data.getTrackId());
        return;
    }
    
    // Non-blocking enqueue (~20ns)
    // Domain thread returns immediately without waiting for processing
    enqueueMessage(data);
}

void DelayCalcTrackDataCustomOutgoingAdapter::enqueueMessage(const DelayCalcTrackData& data) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        // Prevent unbounded queue growth
        // Overflow strategy: Drop oldest message (FIFO eviction)
        // This prevents memory exhaustion and ensures latest data is processed
        // Alternative: Drop newest (better for historical analysis)
        // Choice: Drop oldest - prioritizes recent data for real-time systems
        if (messageQueue_.size() >= MAX_QUEUE_SIZE) {
            Logger::warn("Message queue full, dropping oldest message");
            messageQueue_.pop();  // Remove oldest element
        }
        
        messageQueue_.push(data);
    }
    
    // Notify background worker thread that new data is available
    // Only one thread needs to wake up (notify_one vs notify_all)
    queueCV_.notify_one();
}

// ==================== Background Processing Loop ====================

void DelayCalcTrackDataCustomOutgoingAdapter::process() {
    Logger::debug("Custom adapter processing thread started: ", adapterName_);
    
    // Main processing loop - runs until stop() sets running_ to false
    // Pattern: Event queue with condition variable wait
    while (running_.load()) {
        DelayCalcTrackData data;
        
        // Wait for message with timeout
        // Timeout allows periodic check of running_ flag for graceful shutdown
        // Without timeout, thread would block indefinitely on empty queue
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            // wait_for with predicate:
            // - Atomically releases lock and waits for notification OR timeout
            // - Predicate prevents spurious wakeups (must return true to proceed)
            // - Predicate: (!messageQueue_.empty() || !running_)
            //   * Queue has data: Process it
            //   * Adapter stopping: Exit loop
            // Timeout: QUEUE_WAIT_TIMEOUT_MS (100ms) - balance between responsiveness and CPU usage
            if (queueCV_.wait_for(lock, std::chrono::milliseconds(QUEUE_WAIT_TIMEOUT_MS),
                [this]() { return !messageQueue_.empty() || !running_.load(); })) {
                
                // Check stop condition first (higher priority than data processing)
                if (!running_.load()) {
                    break;
                }
                
                // Dequeue message for processing
                if (!messageQueue_.empty()) {
                    data = std::move(messageQueue_.front());  // Move semantics for efficiency
                    messageQueue_.pop();
                } else {
                    continue;  // Spurious wakeup with empty queue - retry
                }
            } else {
                continue;  // Timeout - check running flag and retry
            }
        }
        
        // Process FirstHopDelayTime (outside lock)
        // Extract delay metric from DelayCalcTrackData
        // FirstHopDelayTime = time between data generation and first reception (microseconds)
        try {
            int64_t firstHopDelay = data.getFirstHopDelayTime();
            
            // Update moving average with new sample
            // This recalculates average across last 100 samples
            updateMovingAverage(firstHopDelay);
            
            Logger::debug("[", adapterName_, "] Processed TrackID: ", data.getTrackId(), 
                         ", FirstHopDelay: ", firstHopDelay, " µs",
                         ", MovingAverage: ", movingAverage_, " µs",
                         ", Samples: ", sampleBuffer_.size());
            
        } catch (const std::exception& e) {
            Logger::error("[", adapterName_, "] Processing error: ", e.what());
        }
    }
    
    Logger::debug("Custom adapter processing thread stopped: ", adapterName_);
}

// ==================== Moving Average Calculation ====================
// Implements Simple Moving Average (SMA) algorithm
// Formula: SMA = (sum of last N samples) / N
// Circular buffer maintains fixed window of 100 samples (oldest dropped when full)
// Thread-safe: Protected by sampleMutex_ for concurrent read/write access

void DelayCalcTrackDataCustomOutgoingAdapter::updateMovingAverage(int64_t firstHopDelay) {
    std::lock_guard<std::mutex> lock(sampleMutex_);
    
    // Add new sample to circular buffer (deque provides efficient push/pop at both ends)
    sampleBuffer_.push_back(firstHopDelay);
    
    // Remove oldest sample if buffer exceeds limit (maintains fixed window size)
    if (sampleBuffer_.size() > SAMPLE_BUFFER_SIZE) {
        sampleBuffer_.pop_front();  // FIFO eviction
    }
    
    // Calculate moving average
    // Using std::accumulate for sum calculation (efficient and readable)
    // Result is cast to double for sub-microsecond precision
    if (!sampleBuffer_.empty()) {
        int64_t sum = std::accumulate(sampleBuffer_.begin(), sampleBuffer_.end(), 0LL);
        movingAverage_ = static_cast<double>(sum) / static_cast<double>(sampleBuffer_.size());
    } else {
        movingAverage_ = 0.0;  // No samples yet
    }
}

double DelayCalcTrackDataCustomOutgoingAdapter::getMovingAverage() const noexcept {
    std::lock_guard<std::mutex> lock(sampleMutex_);
    return movingAverage_;
}

std::size_t DelayCalcTrackDataCustomOutgoingAdapter::getSampleCount() const noexcept {
    std::lock_guard<std::mutex> lock(sampleMutex_);
    return sampleBuffer_.size();
}
