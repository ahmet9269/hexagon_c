/**
 * @file TrackDataZeroMQIncomingAdapter.hpp
 * @brief ZeroMQ UDP DISH Incoming Adapter for TrackData messages
 * @details Implements the IAdapter interface for receiving TrackData messages
 *          via ZeroMQ DISH socket (UDP multicast). Runs in its own thread
 *          as part of Thread-per-Type architecture.
 * 
 * Architecture:
 * ┌───────────────────────────────────────────────────────────┐
 * │              TrackDataZeroMQIncomingAdapter               │
 * ├───────────────────────────────────────────────────────────┤
 * │  ZeroMQ DISH Socket (UDP Multicast)                       │
 * │           ↓                                               │
 * │  Binary Deserialization → TrackData Model                 │
 * │           ↓                                               │
 * │  ITrackDataIncomingPort::processAndForwardTrackData()     │
 * └───────────────────────────────────────────────────────────┘
 * 
 * @author a_hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IAdapter.hpp
 * @see ITrackDataIncomingPort.hpp
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/incoming/ITrackDataIncomingPort.hpp"
#include "domain/ports/TrackData.hpp"
#include "zmq_config.hpp"

#include <zmq.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <vector>

// ZMQ_DISH draft API fallback (IntelliSense için)
#ifndef ZMQ_DISH
#define ZMQ_DISH 15
#endif

namespace adapters {
namespace incoming {
namespace zeromq {

/**
 * @class TrackDataZeroMQIncomingAdapter
 * @brief Receives TrackData via ZeroMQ DISH socket (UDP multicast)
 * @details Implements IAdapter interface for lifecycle management.
 *          Each instance runs in its own dedicated thread with
 *          real-time priority and CPU affinity support.
 * 
 * @invariant When running_, receiveThread_ is active
 * @invariant socket_ is properly configured before start()
 */
class TrackDataZeroMQIncomingAdapter : public ::adapters::IAdapter {
public:
    /**
     * @brief Construct adapter with domain port (raw pointer - legacy)
     * @param incomingPort Domain service port for processing received data
     * @pre incomingPort is not null
     * @post Adapter is configured but not started
     */
    explicit TrackDataZeroMQIncomingAdapter(
        domain::ports::incoming::ITrackDataIncomingPort* incomingPort);
    
    /**
     * @brief Construct adapter with domain port (shared_ptr - preferred)
     * @param incomingPort Domain service port for processing received data
     * @pre incomingPort is not null
     * @post Adapter is configured but not started
     */
    explicit TrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort);

    /**
     * @brief Destructor - ensures graceful shutdown
     * @post Receive thread is stopped
     * @post ZeroMQ socket is closed
     */
    ~TrackDataZeroMQIncomingAdapter() override;

    // IAdapter interface implementation
    
    /**
     * @brief Start the adapter's receive loop
     * @return true if started successfully
     * @post isRunning() returns true
     * @post Receive thread is active
     */
    [[nodiscard]] bool start() override;

    /**
     * @brief Stop the adapter gracefully
     * @details Signals receive loop to exit and joins thread
     * @post isRunning() returns false
     * @post Receive thread has terminated
     */
    void stop() override;

    /**
     * @brief Check if adapter is running
     * @return true if receive loop is active
     */
    [[nodiscard]] bool isRunning() const override;

    /**
     * @brief Get adapter name
     * @return "TrackDataZeroMQIncomingAdapter"
     */
    [[nodiscard]] std::string getName() const noexcept override;

    // Delete copy/move for thread safety
    TrackDataZeroMQIncomingAdapter(const TrackDataZeroMQIncomingAdapter&) = delete;
    TrackDataZeroMQIncomingAdapter& operator=(const TrackDataZeroMQIncomingAdapter&) = delete;
    TrackDataZeroMQIncomingAdapter(TrackDataZeroMQIncomingAdapter&&) = delete;
    TrackDataZeroMQIncomingAdapter& operator=(TrackDataZeroMQIncomingAdapter&&) = delete;

private:
    /**
     * @brief Load ZeroMQ configuration
     * @details Reads from config file or uses defaults
     */
    void loadConfiguration();

    /**
     * @brief Initialize ZeroMQ socket
     * @return true if socket initialized successfully
     */
    bool initializeSocket();

    /**
     * @brief Main receive loop (runs in dedicated thread)
     * @details Continuously receives and processes messages until stopped
     */
    void receiveLoop();

    /**
     * @brief Deserialize binary data to TrackData
     * @param data Raw binary message data
     * @param trackData Output TrackData object
     * @return true if deserialization successful
     */
    bool deserializeTrackData(const std::vector<uint8_t>& data, 
                               domain::model::TrackData& trackData);

    // Configuration - MISRA: Fixed-width integers
    std::string protocol_;          ///< Network protocol (udp)
    std::string endpoint_;          ///< ZeroMQ endpoint
    std::string groupName_;         ///< ZeroMQ group name for DISH
    int32_t socketType_;            ///< ZeroMQ socket type (DISH)
    int32_t receiveTimeout_;        ///< Receive timeout in ms

    // ZeroMQ components
    zmq::context_t context_;        ///< ZeroMQ context
    std::unique_ptr<zmq::socket_t> socket_;  ///< DISH socket

    // Thread management
    std::thread receiveThread_;     ///< Dedicated receive thread
    std::atomic<bool> running_;     ///< Running state flag
    std::atomic<bool> stopRequested_;  ///< Stop signal flag

    // Domain port
    domain::ports::incoming::ITrackDataIncomingPort* incomingPort_;  ///< Domain service port (raw)
    std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPortShared_;  ///< Domain service port (shared)

    // ==================== Socket Configuration Constants ====================
    // Production Environment (UDP Multicast)
    // static constexpr const char* DEFAULT_ENDPOINT = "udp://239.1.1.1:9000";
    // static constexpr const char* DEFAULT_PROTOCOL = "udp";
    // static constexpr int32_t DEFAULT_SOCKET_TYPE = ZMQ_DISH;
    
    // Development Environment (TCP Localhost)
    static constexpr const char* DEFAULT_ENDPOINT{"tcp://127.0.0.1:15000"};
    static constexpr const char* DEFAULT_PROTOCOL{"tcp"};
    static constexpr int32_t DEFAULT_SOCKET_TYPE{ZMQ_SUB};  ///< SUB for TCP, DISH for UDP
    
    static constexpr const char* DEFAULT_GROUP{"TrackData"};  ///< Group name for DISH socket
    static constexpr int32_t DEFAULT_RECEIVE_TIMEOUT{100};    ///< Receive timeout (ms)
    
    // Thread Configuration
    static constexpr int32_t REALTIME_THREAD_PRIORITY{95};    ///< SCHED_FIFO priority
    static constexpr int32_t DEDICATED_CPU_CORE{1};           ///< CPU affinity core
};

} // namespace zeromq
} // namespace incoming
} // namespace adapters
