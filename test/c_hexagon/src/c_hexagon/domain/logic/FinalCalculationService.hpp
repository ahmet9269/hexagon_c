#pragma once

#include "domain/ports/DelayCalcTrackData.hpp"
#include "domain/ports/FinalCalcTrackData.hpp"
#include "domain/ports/outgoing/ITrackDataStatisticOutgoingPort.hpp"
#include "domain/ports/incoming/IDelayCalcTrackDataIncomingPort.hpp"
#include <memory>

namespace domain {
namespace logic {

using domain::ports::DelayCalcTrackData;
using domain::ports::FinalCalcTrackData;

class FinalCalculationService : public ports::incoming::IDelayCalcTrackDataIncomingPort {
private:
    std::shared_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port_;
    
public:
    /**
     * @brief Default constructor - operates without outgoing adapter
     */
    FinalCalculationService();
    
    /**
     * @brief Constructor with outgoing port (unique_ptr - takes ownership)
     * @param outgoing_port Port for sending processed data
     */
    explicit FinalCalculationService(
        std::unique_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port);
    
    /**
     * @brief Constructor with outgoing port (shared_ptr - shared ownership)
     * @param outgoing_port Port for sending processed data
     */
    explicit FinalCalculationService(
        std::shared_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port);
    
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