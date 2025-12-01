/**
 * @file IExtrapTrackDataIncomingPort.hpp
 * @brief Incoming port interface for handling track data
 */

#pragma once

#include "domain/ports/ExtrapTrackData.hpp"

// Using declarations for convenience
namespace domain{
namespace ports{
namespace incoming{

/**
 * @interface IExtrapTrackDataIncomingPort
 * @brief Inbound port for handling ExtrapTrackData from external sources
 * 
 * This interface is implemented by application services and called by
 * incoming adapters (ZeroMQ, file, console, etc.) when new data arrives.
 */
class IExtrapTrackDataIncomingPort {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~IExtrapTrackDataIncomingPort() = default;

    /**
     * @brief Called when new track data is received
     * @param data The received track data to process
     */
    virtual void onDataReceived(const ExtrapTrackData& data) = 0;
};
}}}