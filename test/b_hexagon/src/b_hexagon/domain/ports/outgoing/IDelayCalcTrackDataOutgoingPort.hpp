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

#include "domain/ports/outgoing/DelayCalcTrackData.hpp"

namespace domain {
namespace ports {
namespace outgoing {

/**
 * @interface IDelayCalcTrackDataOutgoingPort
 * @brief Outbound port for writing processed DelayCalcTrackData
 * @details This interface defines the "right side" port (outgoing/driven port)
 *          in the Hexagonal Architecture pattern.
 *          
 * Hexagonal Architecture Pattern:
 * - Implemented by: Outgoing adapters (ZeroMQ, file, database, etc.)
 * - Called by: Domain services (ProcessTrackUseCase)
 * 
 * Flow: Domain → IDelayCalcTrackDataOutgoingPort → Outgoing Adapter → External System
 * 
 * Benefits:
 * - Decouples domain from infrastructure (ZeroMQ, DDS, Kafka, etc.)
 * - Enables testing with mock adapters (verify send calls without network)
 * - Allows multiple adapter implementations (ZeroMQ + Custom analytics)
 * - Supports composite pattern (one port, multiple adapters)
 *          
 * Usage:
 * @code
 * class MyAdapter : public IDelayCalcTrackDataOutgoingPort {
 * public:
 *     void sendDelayCalcTrackData(const DelayCalcTrackData& data) override {
 *         // Send data to external system (ZeroMQ, file, etc.)
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