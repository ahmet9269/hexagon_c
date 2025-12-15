/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 * @details Thread-per-Type architecture compliant - implements IAdapter interface
 *          SOLID compliant - uses direct ZeroMQ socket for simplicity
 *          Uses background worker thread for non-blocking sends (~20ns enqueue latency)
 * 
 * Dependency Inversion:
 * - IDelayCalcTrackDataOutgoingPort: Domain port abstraction
 * 
 * Thread Architecture:
 * ┌─────────────────────────────────────────────────────────────────┐
 * │  Domain Thread          Background Worker Thread               │
 * │  ─────────────          ────────────────────────                │
 * │  sendDelayCalcTrackData() ──┐                                   │
 * │       │                     │                                   │
 * │       ▼                     ▼                                   │
 * │  [Enqueue ~20ns]     [Dequeue + ZMQ Send]                      │
 * │       │                     │                                   │
 * │  Return immediately    Background transmission                  │
 * └─────────────────────────────────────────────────────────────────┘
 */

#pragma once

#include "adapters/common/IAdapter.hpp"                              // IAdapter interface
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp" // Outbound port interface
#include "domain/ports/outgoing/DelayCalcTrackData.hpp"              // Domain data model
#include <zmq_config.hpp>
#include <zmq.hpp>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;

/**
 * @class DelayCalcTrackDataZeroMQOutgoingAdapter
 * @brief ZeroMQ RADIO adapter for broadcasting DelayCalcTrackData
 * 
 * SOLID Compliance:
 * - Single Responsibility: Only handles outgoing message transmission
 * - Open/Closed: Extends IAdapter and port, closed for modification
 * - Liskov Substitution: Can replace any IAdapter or IDelayCalcTrackDataOutgoingPort
 * - Interface Segregation: Implements focused interfaces
 * - Dependency Inversion: Depends on domain port abstractions
 * 
 * Real-time Features:
 * - Non-blocking sendDelayCalcTrackData() (~20ns enqueue)
 * - Background worker thread for actual ZMQ transmission
 * - Bounded queue with overflow protection
 * - SCHED_FIFO priority for worker thread
 * 
 * Test Coverage:
 * - Direct ZeroMQ socket usage for production simplicity
 * - Verify serialization and transmission
 */
class DelayCalcTrackDataZeroMQOutgoingAdapter final 
    : public adapters::IAdapter
    , public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
public:
    // Simple inline ZeroMQ socket wrapper for RADIO pattern
    // Provides minimal interface for RADIO socket operations
    // RADIO Pattern: Distributes messages to DISH subscribers via group addressing
    class SimpleZMQSocket {
    public:
        SimpleZMQSocket() : socket_(nullptr) {}
        
        bool connect(const std::string& endpoint, const std::string& group) {
            try {
                context_ = std::make_shared<zmq::context_t>(1);
                socket_ = std::make_unique<zmq::socket_t>(*context_, zmq::socket_type::radio);
                
                // RADIO sockets connect (not bind) to endpoint
                // DISH sockets bind to same endpoint to receive messages
                socket_->connect(endpoint);
                
                // Set send timeout to prevent blocking on slow subscribers
                socket_->set(zmq::sockopt::sndtimeo, 100);
                
                group_ = group;  // Store group name for message tagging
                return true;
            } catch (const zmq::error_t&) {
                return false;
            }
        }
        
        bool send(const std::vector<uint8_t>& data) {
            if (!socket_) return false;
            try {
                zmq::message_t msg(data.data(), data.size());
                
                // Set message group for DISH filtering
                // Only DISH subscribers joined to this group will receive the message
                msg.set_group(group_.c_str());
                
                return socket_->send(msg, zmq::send_flags::none).has_value();
            } catch (const zmq::error_t&) {
                return false;
            }
        }
        
        void close() {
            if (socket_) {
                socket_->close();
                socket_.reset();
            }
        }
        
        bool isConnected() const { return socket_ != nullptr; }
        
    private:
        std::shared_ptr<zmq::context_t> context_;
        std::unique_ptr<zmq::socket_t> socket_;
        std::string group_;
    };

    /**
     * @brief Default constructor (production use)
     */
    explicit DelayCalcTrackDataZeroMQOutgoingAdapter();
    
    /**
     * @brief Construct with custom endpoint and group (production use)
     */
    DelayCalcTrackDataZeroMQOutgoingAdapter(
        const std::string& endpoint,
        const std::string& group);

    // Destructor - RAII cleanup
    ~DelayCalcTrackDataZeroMQOutgoingAdapter() noexcept override;

    // Disable copy operations
    DelayCalcTrackDataZeroMQOutgoingAdapter(const DelayCalcTrackDataZeroMQOutgoingAdapter& other) = delete;
    DelayCalcTrackDataZeroMQOutgoingAdapter& operator=(const DelayCalcTrackDataZeroMQOutgoingAdapter& other) = delete;

    // Disable move operations for thread safety
    DelayCalcTrackDataZeroMQOutgoingAdapter(DelayCalcTrackDataZeroMQOutgoingAdapter&& other) = delete;
    DelayCalcTrackDataZeroMQOutgoingAdapter& operator=(DelayCalcTrackDataZeroMQOutgoingAdapter&& other) = delete;

    // ==================== IAdapter Interface ====================
    
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const noexcept override;

    // ==================== IDelayCalcTrackDataOutgoingPort Interface ====================
    
    /**
     * @brief Send data via RADIO socket (non-blocking)
     * @param data Data to send
     * @details Enqueues message for background transmission (~20ns latency)
     *          Domain thread returns immediately without blocking on ZMQ I/O
     * @thread_safe Yes - uses mutex-protected queue
     */
    void sendDelayCalcTrackData(const DelayCalcTrackData& data) override;

    /**
     * @brief Check if adapter is ready to accept messages
     * @return true if running and socket connected
     */
    [[nodiscard]] bool isReady() const noexcept;

private:
    // Network configuration constants
    static constexpr const char* ZMQ_ADDRESS = "127.0.0.1";
    static constexpr int ZMQ_PORT = 15002;
    static constexpr const char* ZMQ_PROTOCOL = "udp";
    static constexpr const char* ZMQ_GROUP = "DelayCalcTrackData";
    
    // Queue configuration
    // MAX_QUEUE_SIZE controls memory vs latency trade-off:
    // - Larger queue: More buffering, handles bursts, higher memory usage
    // - Smaller queue: Lower memory, drops messages faster during overload
    // 200 messages ≈ 15KB (200 * 76 bytes per message)
    static constexpr std::size_t MAX_QUEUE_SIZE = 200;   ///< Prevent unbounded growth
    static constexpr int QUEUE_WAIT_TIMEOUT_MS = 100;    ///< Graceful shutdown timeout (allows loop to check running flag)

    /**
     * @brief Background processing loop for message transmission
     * @details Runs in dedicated thread, continuously dequeues messages
     *          and sends them via ZeroMQ RADIO socket
     */
    void process();

    /**
     * @brief Enqueue message for background transmission
     * @param data Track data to queue
     */
    void enqueueMessage(const DelayCalcTrackData& data);

    // Configuration
    std::string endpoint_;             ///< ZeroMQ endpoint
    std::string group_;                ///< Group identifier for DISH filtering
    std::string adapterName_;          ///< Adapter name for logging
    
    // Simple ZeroMQ socket (inline implementation)
    SimpleZMQSocket socket_;
    
    // Thread management
    std::thread publisherThread_;      ///< Background worker thread
    std::atomic<bool> running_{false}; ///< Running state
    std::atomic<bool> ready_{false};   ///< Socket ready state

    // Thread-safe message queue
    mutable std::mutex queueMutex_;              ///< Queue protection
    std::condition_variable queueCV_;   ///< Queue notification
    std::queue<DelayCalcTrackData> messageQueue_; ///< Pending messages
};