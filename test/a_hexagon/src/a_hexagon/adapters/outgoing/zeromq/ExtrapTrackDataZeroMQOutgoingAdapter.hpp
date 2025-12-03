/**
 * @file ExtrapTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ Outgoing Adapter for ExtrapTrackData messages (DIP compliant)
 * @details Implements both IAdapter interface and IExtrapTrackDataOutgoingPort
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
 * @see IExtrapTrackDataOutgoingPort.hpp
 * @see IMessageSocket.hpp
 */

#ifndef ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H
#define ZEROMQ_EXTRAP_TRACK_DATA_ADAPTER_H

#include "adapters/common/IAdapter.hpp"
#include "adapters/common/messaging/IMessageSocket.hpp"
#include "domain/ports/outgoing/IExtrapTrackDataOutgoingPort.hpp"
#include "domain/model/ExtrapTrackData.hpp"

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <cstdint>  // MISRA Rule 9-3-1: Fixed-width integers
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace adapters {
namespace outgoing {
namespace zeromq {

/**
 * @class ExtrapTrackDataZeroMQOutgoingAdapter
 * @brief Sends ExtrapTrackData via ZeroMQ RADIO socket (UDP multicast)
 * @details Implements both IAdapter (for lifecycle management) and
 *          IExtrapTrackDataOutgoingPort (for domain service integration).
 *          Uses background worker thread for non-blocking sends (~20ns enqueue).
 * 
 * Thread Safety:
 * - Uses mutex-protected queue for thread-safe message passing
 * - Background worker thread handles actual ZMQ transmission
 * - Non-blocking sendExtrapTrackData() for real-time performance
 * 
 * @invariant When running_, socket is connected and ready to send
 */
class ExtrapTrackDataZeroMQOutgoingAdapter 
    : public ::adapters::IAdapter
    , public domain::ports::outgoing::IExtrapTrackDataOutgoingPort {
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
     * @details Non-blocking - queues message for background transmission (~20ns)
     */
    void sendExtrapTrackData(const domain::model::ExtrapTrackData& data) override;

    /**
     * @brief Check if the outgoing connection is ready
     * @return true if socket is connected and ready to send
     */
    [[nodiscard]] bool isReady() const noexcept;

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

    /**
     * @brief Background worker thread for message transmission
     * @details Runs with real-time priority and CPU affinity
     */
    void publisherWorker();

    /**
     * @brief Enqueue message for transmission (non-blocking ~20ns)
     * @param data Track data to send
     */
    void enqueueMessage(const domain::model::ExtrapTrackData& data);

    // Configuration
    std::string endpoint_;          ///< ZeroMQ endpoint
    std::string group_;             ///< ZeroMQ group name for RADIO

    // Socket abstraction (DIP compliant)
    std::unique_ptr<adapters::common::messaging::IMessageSocket> socket_;  ///< Socket abstraction

    // State
    std::atomic<bool> running_;     ///< Running state flag
    std::atomic<bool> ready_;       ///< Ready to send flag
    bool ownsSocket_{false};        ///< Flag to track socket ownership

    // Background worker thread infrastructure
    std::thread publisherThread_;                       ///< Background publisher thread
    std::queue<domain::model::ExtrapTrackData> messageQueue_;  ///< Message queue
    mutable std::mutex queueMutex_;                     ///< Queue protection mutex
    std::condition_variable queueCV_;                   ///< Queue notification

    // ==================== Configuration Constants ====================
    // Real-time thread configuration
    static constexpr int32_t REALTIME_THREAD_PRIORITY{80};  ///< SCHED_FIFO priority
    static constexpr int32_t DEDICATED_CPU_CORE{2};         ///< CPU affinity core
    static constexpr std::size_t MAX_QUEUE_SIZE{1000};      ///< Max queue size before drop

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
