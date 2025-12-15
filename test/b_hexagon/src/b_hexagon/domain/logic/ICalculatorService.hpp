/**
 * @file ICalculatorService.hpp
 * @brief Abstract interface for Calculator Service (Dependency Inversion)
 * @details Enables mock injection for unit testing domain logic without 
 *          concrete implementation dependencies.
 * 
 * SOLID Compliance:
 * ┌─────────────────────────────────────────────────────────────────────┐
 * │                    DEPENDENCY INVERSION PRINCIPLE                   │
 * ├─────────────────────────────────────────────────────────────────────┤
 * │                                                                     │
 * │  ┌─────────────────┐         ┌─────────────────────────────────┐   │
 * │  │ProcessTrackUseCase│ ────▶ │    ICalculatorService           │   │
 * │  │   (High Level)  │         │      (Abstraction)              │   │
 * │  └─────────────────┘         └─────────────────────────────────┘   │
 * │                                         ▲                          │
 * │                              ┌──────────┴──────────┐               │
 * │                              │                     │               │
 * │                    ┌─────────────────┐   ┌─────────────────┐       │
 * │                    │CalculatorService│   │MockCalculatorSvc│       │
 * │                    │   (Concrete)    │   │    (Test)       │       │
 * │                    └─────────────────┘   └─────────────────┘       │
 * │                                                                     │
 * └─────────────────────────────────────────────────────────────────────┘
 * 
 * Benefits:
 * - Unit tests can verify ProcessTrackUseCase without real calculations
 * - Easy to test edge cases (error conditions, boundary values)
 * - Swap implementations without changing domain logic
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @see CalculatorService.hpp
 */

#ifndef B_HEXAGON_DOMAIN_LOGIC_ICALCULATOR_SERVICE_HPP
#define B_HEXAGON_DOMAIN_LOGIC_ICALCULATOR_SERVICE_HPP

#include "domain/ports/incoming/ExtrapTrackData.hpp"
#include "domain/ports/outgoing/DelayCalcTrackData.hpp"

namespace domain {
namespace logic {

/**
 * @class ICalculatorService
 * @brief Abstract interface for delay calculation services
 * @details Defines the contract for calculating track data delays.
 *          Enables mock injection for testing ProcessTrackUseCase.
 * 
 * Usage:
 * @code
 * // Production:
 * auto calculator = std::make_unique<CalculatorService>();
 * auto useCase = std::make_unique<ProcessTrackUseCase>(std::move(calculator), outPort);
 * 
 * // Test:
 * auto mockCalculator = std::make_unique<MockCalculatorService>();
 * mockCalculator->setExpectedDelay(42.0);  // Control test behavior
 * auto useCase = std::make_unique<ProcessTrackUseCase>(std::move(mockCalculator), mockPort);
 * @endcode
 */
class ICalculatorService {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~ICalculatorService() = default;

    /**
     * @brief Calculate delay from extrapolated track data
     * @param trackData Input track data with timing information
     * @return DelayCalcTrackData with computed delay values
     * @throws std::invalid_argument if trackData is invalid
     */
    [[nodiscard]] virtual ports::DelayCalcTrackData calculateDelay(
        const ports::ExtrapTrackData& trackData) const = 0;

protected:
    ICalculatorService() = default;
    ICalculatorService(const ICalculatorService&) = default;
    ICalculatorService(ICalculatorService&&) = default;
    ICalculatorService& operator=(const ICalculatorService&) = default;
    ICalculatorService& operator=(ICalculatorService&&) = default;
};

} // namespace logic
} // namespace domain

#endif // B_HEXAGON_DOMAIN_LOGIC_ICALCULATOR_SERVICE_HPP
