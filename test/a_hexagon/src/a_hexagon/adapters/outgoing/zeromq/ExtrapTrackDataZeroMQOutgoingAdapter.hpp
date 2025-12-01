/**
 * @file ExtrapTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ UDP RADIO Outgoing Adapter for ExtrapTrackData messages
 * @details Implements both IAdapter interface and ExtrapTrackDataOutgoingPort
 *          for sending extrapolated track data via ZeroMQ RADIO socket.
 * 
 * Architecture:
 * ┌───────────────────────────────────────────────────────────┐
 * │           ExtrapTrackDataZeroMQOutgoingAdapter            │
 * ├───────────────────────────────────────────────────────────┤
 * │  ExtrapTrackData Model                                    │
 * │           ↓                                               │
 * │  Binary Serialization                                     │
 * │           ↓                                               │
 * │  ZeroMQ RADIO Socket (UDP Multicast)                      │
 * └───────────────────────────────────────────────────────────┘
 * 
 * @author a_hexagon Team
 * @version 1.1
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IAdapter.hpp
 * @see ExtrapTrackDataOutgoingPort.hpp
 */

#ifndef ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H
#define ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/outgoing/ExtrapTrackDataOutgoingPort.hpp"
#include "domain/model/ExtrapTrackData.hpp"
#include "zmq_config.hpp"

#include <zmq.hpp>
#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>  // MISRA Rule 9-3-1: Fixed-width integers

// ZMQ_RADIO draft API fallback (IntelliSense için)
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif

namespace adapters {
namespace outgoing {
namespace zeromq {

/**
 * @class ExtrapTrackDataZeroMQOutgoingAdapter
 * @brief Sends ExtrapTrackData via ZeroMQ RADIO socket (UDP multicast)
 * @details Implements both IAdapter (for lifecycle management) and
 *          ExtrapTrackDataOutgoingPort (for domain service integration).
 * 
 * @invariant When running_, socket is connected and ready to send
 */
class ExtrapTrackDataZeroMQOutgoingAdapter 
    : public ::adapters::IAdapter
    , public domain::ports::outgoing::ExtrapTrackDataOutgoingPort {
public: 
    /**
     * @brief Default constructor
     * @post Adapter is configured but not started
     */
    ExtrapTrackDataZeroMQOutgoingAdapter();

    /**
     * @brief Destructor - ensures graceful shutdown
     */
    ~ExtrapTrackDataZeroMQOutgoingAdapter() override;

    // ==================== IAdapter Interface ====================
    
    /**
     * @brief Start the adapter
     * @return true if started successfully
     * @post isRunning() returns true
     */
    [[nodiscard]] bool start() override;

    /**
     * @brief Stop the adapter gracefully
     * @post isRunning() returns false
     */
    void stop() override;

    /**
     * @brief Check if adapter is running
     * @return true if adapter is active
     */
    [[nodiscard]] bool isRunning() const override;

    /**
     * @brief Get adapter name
     * @return "ExtrapTrackDataZeroMQOutgoingAdapter"
     * @note MISRA Rule 15-5-1: noexcept specification
     */
    [[nodiscard]] std::string getName() const noexcept override;

    // ==================== ExtrapTrackDataOutgoingPort Interface ====================

    /**
     * @brief Send vector of ExtrapTrackData
     * @param data Vector of ExtrapTrackData to send
     */
    void sendExtrapTrackData(const std::vector<domain::model::ExtrapTrackData>& data) override;

    /**
     * @brief Send single ExtrapTrackData
     * @param data ExtrapTrackData to send
     */
    void sendExtrapTrackData(const domain::model::ExtrapTrackData& data) override;

    // Delete copy/move for thread safety
    ExtrapTrackDataZeroMQOutgoingAdapter(const ExtrapTrackDataZeroMQOutgoingAdapter&) = delete;
    ExtrapTrackDataZeroMQOutgoingAdapter& operator=(const ExtrapTrackDataZeroMQOutgoingAdapter&) = delete;
    ExtrapTrackDataZeroMQOutgoingAdapter(ExtrapTrackDataZeroMQOutgoingAdapter&&) = delete;
    ExtrapTrackDataZeroMQOutgoingAdapter& operator=(ExtrapTrackDataZeroMQOutgoingAdapter&&) = delete;

private: 
    /**
     * @brief Load ZeroMQ configuration
     */
    void loadConfiguration();

    /**
     * @brief Initialize ZeroMQ socket
     * @return true if successful
     */
    bool initializeSocket();

    // Configuration
    std::string protocol_;          ///< Network protocol (udp)
    std::string endpoint_;          ///< ZeroMQ endpoint
    std::string groupName_;         ///< ZeroMQ group name for RADIO
    int32_t socketType_;            ///< ZeroMQ socket type (RADIO) - MISRA Rule 9-3-1

    // ZeroMQ components
    zmq::context_t context_;        ///< ZeroMQ context
    std::unique_ptr<zmq::socket_t> socket_;  ///< RADIO socket

    // State
    std::atomic<bool> running_;     ///< Running state flag

    // Constants
    static constexpr const char* DEFAULT_GROUP = "ExtrapTrackData";
};

} // namespace zeromq
} // namespace outgoing
} // namespace adapters

// Legacy namespace alias for backward compatibility
namespace domain {
namespace adapters {
namespace outgoing {
    using ExtrapTrackDataZeroMQOutgoingAdapter = ::adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter;
}
}
}

#endif
