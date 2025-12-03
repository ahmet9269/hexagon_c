/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapter.cpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission
 * @details Thread-per-Type architecture compliant with background worker thread
 *          SOLID compliant - uses IMessageSocket abstraction for testability
 *          Non-blocking send (~20ns enqueue) for real-time performance
 */

#include "DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "adapters/common/messaging/ZeroMQSocket.hpp"
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
using adapters::common::messaging::IMessageSocket;
using adapters::common::messaging::ZeroMQSocket;

// ==================== Constructors ====================

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter()
    : endpoint_(std::string(DEFAULT_PROTOCOL) + "://" + DEFAULT_ADDRESS + ":" + std::to_string(DEFAULT_PORT))
    , group_(DEFAULT_GROUP)
    , adapterName_(std::string(DEFAULT_GROUP) + "-OutAdapter")
    , socket_(nullptr)
    , running_{false}
    , ready_{false} {
    
    socket_ = createDefaultSocket();
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created - endpoint: ", endpoint_, ", group: ", group_);
}

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter(
    const std::string& endpoint,
    const std::string& group)
    : endpoint_(endpoint)
    , group_(group)
    , adapterName_(group + "-OutAdapter")
    , socket_(nullptr)
    , running_{false}
    , ready_{false} {
    
    socket_ = createDefaultSocket();
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created (custom) - endpoint: ", endpoint_, ", group: ", group_);
}

DelayCalcTrackDataZeroMQOutgoingAdapter::DelayCalcTrackDataZeroMQOutgoingAdapter(
    std::unique_ptr<IMessageSocket> socket,
    const std::string& group)
    : endpoint_("injected")
    , group_(group)
    , adapterName_(group + "-OutAdapter")
    , socket_(std::move(socket))
    , running_{false}
    , ready_{false} {
    
    Logger::info("DelayCalcTrackDataZeroMQOutgoingAdapter created (injected socket) - group: ", group_);
}

DelayCalcTrackDataZeroMQOutgoingAdapter::~DelayCalcTrackDataZeroMQOutgoingAdapter() noexcept {
    stop();
    Logger::debug("DelayCalcTrackDataZeroMQOutgoingAdapter destroyed: ", adapterName_);
}

// ==================== Socket Factory ====================

std::unique_ptr<IMessageSocket> DelayCalcTrackDataZeroMQOutgoingAdapter::createDefaultSocket() {
    auto socket = std::make_unique<ZeroMQSocket>(ZeroMQSocket::SocketType::RADIO);
    
    if (!socket->connect(endpoint_)) {
        throw std::runtime_error("Failed to connect RADIO socket to: " + endpoint_);
    }
    
    Logger::debug("RADIO socket initialized via IMessageSocket - endpoint: ", endpoint_, ", group: ", group_);
    return socket;
}

// ==================== IAdapter Interface ====================

bool DelayCalcTrackDataZeroMQOutgoingAdapter::start() {
    if (running_.load()) {
        Logger::warn("Adapter already running: ", adapterName_);
        return true;
    }
    
    if (!socket_ || !socket_->isConnected()) {
        Logger::error("Cannot start adapter: socket not connected");
        return false;
    }
    
    running_.store(true);
    ready_.store(true);
    
    // Start background publisher thread
    publisherThread_ = std::thread([this]() {
        // Configure real-time thread priority on Linux
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = REALTIME_THREAD_PRIORITY;
        int result = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        if (result == 0) {
            Logger::debug("Real-time priority set for outgoing: ", adapterName_);
        }
        
        // Set CPU affinity to dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        publisherWorker();
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
    
    if (socket_) {
        socket_->close();
    }
    
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
        
        // Prevent unbounded queue growth
        if (messageQueue_.size() >= MAX_QUEUE_SIZE) {
            Logger::warn("Message queue full, dropping oldest message");
            messageQueue_.pop();
        }
        
        messageQueue_.push(data);
    }
    
    queueCV_.notify_one();
}

// ==================== Background Worker Thread ====================

void DelayCalcTrackDataZeroMQOutgoingAdapter::publisherWorker() {
    Logger::debug("Publisher worker started: ", adapterName_);
    
    while (running_.load()) {
        DelayCalcTrackData data;
        
        // Wait for message with timeout
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            if (queueCV_.wait_for(lock, std::chrono::milliseconds(100),
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
        try {
            std::vector<uint8_t> binaryData = data.serialize();
            
            if (binaryData.empty()) {
                Logger::error("Empty payload for track ID: ", data.getTrackId());
                continue;
            }
            
            // Send via IMessageSocket abstraction
            if (socket_->send(binaryData, group_)) {
                Logger::debug("[", adapterName_, "] Sent TrackID: ", data.getTrackId(), 
                             ", Size: ", binaryData.size(), " bytes");
            } else {
                Logger::warn("Failed to send message for track: ", data.getTrackId());
            }
            
        } catch (const std::exception& e) {
            Logger::error("[", adapterName_, "] Send error: ", e.what());
        }
    }
    
    Logger::debug("Publisher worker stopped: ", adapterName_);
}