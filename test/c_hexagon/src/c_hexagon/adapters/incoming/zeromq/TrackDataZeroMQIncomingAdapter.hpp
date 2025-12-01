#pragma once

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
 * @brief ZeroMQ Subscriber Adapter using DISH pattern
 * @details Provides group-based message reception over UDP multicast.
 *          Integrates the DISH pattern into hexagonal architecture.
 */
class TrackDataZeroMQIncomingAdapter {
private:
    std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission_;
    
    // ZeroMQ C++ context and socket
    zmq::context_t zmq_context_;
    std::unique_ptr<zmq::socket_t> dish_socket_;
    
    // Thread management
    std::thread subscriber_thread_;
    std::atomic<bool> running_;
    
    // Configuration
    std::string multicast_endpoint_;  // UDP multicast address (e.g., udp://239.1.1.1:9001)
    std::string group_name_;          // Group name to subscribe (e.g., "SOURCE_DATA")
    
    // Latency measurement
    struct LatencyMeasurement {
        std::chrono::steady_clock::time_point receive_time;
        long long send_timestamp_us;  // microseconds
        long long latency_us;         // microseconds
        std::string original_data;
    };
    
    // Helper function for error checking (no longer needed with C++ wrapper)
    // void check_rc(int rc, const std::string& context_msg);

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

    ~TrackDataZeroMQIncomingAdapter();

    /**
     * @brief Starts the DISH subscriber
     * @return true if started successfully
     */
  [[nodiscard]]  bool start();

    /**
     * @brief Stops the DISH subscriber
     */
    void stop();

    /**
     * @brief Returns subscriber active status
     * @return true if subscriber is running
     */
   [[nodiscard]] bool isRunning() const;

private:
    /**
     * @brief Initializes the ZeroMQ DISH socket
     */
    void initializeDishSocket();

    /**
     * @brief Subscriber worker thread - asynchronous message receiving
     */
    void subscriberWorker();

    // Note: processReceivedMessage is not needed for binary DelayCalcTrackData
    // LatencyMeasurement processReceivedMessage(const std::string& received_payload);

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
