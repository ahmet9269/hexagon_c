/**
 * @file ProcessTrackUseCase.hpp
 * @brief Domain use case for processing track data with delay calculations
 * @details SOLID compliant use case that orchestrates the processing flow
 *          from incoming ExtrapTrackData to outgoing DelayCalcTrackData.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "domain/logic/ICalculatorService.hpp"
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "domain/ports/ExtrapTrackData.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include <memory>

namespace domain {
namespace logic {

/**
 * @class ProcessTrackUseCase
 * @brief Domain use case for processing track data
 * @details SOLID compliant - depends on abstractions (ICalculatorService, IDelayCalcTrackDataOutgoingPort)
 *          Thread-per-Type architecture compliant - supports shared ownership of outgoing port
 * 
 * Dependency Inversion:
 * - ICalculatorService: Abstraction for delay calculation (mockable)
 * - IDelayCalcTrackDataOutgoingPort: Abstraction for outbound data (mockable)
 * 
 * Test Coverage Benefits:
 * - Can inject MockCalculatorService to test business logic without real calculations
 * - Can inject MockOutgoingPort to verify output without network
 */
class ProcessTrackUseCase final : public ports::incoming::IExtrapTrackDataIncomingPort {
public:
   /**
    * @brief Construct with unique_ptr ownership (legacy support)
    * @param calculator Calculator service (ownership transferred)
    * @param dataSender Outgoing port (ownership transferred)
    */
   explicit ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::unique_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender);

   /**
    * @brief Construct with shared_ptr ownership (Thread-per-Type architecture)
    * @param calculator Calculator service (ownership transferred)
    * @param dataSender Outgoing port (shared ownership)
    */
   explicit ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::shared_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender);

    ~ProcessTrackUseCase() override = default;

    ProcessTrackUseCase(const ProcessTrackUseCase&) = delete;
    ProcessTrackUseCase& operator=(const ProcessTrackUseCase&) = delete;
    ProcessTrackUseCase(ProcessTrackUseCase&&) = default;
    ProcessTrackUseCase& operator=(ProcessTrackUseCase&&) = default;

    /**
     * @brief Handle incoming track data
     * @param data Received ExtrapTrackData to process
     */
    void onDataReceived(const ports::ExtrapTrackData& data) override;

private:
    void sendDelayCalcTrackData(const ports::DelayCalcTrackData& data);
    
    std::unique_ptr<ICalculatorService> calculator_;                           ///< Calculator service (abstract)
    std::shared_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender_; ///< Outgoing port (abstract)
};

} // namespace logic
} // namespace domain