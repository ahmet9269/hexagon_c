/**
 * @file ITrackDataStatisticOutgoingPort.hpp
 * @brief Secondary port interface for outgoing final track data transmission
 * @details Defines the hexagonal architecture secondary port that outgoing adapters implement.
 *          This interface enables decoupling between domain services and external systems.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see FinalCalcTrackData
 * @see FinalCalcTrackDataZeroMQOutgoingAdapter
 */

#ifndef FINAL_TRACK_DATA_OUTGOING_PORT_HPP
#define FINAL_TRACK_DATA_OUTGOING_PORT_HPP

#include "domain/ports/outgoing/FinalCalcTrackData.hpp"

namespace domain {
namespace ports {
namespace outgoing {

/**
 * @class ITrackDataStatisticOutgoingPort
 * @brief Secondary Port interface for outgoing final track data transmission
 * @details Abstract interface that defines the contract for sending processed final 
 *          track data to external systems. This is a secondary port in hexagonal 
 *          architecture - implemented by outgoing adapters.
 * 
 * @par Hexagonal Architecture Role
 * Secondary Port - defines the boundary between domain logic and external world.
 * Implemented by outgoing adapters (e.g., FinalCalcTrackDataZeroMQOutgoingAdapter).
 * 
 * @par Supported Implementations
 * - ZeroMQ RADIO socket (FinalCalcTrackDataZeroMQOutgoingAdapter)
 * - HTTP REST client (potential future implementation)
 * - File writer (potential future implementation)
 * - Mock implementation (for testing)
 * 
 * @par Usage Example
 * @code
 * class MyOutgoingAdapter : public ITrackDataStatisticOutgoingPort {
 * public:
 *     void sendFinalTrackData(const FinalCalcTrackData& data) override {
 *         // Transmit data to external system
 *     }
 *     bool isReady() const override { return true; }
 * };
 * @endcode
 */
class ITrackDataStatisticOutgoingPort {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~ITrackDataStatisticOutgoingPort() = default;
    
    /**
     * @brief Send final calculated track data to external system
     * 
     * @param data Final calculated track data to be transmitted
     * @throws std::runtime_error if transmission fails
     */
    virtual void sendFinalTrackData(const domain::ports::FinalCalcTrackData& data) = 0;
    
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