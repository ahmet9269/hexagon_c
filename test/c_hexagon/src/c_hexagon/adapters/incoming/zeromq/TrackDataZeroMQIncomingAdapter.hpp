#pragma once

#include "adapters/common/AdapterManager.hpp"
#include "domain/ports/incoming/IDelayCalcTrackDataIncomingPort.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <sstream>
#include <optional>
#include <cstring>
#include <chrono>

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;

namespace adapters {
namespace incoming {
namespace zeromq {

/**
 * @brief ZeroMQ DISH Adapter for receiving DelayCalcTrackData via UDP multicast
 * @details Thread-per-Type architecture compliant - runs in dedicated thread.
 *          Uses RADIO/DISH pattern for group-based UDP multicast messaging.
 * 
 * Network Flow:
 * - B_hexagon (RADIO) --[UDP Multicast]--> C_hexagon (DISH)
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @details Provides group-based message reception over UDP multicast.
 *          Integrates the DISH pattern into hexagonal architecture.
 *          Implements IAdapter for AdapterManager compatibility.
 */
class TrackDataZeroMQIncomingAdapter : public adapters::IAdapter {
private:
    // ==================== Configuration Constants ====================
    // Real-time thread configuration
    static constexpr int REALTIME_THREAD_PRIORITY = 95;
    static constexpr int DEDICATED_CPU_CORE = 2;  // Different from b_hexagon
    static constexpr int RECEIVE_TIMEOUT_MS = 100;
    
    // Network configuration constants (UDP RADIO/DISH pattern)
    static constexpr const char* DEFAULT_MULTICAST_ADDRESS = "239.1.1.5";
    static constexpr int DEFAULT_PORT = 9595;  // Receives from b_hexagon port 9595
    static constexpr const char* DEFAULT_PROTOCOL = "udp";
    static constexpr const char* DEFAULT_GROUP = "DelayCalcTrackData";
    
    // Socket configuration
    static constexpr int LINGER_MS = 0;
    static constexpr int HIGH_WATER_MARK = 0;  // Unlimited
    
    // ==================== Member Variables ====================
    std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission_;
    
    // Configuration (initialized first for logging)
    std::string endpoint_;            // UDP multicast endpoint
    std::string group_;               // Group name for DISH subscription
    std::string adapter_name_;        // Adapter identifier for logging
    
    // ZeroMQ C++ context and socket
    zmq::context_t zmq_context_;
    std::unique_ptr<zmq::socket_t> dish_socket_;
    
    // Thread management
    std::thread subscriber_thread_;
    std::atomic<bool> running_;

public:
    /**
     * @brief Constructor - Default UDP multicast configuration
     * @param track_data_submission Port for sending data to domain layer
     */
    TrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission);

    /**
     * @brief Constructor with custom configuration
     * @param track_data_submission Port for sending data to domain layer
     * @param multicast_endpoint UDP multicast endpoint (e.g., "udp://239.1.1.1:9001")
     * @param group_name Multicast group name to subscribe (e.g., "SOURCE_DATA")
     */
    TrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission,
        const std::string& multicast_endpoint,
        const std::string& group_name);

    ~TrackDataZeroMQIncomingAdapter() override;

    // IAdapter interface implementation
    /**
     * @brief Starts the DISH subscriber
     * @return true if started successfully
     */
    [[nodiscard]] bool start() override;

    /**
     * @brief Stops the DISH subscriber
     */
    void stop() override;

    /**
     * @brief Returns subscriber active status
     * @return true if subscriber is running
     */
    [[nodiscard]] bool isRunning() const override;
    
    /**
     * @brief Get adapter name for logging
     * @return Adapter identifier
     */
    [[nodiscard]] std::string getName() const override;

private:
    /**
     * @brief Initializes the ZeroMQ DISH socket
     */
    void initializeDishSocket();

    /**
     * @brief Subscriber worker thread - asynchronous message receiving
     */
    void subscriberWorker();

    /**
     * @brief Deserializes binary data to DelayCalcTrackData
     * @param binary_data Raw binary data from ZeroMQ message
     * @return Optional containing deserialized data if successful
     */
    std::optional<domain::ports::DelayCalcTrackData> deserializeDelayCalcTrackData(
        const std::vector<uint8_t>& binary_data);
};

} // namespace zeromq
} // namespace incoming
} // namespace adapters
