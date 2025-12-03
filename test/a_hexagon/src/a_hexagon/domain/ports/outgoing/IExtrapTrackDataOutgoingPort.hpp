/**
 * @file IExtrapTrackDataOutgoingPort.hpp
 * @brief Output port interface for ExtrapTrackData transmission
 * @details Defines the contract between domain services and outgoing adapters.
 *          Part of the hexagonal architecture's secondary ports.
 * 
 * @author A Hexagon Team
 * @version 2.1
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see ExtrapTrackDataZeroMQOutgoingAdapter.hpp
 * @see TrackDataExtrapolator.hpp
 */

#ifndef I_EXTRAP_TRACK_DATA_OUTGOING_PORT_HPP
#define I_EXTRAP_TRACK_DATA_OUTGOING_PORT_HPP

#include "domain/model/ExtrapTrackData.hpp"
#include <vector>

namespace domain {
namespace ports {
namespace outgoing {

/**
 * @class IExtrapTrackDataOutgoingPort
 * @brief Abstract interface for extrapolated track data output port (secondary port)
 * @details This interface defines the contract for sending extrapolated
 *          TrackData to outgoing adapters. Implements the "Driver" side
 *          of the hexagonal architecture.
 * 
 * Usage:
 * @code
 * class ZeroMQOutgoingAdapter : public IExtrapTrackDataOutgoingPort {
 *     void sendExtrapTrackData(const ExtrapTrackData& data) override {
 *         // Serialize and send via ZeroMQ
 *     }
 * };
 * @endcode
 * 
 * @note Implementations should handle serialization and network transmission.
 * @note Non-blocking implementations are preferred for real-time performance.
 */
class IExtrapTrackDataOutgoingPort {
public: 
    /**
     * @brief Virtual destructor for proper cleanup
     * @note MISRA Rule 12-8-1: Base classes shall have virtual destructor
     */
    virtual ~IExtrapTrackDataOutgoingPort() = default;
    
    /**
     * @brief Send batch of extrapolated track data
     * @param data Vector of ExtrapTrackData to transmit
     * @pre All data items in vector are valid
     * @post Data is queued/sent to downstream consumers
     * @note May be non-blocking depending on implementation
     */
    virtual void sendExtrapTrackData(const std::vector<model::ExtrapTrackData>& data) = 0;
    
    /**
     * @brief Send single extrapolated track data (preferred for real-time)
     * @param data Single ExtrapTrackData to transmit
     * @pre data.isValid() returns true
     * @post Data is queued/sent to downstream consumers
     * @latency ~20ns for queue-based implementations
     * @thread_safe Must be thread-safe for concurrent calls
     */
    virtual void sendExtrapTrackData(const model::ExtrapTrackData& data) = 0;
    
protected:
    /// @brief Protected default constructor
    IExtrapTrackDataOutgoingPort() = default;
    
    /// @brief Protected copy constructor
    IExtrapTrackDataOutgoingPort(const IExtrapTrackDataOutgoingPort&) = default;
    
    /// @brief Protected move constructor
    IExtrapTrackDataOutgoingPort(IExtrapTrackDataOutgoingPort&&) = default;
    
    /// @brief Protected copy assignment operator
    IExtrapTrackDataOutgoingPort& operator=(const IExtrapTrackDataOutgoingPort&) = default;
    
    /// @brief Protected move assignment operator
    IExtrapTrackDataOutgoingPort& operator=(IExtrapTrackDataOutgoingPort&&) = default;
};

} // namespace outgoing
} // namespace ports
} // namespace domain

#endif // I_EXTRAP_TRACK_DATA_OUTGOING_PORT_HPP
