// #pragma once = Modern header guard syntax
// Ensures this header file is included only once per compilation unit
// Modern approach replacing traditional #ifndef/#define/#endif
#pragma once

// DOMAIN MODEL INCLUDES - Business logic models
#include "domain/ports/DelayCalcTrackData.hpp"    // Incoming DelayCalcTrackData from B_hexagon

// STL (Standard Template Library) INCLUDES
#include <memory>      // For smart pointers (std::shared_ptr, std::unique_ptr)

namespace domain {
namespace ports {
namespace incoming {

/**
 * @brief Primary Port for incoming DelayCalcTrackData
 * 
 * This is the hexagonal architecture primary port interface that domain logic implements.
 * Receives DelayCalcTrackData from B_hexagon via ZeroMQ DISH socket.
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