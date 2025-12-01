/**
 * @file FinalTrackDataOutgoingPort.hpp
 * @brief Interface for outgoing final track data communication
 */

#ifndef FINAL_TRACK_DATA_OUTGOING_PORT_HPP
#define FINAL_TRACK_DATA_OUTGOING_PORT_HPP

#include "../../model/FinalCalcTrackData.hpp"

namespace domain {
namespace ports {
namespace outgoing {

/**
 * @brief Abstract interface for outgoing final track data transmission
 * 
 * This port defines the contract for sending processed final track data
 * to external systems. Implementations can use any technology (ZeroMQ, 
 * HTTP, TCP, File, Database, etc.)
 */
class FinalTrackDataOutgoingPort {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~FinalTrackDataOutgoingPort() = default;
    
    /**
     * @brief Send final calculated track data to external system
     * 
     * @param data Final calculated track data to be transmitted
     * @throws std::runtime_error if transmission fails
     */
    virtual void sendFinalTrackData(const domain::model::FinalCalcTrackData& data) = 0;
    
    /**
     * @brief Check if the outgoing connection is ready/healthy
     * 
     * @return true if connection is ready, false otherwise
     */
    virtual bool isReady() const = 0;
};

} // namespace outgoing
} // namespace ports
} // namespace domain

#endif // FINAL_TRACK_DATA_OUTGOING_PORT_HPP