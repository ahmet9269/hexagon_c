#include "FinalCalculationService.hpp"
#include <chrono>
#include <iostream>

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
        //std::cout << "✅ Sent FinalCalcTrackData via custom adapter" << std::endl;
    } else {
        std::cerr << "⚠️  Warning: Custom outgoing adapter not ready" << std::endl;
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
    finalData.setTotalDelayTime(finalData.getSecondHopDelayTime()+delayCalcData.getFirstHopDelayTime());



    //std::cout << "First;" << delayCalcData.getFirstHopDelayTime() << ";Second;"<< finalData.getSecondHopDelayTime()<<";Total;"<<delayCalcData.getFirstHopDelayTime()+finalData.getSecondHopDelayTime() << std::endl;
    
    return finalData;
}

void FinalCalculationService::logProcessingResults(const FinalCalcTrackData& finalData) {
//    std::cout << "Created FinalCalcTrackData for Track ID: " << finalData.getTrackId() << std::endl
//              << " FirstHopDelayTime: " << finalData.getFirstHopDelayTime() << " microseconds" << std::endl
//              << " SecondHopDelayTime: " << finalData.getSecondHopDelayTime() << " microseconds" << std::endl
//              << " Total ZeroMQ Delay: " << finalData.getFirstHopDelayTime() + finalData.getSecondHopDelayTime() << " microseconds" << std::endl
//              << " Total Delay: " << finalData.getTotalDelayTime() << " microseconds" << std::endl;

    std::cout << "First;" << finalData.getFirstHopDelayTime() << ";Second;"<< finalData.getSecondHopDelayTime()<<";Total;"<<finalData.getFirstHopDelayTime()+finalData.getSecondHopDelayTime() << std::endl;

}

} // namespace logic
} // namespace domain
