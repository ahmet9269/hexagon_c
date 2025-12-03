/**
 * @file ITrackDataIncomingPort.hpp
 * @brief Input port interface for TrackData processing
 * @details Defines the contract between incoming adapters and domain services.
 *          Part of the hexagonal architecture's primary ports.
 * 
 * @author A Hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see TrackDataExtrapolator.hpp
 * @see TrackDataZeroMQIncomingAdapter.hpp
 */

#ifndef TRACK_DATA_INCOMING_PORT_H 
#define TRACK_DATA_INCOMING_PORT_H 

#include "domain/ports/TrackData.hpp"

namespace domain {
namespace ports {
namespace incoming {

/**
 * @class ITrackDataIncomingPort
 * @brief Abstract interface for track data input port (primary port)
 * @details This interface defines the contract for receiving and processing
 *          TrackData from incoming adapters. Implements the "Driven" side
 *          of the hexagonal architecture.
 * 
 * Usage:
 * @code
 * class TrackDataExtrapolator : public ITrackDataIncomingPort {
 *     void processAndForwardTrackData(const TrackData& data) override {
 *         // Process and extrapolate track data
 *     }
 * };
 * @endcode
 * 
 * @note All implementations must be thread-safe as adapters may call
 *       from different threads.
 */
class ITrackDataIncomingPort {
public: 
    /**
     * @brief Virtual destructor for proper cleanup
     * @note MISRA Rule 12-8-1: Base classes shall have virtual destructor
     */
    virtual ~ITrackDataIncomingPort() = default;
    
    /**
     * @brief Process received track data and forward to output
     * @param trackData Input track data to process
     * @pre trackData.isValid() returns true
     * @post Processed data is forwarded to outgoing port
     * @note Implementation should handle extrapolation and timing
     * @thread_safe Must be thread-safe for concurrent adapter calls
     */
    virtual void processAndForwardTrackData(const model::TrackData& trackData) = 0;
};

} // namespace incoming
} // namespace ports
} // namespace domain

#endif // TRACK_DATA_INCOMING_PORT_H
