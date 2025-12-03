/**
 * @file IExtrapTrackDataIncomingPort.hpp
 * @brief Incoming port interface for handling track data
 * @details Defines the inbound port for receiving ExtrapTrackData from external
 *          sources. Part of the Hexagonal Architecture ports layer.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @see ExtrapTrackDataZeroMQIncomingAdapter
 */

#pragma once

#include "domain/ports/ExtrapTrackData.hpp"

namespace domain {
namespace ports {
namespace incoming {

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

} // namespace incoming
} // namespace ports
} // namespace domain