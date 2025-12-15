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

#include "domain/ports/incoming/ExtrapTrackData.hpp"

namespace domain {
namespace ports {
namespace incoming {

/**
 * @interface IExtrapTrackDataIncomingPort
 * @brief Inbound port for handling ExtrapTrackData from external sources
 * 
 * Hexagonal Architecture Pattern:
 * This interface defines the "left side" port (incoming/driver port)
 * - Implemented by: Domain services (ProcessTrackUseCase)
 * - Called by: Incoming adapters (ExtrapTrackDataZeroMQIncomingAdapter)
 * 
 * Flow: External Source → Incoming Adapter → IExtrapTrackDataIncomingPort → Domain
 * 
 * Benefits:
 * - Decouples domain from infrastructure (ZeroMQ, file, console, etc.)
 * - Enables testing with mock adapters
 * - Allows multiple adapter implementations without changing domain
 */
class IExtrapTrackDataIncomingPort {
public:
    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~IExtrapTrackDataIncomingPort() = default;

    /**
     * @brief Submit new track data to domain layer for processing
     * @param data The received track data to process
     * @details This method is called by incoming adapters to submit data
     *          into the domain processing queue. Non-blocking operation.
     *          
     * Performance: ~20ns enqueue latency (adapter returns immediately)
     * Thread Safety: Must be thread-safe (called from adapter worker thread)
     * Validation: Should validate data.isValid() before processing
     * 
     * Example:
     * @code
     * // In incoming adapter's worker thread:
     * ExtrapTrackData data = deserialize(message);
     * incomingPort->submitExtrapTrackData(data);  // Returns immediately
     * @endcode
     */
    virtual void submitExtrapTrackData(const ExtrapTrackData& data) = 0;
};

} // namespace incoming
} // namespace ports
} // namespace domain