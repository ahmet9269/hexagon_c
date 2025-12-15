/**
 * @file CalculatorService.hpp
 * @brief Concrete implementation of ICalculatorService
 * @details SOLID compliant - implements abstract interface for dependency injection
 */

#pragma once

#include "ICalculatorService.hpp"
#include "domain/ports/incoming/ExtrapTrackData.hpp"
#include "domain/ports/outgoing/DelayCalcTrackData.hpp"
#include <chrono>

namespace domain {
namespace logic {

/**
 * @class CalculatorService
 * @brief Domain service for calculating timing delays and processing track data
 * @details Concrete implementation of ICalculatorService.
 *          Can be replaced with MockCalculatorService in unit tests.
 *          
 * Core Responsibility:
 * Calculate delay metrics from extrapolated track data
 * - Input: ExtrapTrackData (76 bytes, 10 fields)
 * - Output: DelayCalcTrackData (96 bytes, 12 fields)
 * - Added: firstHopDelayTime, secondHopSentTime
 * 
 * Calculation Logic:
 * 1. Get current timestamp (microsecond precision)
 * 2. Calculate first hop delay: currentTime - firstHopSentTime
 * 3. Set second hop sent time: currentTime
 * 4. Copy all original fields to output
 * 
 * SOLID Compliance:
 * - Single Responsibility: Only handles delay calculations
 * - Open/Closed: Extends ICalculatorService, closed for modification
 * - Liskov Substitution: Can replace any ICalculatorService
 * - Interface Segregation: Implements focused ICalculatorService interface
 * - Dependency Inversion: ProcessTrackUseCase depends on ICalculatorService abstraction
 */
class CalculatorService final : public ICalculatorService {
public:
    /**
     * @brief Default constructor
     */
    CalculatorService() = default;

    /**
     * @brief Destructor
     */
    ~CalculatorService() override = default;

    // Disable copy operations
    CalculatorService(const CalculatorService&) = delete;
    CalculatorService& operator=(const CalculatorService&) = delete;

    // Enable move operations
    CalculatorService(CalculatorService&&) noexcept = default;
    CalculatorService& operator=(CalculatorService&&) noexcept = default;

    /**
     * @brief Calculate delay between original update time and current time
     * @param trackData Input track data with timing information
     * @return DelayCalcTrackData with computed delay value
     * @details Transforms ExtrapTrackData to DelayCalcTrackData by:
     *          1. Copying all input fields (position, velocity, timestamps)
     *          2. Calculating firstHopDelayTime (currentTime - firstHopSentTime)
     *          3. Setting secondHopSentTime (current processing time)
     *          
     * Thread Safety: const method, can be called from multiple threads
     * Performance: Fast operation (~microseconds), dominated by time syscall
     */
    [[nodiscard]] ports::DelayCalcTrackData calculateDelay(
        const ports::ExtrapTrackData& trackData) const override;

private:
    /**
     * @brief Get current time in microseconds since epoch
     * @return Current timestamp in microseconds
     * @details Uses std::chrono::system_clock for wall-clock time
     *          Precision: 1 microsecond (10^-6 seconds)
     *          Epoch: Unix epoch (1970-01-01 00:00:00 UTC)
     *          Range: 0 to ~292,471 years
     *          
     * noexcept: Never throws (returns 0 on failure)
     */
    [[nodiscard]] long getCurrentTimeMicroseconds() const noexcept;

    /**
     * @brief Calculate processing delay from timestamp
     * @param originalTime Original update time in microseconds
     * @param currentTime Current time in microseconds
     * @return Calculated delay in microseconds (0 if invalid/negative)
     * @details Validates timestamps before subtraction:
     *          - Returns 0 if either timestamp is zero/negative
     *          - Returns 0 if current <= original (prevents negative delay)
     *          - Otherwise returns: currentTime - originalTime
     *          
     * noexcept: Never throws (returns safe default on error)
     */
    [[nodiscard]] long calculateTimeDelta(long originalTime, long currentTime) const noexcept;
};

} // namespace logic
} // namespace domain
