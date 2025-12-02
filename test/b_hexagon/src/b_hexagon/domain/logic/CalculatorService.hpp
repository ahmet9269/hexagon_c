/**
 * @file CalculatorService.hpp
 * @brief Concrete implementation of ICalculatorService
 * @details SOLID compliant - implements abstract interface for dependency injection
 */

#pragma once

#include "ICalculatorService.hpp"
#include "domain/ports/ExtrapTrackData.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include <chrono>

namespace domain {
namespace logic {

/**
 * @class CalculatorService
 * @brief Domain service for calculating timing delays and processing track data
 * @details Concrete implementation of ICalculatorService.
 *          Can be replaced with MockCalculatorService in unit tests.
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
     */
    [[nodiscard]] ports::DelayCalcTrackData calculateDelay(
        const ports::ExtrapTrackData& trackData) const override;

private:
    /**
     * @brief Get current time in microseconds since epoch
     * @return Current timestamp in microseconds
     */
    [[nodiscard]] long getCurrentTimeMicroseconds() const noexcept;

    /**
     * @brief Calculate processing delay from timestamp
     * @param originalTime Original update time in microseconds
     * @param currentTime Current time in microseconds
     * @return Calculated delay in microseconds
     */
    [[nodiscard]] long calculateTimeDelta(long originalTime, long currentTime) const noexcept;
};

} // namespace logic
} // namespace domain
