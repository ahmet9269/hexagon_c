#pragma once

#include "domain/logic/CalculatorService.hpp"
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "domain/ports/ExtrapTrackData.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include <memory>
namespace domain{
namespace logic {
/**
 * @class ProcessTrackUseCase
 * @brief Domain use case for processing track data
 */
class ProcessTrackUseCase final : public domain::ports::incoming::IExtrapTrackDataIncomingPort {
public:
   explicit ProcessTrackUseCase(
    std::unique_ptr<CalculatorService> calculator,
    std::unique_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort>dataSender);

    ~ProcessTrackUseCase() override = default;

    ProcessTrackUseCase(const ProcessTrackUseCase&) = delete;
    ProcessTrackUseCase& operator=(const ProcessTrackUseCase&) = delete;
    ProcessTrackUseCase(ProcessTrackUseCase&&) = default;
    ProcessTrackUseCase& operator=(ProcessTrackUseCase&&) = default;

    void onDataReceived(const domain::ports::ExtrapTrackData& data) override;

private:
    void sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data);
    std::unique_ptr<CalculatorService> calculator_;
    std::unique_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender_;
};
}
}