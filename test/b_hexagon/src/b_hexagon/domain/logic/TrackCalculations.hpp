/**
 * @file TrackCalculations.hpp
 * @brief Pure stateless calculation utilities for track timing metrics
 * @details Provides static methods for calculating first hop delays and
 *          second hop sent times for ExtrapTrackData processing.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "domain/ports/ExtrapTrackData.hpp"   // Input data structure
#include <chrono>                              // Time utilities for nanosecond precision

namespace domain {
namespace logic {

/**
 * @brief Pure stateless calculation utilities for `ExtrapTrackData` timing metrics.
 * @details Centralizes arithmetic logic to keep the application service (orchestrator) 
 *          lightweight and focused on workflow. Facilitates targeted unit testing of 
 *          business rules. All returned / consumed time values are expressed as 
 *          nanoseconds since epoch, represented as `long` for convenience.
 *
 * @dot
 * digraph timing_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *   
 *   input [label="Input: ExtrapTrackData\n+ firstHopSentTime", fillcolor=lightblue];
 *   receive [label="System captures\nReceive Time", fillcolor=yellow];
 *   calc1 [label="Calculate First Hop Delay\n= receiveTime - firstHopSentTime", fillcolor=orange];
 *   calc2 [label="Calculate Second Hop Sent Time\n= current system time", fillcolor=orange];
 *   output [label="Output: DelayCalculated\nTrackData", fillcolor=lightgreen];
 *   
 *   input -> receive;
 *   receive -> calc1;
 *   calc1 -> calc2;
 *   calc2 -> output;
 *   
 *   {rank=same; calc1; calc2;}
 * }
 * @enddot
 */
/**
 * @class TrackCalculations
 * @brief Pure stateless calculation utilities for track timing metrics
 * @details Thread-safe utility class providing static methods for timing calculations.
 *          All methods are const-correct and exception-safe.
 */
class TrackCalculations {
public:
    /// @brief Deleted constructor - purely static utility class
    TrackCalculations() = delete;
    
    /**
     * @brief Calculates first hop delay time in nanoseconds
     * @param trackData The input ExtrapTrackData containing firstHopSentTime
     * @param receiveTimeNs The time when data was received (nanoseconds since epoch)
     * @return Delay time in nanoseconds (receiveTimeNs - firstHopSentTime)
     * @note Thread-safe - uses only input parameters
     */
    [[nodiscard]] static long calculateFirstHopDelayTime(
        const ports::ExtrapTrackData& trackData, 
        long receiveTimeNs) noexcept {
        return receiveTimeNs - trackData.getFirstHopSentTime();
    }

    /**
     * @brief Calculates second hop sent time as current system time in nanoseconds
     * @return Current system time in nanoseconds since epoch
     * @note Thread-safe - uses system clock
     */
    [[nodiscard]] static long calculateSecondHopSentTime() noexcept {
        auto now = std::chrono::system_clock::now();
        return static_cast<long>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()).count());
    }
};

} // namespace logic
} // namespace domain
