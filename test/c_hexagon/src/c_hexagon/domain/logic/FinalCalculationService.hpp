/**
 * @file FinalCalculationService.hpp
 * @brief Domain service for final track data calculations
 * @details Implements the business logic for processing delay calculation data
 *          and producing final track statistics with multi-hop timing information.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IDelayCalcTrackDataIncomingPort
 * @see ITrackDataStatisticOutgoingPort
 */

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

/**
 * @class FinalCalculationService
 * @brief Domain service implementing final track data calculation logic
 * @details Processes incoming DelayCalcTrackData and produces FinalCalcTrackData
 *          with complete multi-hop delay analysis. Implements the primary port interface
 *          for integration with incoming adapters.
 * 
 * @par Thread Safety
 * This service is designed to be used from a single thread (Thread-per-Type pattern).
 * Each message type has its own service instance.
 * 
 * @par Data Flow
 * 1. Receives DelayCalcTrackData from incoming adapter
 * 2. Calculates hop delays and total delay
 * 3. Creates FinalCalcTrackData with complete timing information
 * 4. Sends result via outgoing port to external systems
 * 
 * @invariant outgoing_port_ may be null (standalone mode)
 * @see IDelayCalcTrackDataIncomingPort
 * @see ITrackDataStatisticOutgoingPort
 */
class FinalCalculationService : public ports::incoming::IDelayCalcTrackDataIncomingPort {
private:
    std::shared_ptr<ports::outgoing::ITrackDataStatisticOutgoingPort> outgoing_port_;  ///< Outgoing port for sending results
    
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