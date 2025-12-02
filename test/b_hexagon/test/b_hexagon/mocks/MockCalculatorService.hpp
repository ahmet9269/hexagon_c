/**
 * @file MockCalculatorService.hpp
 * @brief GMock implementation of ICalculatorService for unit testing
 * @details Enables testing ProcessTrackUseCase without domain logic dependencies.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#ifndef B_HEXAGON_TEST_MOCK_CALCULATOR_SERVICE_HPP
#define B_HEXAGON_TEST_MOCK_CALCULATOR_SERVICE_HPP

#include <gmock/gmock.h>
#include "domain/logic/ICalculatorService.hpp"
#include "domain/ports/ExtrapTrackData.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"

namespace mocks {

/**
 * @class MockCalculatorService
 * @brief GMock-based mock calculator service for unit testing
 */
class MockCalculatorService : public domain::logic::ICalculatorService {
public:
    MockCalculatorService() = default;
    ~MockCalculatorService() override = default;

    // GMock method declaration - matches ICalculatorService signature
    MOCK_METHOD(domain::ports::DelayCalcTrackData, calculateDelay, 
                (const domain::ports::ExtrapTrackData& trackData), (const, override));
};

}  // namespace mocks

#endif  // B_HEXAGON_TEST_MOCK_CALCULATOR_SERVICE_HPP
