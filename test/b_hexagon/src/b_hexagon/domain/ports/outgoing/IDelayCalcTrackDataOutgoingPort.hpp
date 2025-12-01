/**
 * @file IDelayCalcTrackDataOutgoingPort.hpp
 * @brief Outgoing port interface for writing processed track data
 */

#pragma once

#include "domain/ports/DelayCalcTrackData.hpp"

namespace domain {
namespace ports {
namespace outgoing {

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;

/**
 * @interface IDelayCalcTrackDataOutgoingPort
 * @brief Outbound port for writing processed DelayCalcTrackData
 * 
 * This interface is called by application services and implemented by
 * outgoing adapters (ZeroMQ, file, console, etc.) to send processed data.
 */
class IDelayCalcTrackDataOutgoingPort {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~IDelayCalcTrackDataOutgoingPort() = default;

    /**
     * @brief Send processed track data to external destination
     * @param data The processed track data with delay calculations
     */
    virtual void sendDelayCalcTrackData(const DelayCalcTrackData& data) = 0;
};
}
}
}