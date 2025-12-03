/**
 * @file IDelayCalcTrackDataIncomingPort.hpp
 * @brief Primary port interface for incoming DelayCalcTrackData
 * @details Defines the hexagonal architecture primary port that domain logic implements.
 *          This interface enables decoupling between adapters and domain services.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see DelayCalcTrackData
 * @see FinalCalculationService
 */

#pragma once

#include "domain/ports/DelayCalcTrackData.hpp"
#include <memory>

namespace domain {
namespace ports {
namespace incoming {

/**
 * @class IDelayCalcTrackDataIncomingPort
 * @brief Primary Port interface for incoming DelayCalcTrackData
 * @details This is the hexagonal architecture primary port interface that domain logic implements.
 *          Receives DelayCalcTrackData from B_hexagon via ZeroMQ DISH socket.
 * 
 * @par Hexagonal Architecture Role
 * Primary Port - defines the boundary between external world and domain logic.
 * Implemented by domain services (e.g., FinalCalculationService).
 * 
 * @par Usage Example
 * @code
 * class MyService : public IDelayCalcTrackDataIncomingPort {
 * public:
 *     void submitDelayCalcTrackData(const DelayCalcTrackData& data) override {
 *         // Process incoming data
 *     }
 * };
 * @endcode
 */
class IDelayCalcTrackDataIncomingPort {
public:
    virtual ~IDelayCalcTrackDataIncomingPort() = default;

    /**
     * @brief Process incoming DelayCalcTrackData from B_hexagon
     * @param data DelayCalcTrackData containing delay calculations from B_hexagon
     */
    virtual void submitDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) = 0;
};

} // namespace incoming
} // namespace ports
} // namespace domain

// HEADER GUARD NOTES:
// This file is protected with #pragma once
// Even if included multiple times in the same compilation unit
// It will only be processed once, optimizing compilation time

// HEXAGONAL ARCHITECTURE FLOW:
// 1. External System (hexagon_b) -> Sends ZeroMQ message
// 2. ZeroMQTrackDataSubscriber (Primary Adapter) -> Receives the message
// 3. IDelayCalcTrackDataIncomingPort interface (Primary Port) -> Entry point to domain
// 4. TrackDataProcessor (Domain Logic) -> Applies business logic
// 5. TrackDataRepository & TrackDataPublisher (Secondary Ports) -> Output to external world