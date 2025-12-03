/**
 * @file FinalCalcTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ RADIO socket adapter for sending FinalCalcTrackData
 * @details Implements the outgoing adapter in hexagonal architecture for
 *          transmitting processed track data to external systems.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/outgoing/ITrackDataStatisticOutgoingPort.hpp"
#include "domain/ports/FinalCalcTrackData.hpp"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace adapters {
namespace outgoing {
namespace zeromq {

/**
 * @brief ZeroMQ RADIO Adapter for FinalCalcTrackData transmission
 * @details Provides group-based message transmission using RADIO/DISH pattern.
 *          Implements both IAdapter (for lifecycle) and ITrackDataStatisticOutgoingPort
 *          (for hexagonal architecture integration).
 * 
 * Thread Safety:
 * - Uses mutex-protected queue for thread-safe message passing
 * - Background worker thread handles actual ZMQ transmission
 * - Non-blocking sendFinalTrackData() for real-time performance
 */
class FinalCalcTrackDataZeroMQOutgoingAdapter 
    : public adapters::IAdapter
    , public domain::ports::outgoing::ITrackDataStatisticOutgoingPort {
    
public:
    /**
     * @brief Constructor with default configuration
     * @details Uses TCP localhost for development/container environment
     */
    FinalCalcTrackDataZeroMQOutgoingAdapter();

    /**
     * @brief Constructor with custom configuration
     * @param endpoint ZeroMQ endpoint (e.g., "tcp://127.0.0.1:15003")
     * @param group_name Multicast group name for RADIO socket
     */
    FinalCalcTrackDataZeroMQOutgoingAdapter(
        const std::string& endpoint,
        const std::string& group_name);

    /**
     * @brief Destructor - ensures graceful shutdown
     */
    ~FinalCalcTrackDataZeroMQOutgoingAdapter() override;

    // Delete copy operations
    FinalCalcTrackDataZeroMQOutgoingAdapter(const FinalCalcTrackDataZeroMQOutgoingAdapter&) = delete;
    FinalCalcTrackDataZeroMQOutgoingAdapter& operator=(const FinalCalcTrackDataZeroMQOutgoingAdapter&) = delete;

    // ═══════════════════════════════════════════════════════════════════
    // IAdapter Interface Implementation
    // ═══════════════════════════════════════════════════════════════════

    /**
     * @brief Start the RADIO publisher
     * @return true if started successfully
     */
    [[nodiscard]] bool start() override;

    /**
     * @brief Stop the RADIO publisher
     */
    void stop() override;

    /**
     * @brief Check if adapter is running
     * @return true if running
     */
    [[nodiscard]] bool isRunning() const override;

    /**
     * @brief Get adapter name for logging
     * @return Adapter identifier
     */
    [[nodiscard]] std::string getName() const override;

    // ═══════════════════════════════════════════════════════════════════
    // ITrackDataStatisticOutgoingPort Interface Implementation
    // ═══════════════════════════════════════════════════════════════════

    /**
     * @brief Send FinalCalcTrackData to external system
     * @param data Final calculated track data to transmit
     * @details Non-blocking - queues message for background transmission
     */
    void sendFinalTrackData(const domain::ports::FinalCalcTrackData& data) override;

    /**
     * @brief Check if the outgoing connection is ready
     * @return true if socket is bound and ready to send
     */
    [[nodiscard]] bool isReady() const override;

private:
    /**
     * @brief Initialize the ZeroMQ RADIO socket
     * @throws zmq::error_t if socket initialization fails
     */
    void initializeRadioSocket();

    /**
     * @brief Background worker thread for message transmission
     */
    void publisherWorker();

    /**
     * @brief Enqueue message for transmission
     * @param data Track data to send
     */
    void enqueueMessage(const domain::ports::FinalCalcTrackData& data);

private:
    // Configuration
    std::string endpoint_;          ///< ZeroMQ endpoint
    std::string group_name_;        ///< Multicast group name
    std::string adapter_name_;      ///< Adapter identifier

    // ZeroMQ components
    zmq::context_t zmq_context_;
    std::unique_ptr<zmq::socket_t> radio_socket_;

    // Thread management
    std::thread publisher_thread_;
    std::atomic<bool> running_;
    std::atomic<bool> ready_;

    // Thread-safe message queue
    mutable std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::queue<domain::ports::FinalCalcTrackData> message_queue_;
    static constexpr std::size_t MAX_QUEUE_SIZE = 1000;  ///< Prevent unbounded growth
};

} // namespace zeromq
} // namespace outgoing
} // namespace adapters
