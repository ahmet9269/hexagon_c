/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapter.cpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission
 * @details Thread-per-Type architecture compliant with background worker thread
 *          SOLID compliant - uses direct ZeroMQ socket for simplicity
 *          Non-blocking send (~20ns enqueue) for real-time performance
 */

#include <zmq_config.hpp>
#include <zmq.hpp>
#include "DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "../../../utils/Logger.hpp"
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <chrono>

// Linux real-time scheduling headers
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

using domain::ports::DelayCalcTrackData;

// ==================== Constructors ====================

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter()
    : endpoint_(std::string(ZMQ_PROTOCOL) + "://" + ZMQ_ADDRESS + ":" + std::to_string(ZMQ_PORT))
    , group_(ZMQ_GROUP)
    , adapterName_(std::string(ZMQ_GROUP) + "-OutAdapter")
    , socket_()  // Default constructed SimpleZMQSocket
    , running_{false}
    , ready_{false} {
    
    if (!socket_.connect(endpoint_, group_)) {
        throw std::runtime_error("Failed to connect SimpleZMQSocket to: " + endpoint_);
    }
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created - endpoint: ", endpoint_, ", group: ", group_);
}

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter(
    const std::string& endpoint,
    const std::string& group)
    : endpoint_(endpoint)
    , group_(group)
    , adapterName_(group + "-OutAdapter")
    , socket_()  // Default constructed SimpleZMQSocket
    , running_{false}
    , ready_{false} {
    
    if (!socket_.connect(endpoint_, group_)) {
        throw std::runtime_error("Failed to connect SimpleZMQSocket to: " + endpoint_);
    }
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created (custom) - endpoint: ", endpoint_, ", group: ", group_);
}



DelayCalcTrackDataZeroMQOutgoingAdapter::~DelayCalcTrackDataZeroMQOutgoingAdapter() noexcept {
    stop();
    Logger::debug("DelayCalcTrackDataZeroMQOutgoingAdapter destroyed: ", adapterName_);
}



// ==================== IAdapter Interface ====================

bool DelayCalcTrackDataZeroMQOutgoingAdapter::start() {
    if (running_.load()) {
        Logger::warn("Adapter already running: ", adapterName_);
        return true;
    }
    
    if (!socket_.isConnected()) {
        Logger::error("Cannot start adapter: socket not connected");
        return false;
    }
    
    running_.store(true);
    ready_.store(true);
    
    // Start background publisher thread
    publisherThread_ = std::thread([this]() {
        process();
    });
    
    Logger::info("Adapter started with background worker: ", adapterName_);
    return true;
}

void DelayCalcTrackDataZeroMQOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    Logger::info("Stopping adapter: ", adapterName_);
    running_.store(false);
    ready_.store(false);
    
    // Wake up the worker thread
    queueCV_.notify_all();
    
    if (publisherThread_.joinable()) {
        publisherThread_.join();
    }
    
    socket_.close();
    
    Logger::info("Adapter stopped: ", adapterName_);
}

bool DelayCalcTrackDataZeroMQOutgoingAdapter::isRunning() const {
    return running_.load();
}

bool DelayCalcTrackDataZeroMQOutgoingAdapter::isReady() const noexcept {
    return ready_.load() && running_.load();
}

std::string DelayCalcTrackDataZeroMQOutgoingAdapter::getName() const noexcept {
    return adapterName_;
}

// ==================== Non-blocking Send (~20ns enqueue) ====================
// sendDelayCalcTrackData is the hot path called by domain thread
// Performance goal: Return to caller ASAP (~20ns) to maintain real-time responsiveness
// Actual ZeroMQ transmission happens in background thread

void DelayCalcTrackDataZeroMQOutgoingAdapter::sendDelayCalcTrackData(const DelayCalcTrackData& data) {
    if (!isReady()) {
        Logger::warn("Adapter not ready, dropping message for track: ", data.getTrackId());
        return;
    }
    
    // Validate input data
    if (!data.isValid()) {
        Logger::error("Invalid DelayCalcTrackData for track ID: ", data.getTrackId());
        return;
    }
    
    // Non-blocking enqueue (~20ns)
    enqueueMessage(data);
}

void DelayCalcTrackDataZeroMQOutgoingAdapter::enqueueMessage(const DelayCalcTrackData& data) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        // Prevent unbounded queue growth (protects against memory exhaustion)
        // Strategy: Drop oldest message (FIFO eviction)
        // Alternative strategies: Drop newest, drop by priority, backpressure
        if (messageQueue_.size() >= MAX_QUEUE_SIZE) {
            Logger::warn("Message queue full, dropping oldest message");
            messageQueue_.pop();  // Remove oldest
        }
        
        messageQueue_.push(data);
    }
    
    queueCV_.notify_one();
}

// ==================== Background Processing Loop ====================

void DelayCalcTrackDataZeroMQOutgoingAdapter::process() {
    Logger::debug("Outgoing adapter processing thread started: ", adapterName_);
    
    while (running_.load()) {
        DelayCalcTrackData data;
        
        // Wait for message with timeout
        // Condition variable pattern: wait_for(lock, timeout, predicate)
        // - Returns true if predicate becomes true (message available or shutdown)
        // - Returns false on timeout (allows periodic shutdown check)
        // - Lock is automatically released during wait, reacquired on wake
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            if (queueCV_.wait_for(lock, std::chrono::milliseconds(QUEUE_WAIT_TIMEOUT_MS),
                [this]() { return !messageQueue_.empty() || !running_.load(); })) {
                
                if (!running_.load()) {
                    break;
                }
                
                if (!messageQueue_.empty()) {
                    data = std::move(messageQueue_.front());
                    messageQueue_.pop();
                } else {
                    continue;
                }
            } else {
                continue;  // Timeout, check running flag
            }
        }
        
        // Serialize and send (outside lock)
        // Lock is released here to prevent blocking enqueue operations
        try {
            // Serialize domain object to binary format (96 bytes)
            // Format: Little-endian packed struct
            std::vector<uint8_t> binaryData = data.serialize();
            
            if (binaryData.empty()) {
                Logger::error("Empty payload for track ID: ", data.getTrackId());
                continue;
            }
            
            // Send via SimpleZMQSocket (RADIO pattern with group tag)
            if (socket_.send(binaryData)) {
                Logger::debug("[", adapterName_, "] Sent TrackID: ", data.getTrackId(), 
                             ", Size: ", binaryData.size(), " bytes");
            } else {
                Logger::warn("Failed to send message for track: ", data.getTrackId());
            }
            
        } catch (const std::exception& e) {
            Logger::error("[", adapterName_, "] Send error: ", e.what());
        }
    }
    
    Logger::debug("Outgoing adapter processing thread stopped: ", adapterName_);
}