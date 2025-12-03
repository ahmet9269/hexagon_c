/**
 * @file TrackDataExtrapolator.hpp
 * @brief Domain service for track data extrapolation
 * @details Implements the core business logic for extrapolating track data
 *          from input frequency (8Hz) to output frequency (100Hz).
 *          Part of the hexagonal architecture domain layer.
 * 
 * @author A Hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see ITrackDataIncomingPort.hpp
 * @see IExtrapTrackDataOutgoingPort.hpp
 */

#ifndef TRACK_DATA_EXTRAPOLATOR_H 
#define TRACK_DATA_EXTRAPOLATOR_H 

#include <domain/ports/outgoing/IExtrapTrackDataOutgoingPort.hpp>
#include "domain/ports/incoming/ITrackDataIncomingPort.hpp"
#include <vector>
#include <memory>
#include "domain/model/TrackData.hpp"
#include "domain/model/ExtrapTrackData.hpp"

namespace domain {
namespace logic {

using domain::model::TrackData;     
using domain::model::ExtrapTrackData;
using std::vector; 

/**
 * @class TrackDataExtrapolator
 * @brief Domain service for extrapolating track data
 * @details Implements ITrackDataIncomingPort for hexagonal architecture
 * @note MISRA C++ 2023 compliant implementation
 */
class TrackDataExtrapolator : public domain::ports::incoming::ITrackDataIncomingPort {
private:
    /// @brief Outgoing port for sending extrapolated data - MISRA compliant smart pointer
    std::unique_ptr<domain::ports::outgoing::IExtrapTrackDataOutgoingPort> outgoingPort_;
    
    /// @brief Raw pointer for legacy compatibility (non-owning)
    domain::ports::outgoing::IExtrapTrackDataOutgoingPort* rawOutgoingPort_ = nullptr;

public: 
    /**
     * @brief Constructor with ownership transfer
     * @param outgoingPort Unique pointer to outgoing port (ownership transferred)
     */
    explicit TrackDataExtrapolator(
        std::unique_ptr<domain::ports::outgoing::IExtrapTrackDataOutgoingPort> outgoingPort);
    
    /**
     * @brief Legacy constructor for backward compatibility
     * @param outgoingPort Raw pointer to outgoing port (non-owning)
     * @deprecated Use unique_ptr constructor instead
     */
    explicit TrackDataExtrapolator(
        domain::ports::outgoing::IExtrapTrackDataOutgoingPort* outgoingPort);
    
    /**
     * @brief Process and forward track data
     * @param trackData Input track data to process
     */
    void processAndForwardTrackData(const domain::model::TrackData& trackData);
    
    /**
     * @brief Extrapolate track data at specified frequencies
     * @param trackData Input track data
     * @param inputFrequency Input data frequency (Hz)
     * @param outputFrequency Output data frequency (Hz)
     */
    void extrapolateTrackData(const TrackData& trackData, double inputFrequency, double outputFrequency);

    // Delete copy operations for MISRA compliance
    TrackDataExtrapolator(const TrackDataExtrapolator&) = delete;
    TrackDataExtrapolator& operator=(const TrackDataExtrapolator&) = delete;
    
    // Default move operations
    TrackDataExtrapolator(TrackDataExtrapolator&&) = default;
    TrackDataExtrapolator& operator=(TrackDataExtrapolator&&) = default;
    
    ~TrackDataExtrapolator() = default;
};

} // namespace logic
} // namespace domain

#endif
