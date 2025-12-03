/**
 * @file IDelayCalcTrackDataOutgoingPort.hpp
 * @brief Outgoing port interface for writing processed track data
 * @details Defines the outbound port for sending DelayCalcTrackData to external
 *          destinations. Part of the Hexagonal Architecture ports layer.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @see DelayCalcTrackDataZeroMQOutgoingAdapter
 */

#pragma once

#include "domain/ports/DelayCalcTrackData.hpp"

namespace domain {
namespace ports {
namespace outgoing {

/**
 * @interface IDelayCalcTrackDataOutgoingPort
 * @brief Outbound port for writing processed DelayCalcTrackData
 * @details This interface is called by application services and implemented by
 *          outgoing adapters (ZeroMQ, file, console, etc.) to send processed data.
 *          Enables dependency injection and mock testing.
 * 
 * Usage:
 * @code
 * class MyAdapter : public IDelayCalcTrackDataOutgoingPort {
 * public:
 *     void sendDelayCalcTrackData(const DelayCalcTrackData& data) override {
 *         // Send data to external system
 *     }
 * };
 * @endcode
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
     * @throws std::runtime_error if send operation fails
     */
    virtual void sendDelayCalcTrackData(const DelayCalcTrackData& data) = 0;
    
protected:
    /// @brief Protected default constructor
    IDelayCalcTrackDataOutgoingPort() = default;
    
    /// @brief Protected copy constructor
    IDelayCalcTrackDataOutgoingPort(const IDelayCalcTrackDataOutgoingPort&) = default;
    
    /// @brief Protected move constructor
    IDelayCalcTrackDataOutgoingPort(IDelayCalcTrackDataOutgoingPort&&) = default;
    
    /// @brief Protected copy assignment operator
    IDelayCalcTrackDataOutgoingPort& operator=(const IDelayCalcTrackDataOutgoingPort&) = default;
    
    /// @brief Protected move assignment operator
    IDelayCalcTrackDataOutgoingPort& operator=(IDelayCalcTrackDataOutgoingPort&&) = default;
};

} // namespace outgoing
} // namespace ports
} // namespace domain