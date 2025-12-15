/**
 * @file CalculatorService.cpp
 * @brief Implementation of CalculatorService for track data processing
 * @details Concrete implementation of ICalculatorService interface
 */

#include "domain/logic/CalculatorService.hpp"
#include "utils/Logger.hpp"

namespace domain {
namespace logic {

ports::DelayCalcTrackData CalculatorService::calculateDelay(const ports::ExtrapTrackData& trackData) const {
    Logger::debug("Processing track ", trackData.getTrackId(), " - calculating delay metrics");
    
    // Get current processing time for second hop
    // This represents when we're processing the data (second hop timestamp)
    long currentTime = getCurrentTimeMicroseconds();
    
    // Validate input timestamp to prevent negative delays or overflow
    if (trackData.getFirstHopSentTime() <= 0) {
        Logger::warn("Invalid firstHopSentTime for track ", trackData.getTrackId(), ": ", trackData.getFirstHopSentTime());
    }
    
    // Create result with calculated values
    ports::DelayCalcTrackData result;
    
    // Copy all original track data (position, velocity, timestamps)
    // DelayCalcTrackData extends ExtrapTrackData with additional delay fields
    result.setTrackId(trackData.getTrackId());
    result.setXVelocityECEF(trackData.getXVelocityECEF());
    result.setYVelocityECEF(trackData.getYVelocityECEF());
    result.setZVelocityECEF(trackData.getZVelocityECEF());
    result.setXPositionECEF(trackData.getXPositionECEF());
    result.setYPositionECEF(trackData.getYPositionECEF());
    result.setZPositionECEF(trackData.getZPositionECEF());
    result.setUpdateTime(trackData.getUpdateTime());
    result.setOriginalUpdateTime(trackData.getOriginalUpdateTime());
    result.setFirstHopSentTime(trackData.getFirstHopSentTime());
    
    // Calculate first hop delay (current time - first hop sent time)
    // This measures latency from data generation to our processing
    // Unit: microseconds (1 second = 1,000,000 microseconds)
    result.setFirstHopDelayTime(calculateTimeDelta(trackData.getFirstHopSentTime(), currentTime));
    
    // Set second hop sent time as current time
    // This timestamp marks when we're forwarding the processed data
    result.setSecondHopSentTime(currentTime);
    
    Logger::info("Track ", trackData.getTrackId(), " delay calculation complete - first hop delay: ", 
                 result.getFirstHopDelayTime(), " μs, second hop time: ", result.getSecondHopSentTime());
    Logger::info("CURRENT TIME <>>>>>>>  ",currentTime);
    Logger::info("getFirstHopSentTime TIME <>>>>>>>  ",trackData.getFirstHopSentTime());
    
    return result;
}

long CalculatorService::getCurrentTimeMicroseconds() const noexcept {
    try {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        
        // Use microsecond precision (not nanosecond) for consistency with data model
        // Microsecond = 10^-6 seconds (1 millisecond = 1000 microseconds)
        // This provides sufficient precision for delay measurements (sub-millisecond)
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    } catch (...) {
        return 0L;  // Fallback value if time acquisition fails (should never happen)
    }
}

long CalculatorService::calculateTimeDelta(long originalTime, long currentTime) const noexcept {
    // Validate timestamps to prevent negative delays or overflow
    if (originalTime <= 0 || currentTime <= 0) {
        return 0L;  // Invalid timestamps (zero or negative epoch time)
    }
    
    // Check for time going backwards (clock skew or wraparound)
    if (currentTime <= originalTime) {
        return 0L;  // No delay if current time is not later (prevents negative values)
    }
    
    // Calculate delay in microseconds (simple subtraction)
    return currentTime - originalTime;
}

} // namespace logic
} // namespace domain
