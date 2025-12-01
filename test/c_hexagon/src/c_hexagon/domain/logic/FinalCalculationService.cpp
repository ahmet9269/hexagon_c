#include "FinalCalculationService.hpp"
#include "utils/Logger.hpp"
#include <chrono>

namespace domain {
namespace logic {

FinalCalculationService::FinalCalculationService(
    std::unique_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port)
    : outgoing_port_(std::move(outgoing_port)) {
}

void FinalCalculationService::submitDelayCalcTrackData(const DelayCalcTrackData& delayCalcData) {
    processDelayCalcData(delayCalcData);
}

void FinalCalculationService::processDelayCalcData(const DelayCalcTrackData& delayCalcData) {
    // Business Logic: Create FinalCalcTrackData
    FinalCalcTrackData finalData = createFinalCalcTrackData(delayCalcData);
    
    // Log processing results
    logProcessingResults(finalData);
    
    // Send via outgoing port (Hexagonal Architecture)
    if (outgoing_port_ && outgoing_port_->isReady()) {
        outgoing_port_->sendFinalTrackData(finalData);
        LOG_DEBUG("Sent FinalCalcTrackData via outgoing adapter - Track ID: {}", finalData.getTrackId());
    } else {
        LOG_WARN("Custom outgoing adapter not ready");
    }
}

FinalCalcTrackData FinalCalculationService::createFinalCalcTrackData(const DelayCalcTrackData& delayCalcData) {
    FinalCalcTrackData finalData;
    
    // Copy basic track data
    finalData.setTrackId(delayCalcData.getTrackId());
    finalData.setXPositionECEF(delayCalcData.getXPositionECEF());
    finalData.setYPositionECEF(delayCalcData.getYPositionECEF());
    finalData.setZPositionECEF(delayCalcData.getZPositionECEF());
    finalData.setXVelocityECEF(delayCalcData.getXVelocityECEF());
    finalData.setYVelocityECEF(delayCalcData.getYVelocityECEF());
    finalData.setZVelocityECEF(delayCalcData.getZVelocityECEF());
    
    // Set timing information
    auto currentTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    finalData.setThirdHopSentTime(currentTime);
    finalData.setSecondHopSentTime(delayCalcData.getSecondHopSentTime());
    finalData.setFirstHopDelayTime(delayCalcData.getFirstHopDelayTime());
    finalData.setSecondHopDelayTime(currentTime - delayCalcData.getSecondHopSentTime());
    finalData.setTotalDelayTime(finalData.getSecondHopDelayTime() + delayCalcData.getFirstHopDelayTime());
    
    return finalData;
}

void FinalCalculationService::logProcessingResults(const FinalCalcTrackData& finalData) {
    LOG_INFO("Track ID: {} | Hop1: {} μs | Hop2: {} μs | Total: {} μs",
             finalData.getTrackId(),
             finalData.getFirstHopDelayTime(),
             finalData.getSecondHopDelayTime(),
             finalData.getFirstHopDelayTime() + finalData.getSecondHopDelayTime());
}

} // namespace logic
} // namespace domain
