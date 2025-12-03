/**
 * @file ExtrapTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ Outgoing Adapter for ExtrapTrackData messages (DIP compliant)
 * @details Implements both IAdapter interface and ExtrapTrackDataOutgoingPort
 *          for sending extrapolated track data via IMessageSocket abstraction.
 * 
 * Architecture (DIP Compliant):
 * ┌───────────────────────────────────────────────────────────┐
 * │           ExtrapTrackDataZeroMQOutgoingAdapter            │
 * ├───────────────────────────────────────────────────────────┤
 * │  ExtrapTrackData Model                                    │
 * │           ↓                                               │
 * │  Binary Serialization                                     │
 * │           ↓                                               │
 * │  IMessageSocket (Abstraction) ← ZeroMQ/Mock               │
 * └───────────────────────────────────────────────────────────┘
 * 
 * @author a_hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @note DIP compliant - depends on IMessageSocket abstraction
 * @see IAdapter.hpp
 * @see ExtrapTrackDataOutgoingPort.hpp
 * @see IMessageSocket.hpp
 */

#ifndef ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H
#define ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H

#include "adapters/common/IAdapter.hpp"
#include "adapters/common/messaging/IMessageSocket.hpp"
#include "domain/ports/outgoing/ExtrapTrackDataOutgoingPort.hpp"
#include "domain/model/ExtrapTrackData.hpp"

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>  // MISRA Rule 9-3-1: Fixed-width integers

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
     * @brief Construct adapter with DIP - socket injection (preferred for testing)
     * @param socket Message socket abstraction (ZeroMQ or Mock)
     * @pre socket is not null
     * @post Adapter is configured but not started
     */
    explicit ExtrapTrackDataZeroMQOutgoingAdapter(
        std::unique_ptr<adapters::common::messaging::IMessageSocket> socket);

    /**
     * @brief Default constructor (legacy - creates ZeroMQ socket internally)
     * @post Adapter is configured but not started
     * @deprecated Use socket injection constructor for new code
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
     * @brief Load configuration (for legacy constructor)
     */
    void loadConfiguration();

    /**
     * @brief Initialize socket (for legacy constructor)
     * @return true if successful
     */
    bool initializeSocket();

    // Configuration
    std::string endpoint_;          ///< ZeroMQ endpoint
    std::string group_;             ///< ZeroMQ group name for RADIO

    // Socket abstraction (DIP compliant)
    std::unique_ptr<adapters::common::messaging::IMessageSocket> socket_;  ///< Socket abstraction

    // State
    std::atomic<bool> running_;     ///< Running state flag
    bool ownsSocket_{false};        ///< Flag to track socket ownership

    // ==================== Socket Configuration Constants ====================
    // Production Environment (UDP Multicast)
    // static constexpr const char* DEFAULT_ENDPOINT = "udp://239.1.1.5:9596";
    
    // Development Environment (TCP Localhost)
    static constexpr const char* DEFAULT_ENDPOINT{"tcp://127.0.0.1:15001"};
    static constexpr const char* DEFAULT_GROUP{"ExtrapTrackData"};  ///< Group name for RADIO socket
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
