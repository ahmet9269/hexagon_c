/**
 * @file FinalCalculationService.cpp
 * @brief Implementation of the Final Calculation Service
 * @details Implements the business logic for processing delay calculation data
 *          and producing final track statistics with hop timing information.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see FinalCalculationService.hpp
 */

#include "FinalCalculationService.hpp"
#include "utils/Logger.hpp"
#include <chrono>

namespace domain {
namespace logic {

/**
 * @brief Default constructor - creates service without outgoing port
 */
FinalCalculationService::FinalCalculationService()
    : outgoing_port_(nullptr) {
    LOG_DEBUG("FinalCalculationService initialized without outgoing adapter");
}

/**
 * @brief Constructor with outgoing port injection
 * @param outgoing_port Unique pointer to the outgoing port adapter
 */
FinalCalculationService::FinalCalculationService(
    std::unique_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port)
    : outgoing_port_(std::move(outgoing_port)) {
    LOG_DEBUG("FinalCalculationService initialized with outgoing adapter");
}

/**
 * @brief Submit delay calculation data for processing
 * @param delayCalcData Input data containing track and timing information
 */
void FinalCalculationService::submitDelayCalcTrackData(const DelayCalcTrackData& delayCalcData) {
    processDelayCalcData(delayCalcData);
}

/**
 * @brief Process delay calculation data and forward to outgoing port
 * @param delayCalcData Input data to be processed
 * @details Creates FinalCalcTrackData, logs results, and sends via outgoing adapter
 */
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

/**
 * @brief Create FinalCalcTrackData from DelayCalcTrackData
 * @param delayCalcData Source data with delay calculations
 * @return FinalCalcTrackData with computed hop delays and total delay
 * @details Copies track data, calculates second hop delay using current time,
 *          and computes total delay as sum of first and second hop delays
 */
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

/**
 * @brief Log processing results for monitoring and debugging
 * @param finalData Processed track data with delay information
 */
void FinalCalculationService::logProcessingResults(const FinalCalcTrackData& finalData) {
    LOG_INFO("Track ID: {} | Hop1: {} μs | Hop2: {} μs | Total: {} μs",
             finalData.getTrackId(),
             finalData.getFirstHopDelayTime(),
             finalData.getSecondHopDelayTime(),
             finalData.getFirstHopDelayTime() + finalData.getSecondHopDelayTime());
}

} // namespace logic
} // namespace domain
