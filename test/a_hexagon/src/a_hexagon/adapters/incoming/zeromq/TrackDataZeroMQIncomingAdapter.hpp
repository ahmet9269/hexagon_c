/**
 * @file TrackDataZeroMQIncomingAdapter.hpp
 * @brief ZeroMQ UDP DISH Incoming Adapter for TrackData messages
 * @details Implements the IAdapter interface for receiving TrackData messages
 *          via ZeroMQ DISH socket (UDP multicast). Runs in its own thread
 *          as part of Thread-per-Type architecture.
 * 
 * Architecture (DIP Compliant):
 * ┌───────────────────────────────────────────────────────────┐
 * │              TrackDataZeroMQIncomingAdapter               │
 * ├───────────────────────────────────────────────────────────┤
 * │  IMessageSocket (Abstraction) ← ZeroMQ/Mock               │
 * │           ↓                                               │
 * │  Binary Deserialization → TrackData Model                 │
 * │           ↓                                               │
 * │  ITrackDataIncomingPort::processAndForwardTrackData()     │
 * └───────────────────────────────────────────────────────────┘
 * 
 * @author a_hexagon Team
 * @version 3.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @note DIP compliant - depends on IMessageSocket abstraction
 * @see IAdapter.hpp
 * @see ITrackDataIncomingPort.hpp
 * @see IMessageSocket.hpp
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "adapters/common/messaging/IMessageSocket.hpp"
#include "domain/ports/incoming/ITrackDataIncomingPort.hpp"
#include "domain/ports/TrackData.hpp"

#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <vector>

namespace adapters {
namespace incoming {
namespace zeromq {

/**
 * @class TrackDataZeroMQIncomingAdapter
 * @brief Receives TrackData via IMessageSocket abstraction
 * @details Implements IAdapter interface for lifecycle management.
 *          Each instance runs in its own dedicated thread with
 *          real-time priority and CPU affinity support.
 *          DIP compliant - socket is injected for testability.
 * 
 * @invariant When running_, receiveThread_ is active
 * @invariant socket_ is properly configured before start()
 */
class TrackDataZeroMQIncomingAdapter : public ::adapters::IAdapter {
public:
    /**
     * @brief Construct adapter with DIP - socket injection (preferred for testing)
     * @param incomingPort Domain service port for processing received data
     * @param socket Message socket abstraction (ZeroMQ or Mock)
     * @pre incomingPort is not null
     * @pre socket is not null
     * @post Adapter is configured but not started
     */
    explicit TrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort,
        std::unique_ptr<adapters::common::messaging::IMessageSocket> socket);

    /**
     * @brief Construct adapter with domain port (raw pointer - legacy)
     * @param incomingPort Domain service port for processing received data
     * @pre incomingPort is not null
     * @post Adapter is configured but not started
     * @deprecated Use socket injection constructor for new code
     */
    explicit TrackDataZeroMQIncomingAdapter(
        domain::ports::incoming::ITrackDataIncomingPort* incomingPort);
    
    /**
     * @brief Construct adapter with domain port (shared_ptr - legacy)
     * @param incomingPort Domain service port for processing received data
     * @pre incomingPort is not null
     * @post Adapter is configured but not started
     * @deprecated Use socket injection constructor for new code
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
     * @brief Load configuration (for legacy constructors)
     * @details Reads from config file or uses defaults
     */
    void loadConfiguration();

    /**
     * @brief Initialize socket (for legacy constructors)
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
    std::string endpoint_;          ///< ZeroMQ endpoint
    int32_t receiveTimeout_;        ///< Receive timeout in ms

    // Socket abstraction (DIP compliant)
    std::unique_ptr<adapters::common::messaging::IMessageSocket> socket_;  ///< Socket abstraction

    // Thread management
    std::thread receiveThread_;     ///< Dedicated receive thread
    std::atomic<bool> running_;     ///< Running state flag
    std::atomic<bool> stopRequested_;  ///< Stop signal flag

    // Domain port
    domain::ports::incoming::ITrackDataIncomingPort* incomingPort_;  ///< Domain service port (raw)
    std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPortShared_;  ///< Domain service port (shared)

    // Flag to track if we own the socket (for legacy constructors)
    bool ownsSocket_{false};

    // ==================== Socket Configuration Constants ====================
    // Production Environment (UDP Multicast)
    // static constexpr const char* DEFAULT_ENDPOINT = "udp://239.1.1.1:9000";
    
    // Development Environment (TCP Localhost)
    static constexpr const char* DEFAULT_ENDPOINT{"tcp://127.0.0.1:15000"};
    static constexpr int32_t DEFAULT_RECEIVE_TIMEOUT{100};    ///< Receive timeout (ms)
    
    // Thread Configuration
    static constexpr int32_t REALTIME_THREAD_PRIORITY{95};    ///< SCHED_FIFO priority
    static constexpr int32_t DEDICATED_CPU_CORE{1};           ///< CPU affinity core
};

} // namespace zeromq
} // namespace incoming
} // namespace adapters
