#pragma once

#include "../model/DelayCalcTrackData.hpp"
#include "../model/FinalCalcTrackData.hpp"
#include "../ports/outgoing/FinalTrackDataOutgoingPort.hpp"
#include "../ports/incoming/TrackDataSubmission.hpp"
#include <memory>

namespace domain {
namespace logic {

using domain::model::DelayCalcTrackData;
using domain::model::FinalCalcTrackData;

class FinalCalculationService : public ports::incoming::TrackDataSubmission {
private:
    std::unique_ptr<ports::outgoing::FinalTrackDataOutgoingPort> outgoing_port_;
    
public:
    explicit FinalCalculationService(
        std::unique_ptr<ports::outgoing::FinalTrackDataOutgoingPort> outgoing_port);
    
    // Primary Port Implementation
    void submitDelayCalcTrackData(const DelayCalcTrackData& delayCalcData) override;
    
    // Business Logic Public Interface
    void processDelayCalcData(const DelayCalcTrackData& delayCalcData);
    
private:
    /**
     * @brief Create FinalCalcTrackData from DelayCalcTrackData with timing calculations
     * @param delayCalcData Input data from B_hexagon
     * @return Processed final track data with complete delay analysis
     */
    FinalCalcTrackData createFinalCalcTrackData(const DelayCalcTrackData& delayCalcData);
    
    /**
     * @brief Log processing results for monitoring and debugging
     * @param finalData Final calculated track data to log
     */
    void logProcessingResults(const FinalCalcTrackData& finalData);
};

} // namespace logic
} // namespace domain