/**
 * @file ProcessTrackUseCase.cpp
 * @brief Implementation of ProcessTrackUseCase with event queue and dedicated thread
 * @details Orchestrates the processing flow from incoming ExtrapTrackData to
 *          outgoing DelayCalcTrackData using event queue pattern with isolated thread.
 * 
 * @author b_hexagon Team
 * @version 2.0 - Event Queue Pattern
 * @date 2025
 * 
 * @see ProcessTrackUseCase.hpp
 * @see ICalculatorService.hpp
 */

#include "domain/logic/ProcessTrackUseCase.hpp"
#include "utils/Logger.hpp"
#include <stdexcept>

// Linux real-time scheduling headers
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif


namespace domain {
namespace logic {

// ==================== Constructor (unique_ptr - legacy) ====================
ProcessTrackUseCase::ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::unique_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender
    ): calculator_(std::move(calculator)),
    dataSender_(std::move(dataSender)){
        if (!calculator_) {
            throw std::invalid_argument("ICalculatorService cannot be null");
        }
        if (!dataSender_) {
            throw std::invalid_argument("IDelayCalcTrackDataOutgoingPort cannot be null");
        }
        Logger::info("ProcessTrackUseCase initialized (unique_ptr ownership)");
    }

// ==================== Constructor (shared_ptr - Event Queue) ====================
// Dependency Injection: Receives calculator service and outgoing port as abstractions
// - ICalculatorService: Abstract calculator (enables mock injection for testing)
// - IDelayCalcTrackDataOutgoingPort: Abstract outgoing port (supports multiple adapters)
// shared_ptr used for dataSender to allow multiple adapters to share same port
ProcessTrackUseCase::ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::shared_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender
    ): calculator_(std::move(calculator)),
    dataSender_(std::move(dataSender)),
    running_{false} {
        if (!calculator_) {
            throw std::invalid_argument("ICalculatorService cannot be null");
        }
        if (!dataSender_) {
            throw std::invalid_argument("IDelayCalcTrackDataOutgoingPort cannot be null");
        }
        Logger::info("ProcessTrackUseCase initialized (Event Queue with dedicated thread)");
    }

// ==================== Destructor ====================
ProcessTrackUseCase::~ProcessTrackUseCase() {
    stop();
    Logger::debug("ProcessTrackUseCase destroyed");
}

// ==================== Lifecycle Management ====================

bool ProcessTrackUseCase::start() {
    if (running_.load()) {
        Logger::warn("ProcessTrackUseCase already running");
        return true;
    }
    
    running_.store(true);
    
    // Start dedicated processing thread
    processingThread_ = std::thread([this]() {
        process();
    });
    
    Logger::info("ProcessTrackUseCase started with dedicated thread");
    return true;
}

void ProcessTrackUseCase::stop() {
    if (!running_.load()) {
        return;
    }
    
    Logger::info("Stopping ProcessTrackUseCase...");
    running_.store(false);
    
    // Wake up the processing thread
    queueCV_.notify_all();
    
    if (processingThread_.joinable()) {
        processingThread_.join();
    }
    
    Logger::info("ProcessTrackUseCase stopped");
}

bool ProcessTrackUseCase::isRunning() const {
    return running_.load();
}

// ==================== Event Queue Interface ====================
// submitExtrapTrackData is the entry point from incoming adapters
// Called by ExtrapTrackDataZeroMQIncomingAdapter when new data arrives
// Non-blocking: Enqueues message and returns immediately (~20ns)

void ProcessTrackUseCase::submitExtrapTrackData(const ports::ExtrapTrackData& data) {
    if (!running_.load()) {
        Logger::warn("ProcessTrackUseCase not running, dropping track: ", data.getTrackId());
        return;
    }
    
    // Validate input data before queueing
    // isValid() checks all fields are within acceptable ranges
    // Invalid data is rejected early to prevent processing errors
    if (!data.isValid()) {
        Logger::warn("Invalid track data received: ID=", data.getTrackId());
        return;
    }
    
    // Non-blocking enqueue (~20ns)
    enqueueMessage(data);
}

void ProcessTrackUseCase::enqueueMessage(const ports::ExtrapTrackData& data) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        // Prevent unbounded queue growth
        if (eventQueue_.size() >= MAX_QUEUE_SIZE) {
            Logger::warn("Event queue full, dropping oldest message for track: ", data.getTrackId());
            eventQueue_.pop();
        }
        
        eventQueue_.push(data);
    }
    
    queueCV_.notify_one();
}

// ==================== Background Processing Loop ====================

void ProcessTrackUseCase::process() {
    Logger::debug("Domain processing thread started");
    
    while (running_.load()) {
        ports::ExtrapTrackData data;
        
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
        processTrackData(data);
    }
    
    Logger::debug("Domain processing thread stopped");
}

void ProcessTrackUseCase::processTrackData(const ports::ExtrapTrackData& data) {
    try {
        // Log incoming track data for debugging and monitoring
        Logger::info("=== PROCESSING TRACK DATA ===");
        Logger::info("Track ID: ", data.getTrackId());
        Logger::info("Position ECEF: (", data.getXPositionECEF(), ", ", 
                     data.getYPositionECEF(), ", ", data.getZPositionECEF(), ")");
        Logger::info("Velocity ECEF: (", data.getXVelocityECEF(), ", ", 
                     data.getYVelocityECEF(), ", ", data.getZVelocityECEF(), ")");
        Logger::info("Update Time: ", data.getUpdateTime());
        Logger::info("==============================");
        
        // Process the track data through domain logic (via ICalculatorService abstraction)
        // calculateDelay() computes timing metrics and creates DelayCalcTrackData
        // This is the core domain operation: ExtrapTrackData → DelayCalcTrackData
        ports::DelayCalcTrackData processedData = calculator_->calculateDelay(data);
        
        Logger::info("Calculated delay for track ", data.getTrackId(), 
                    " -> Delay: ", processedData.getFirstHopDelayTime(), "μs");
        
        // Send processed data to all outgoing adapters
        // dataSender_ is an abstraction (IDelayCalcTrackDataOutgoingPort)
        // Multiple adapters can implement this interface:
        // - DelayCalcTrackDataZeroMQOutgoingAdapter (primary transmission)
        // - DelayCalcTrackDataCustomOutgoingAdapter (analytics/logging)
        if (dataSender_) {
            dataSender_->sendDelayCalcTrackData(processedData);
            Logger::debug("Successfully sent processed track data for ID=", data.getTrackId());
        } else {
            Logger::error("Error while sending data: dataSender is null");
        }
        
    } catch (const std::exception& e) {
        Logger::error("Error processing track ", data.getTrackId(), ": ", e.what());
    }
}

}
}
