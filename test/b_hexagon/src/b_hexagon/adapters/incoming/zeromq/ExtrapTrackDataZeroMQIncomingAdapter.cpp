/**
 * @file ExtrapTrackDataZeroMQIncomingAdapter.cpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 * @details Thread-per-Type architecture - runs in dedicated worker thread
 *          SOLID compliant - uses IMessageSocket abstraction for testability
 */

#include "ExtrapTrackDataZeroMQIncomingAdapter.hpp"
#include "adapters/common/messaging/ZeroMQSocket.hpp"
#include "../../../utils/Logger.hpp"
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <chrono>

// Linux real-time scheduling headers
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

using domain::ports::ExtrapTrackData;
using adapters::common::messaging::IMessageSocket;
using adapters::common::messaging::ZeroMQSocket;

// ==================== Constructors ====================

ExtrapTrackDataZeroMQIncomingAdapter::ExtrapTrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver)
    : endpoint_(std::string(DEFAULT_PROTOCOL) + "://" + DEFAULT_MULTICAST_ADDRESS + ":" + std::to_string(DEFAULT_PORT))
    , group_(DEFAULT_GROUP)
    , adapterName_(std::string(DEFAULT_GROUP) + "-InAdapter")
    , socket_(nullptr)
    , dataReceiver_(std::move(dataReceiver))
    , running_{false} {
    
    socket_ = createDefaultSocket();
    Logger::info("ExtrapTrackDataZeroMQIncomingAdapter created - endpoint: ", endpoint_, ", group: ", group_);
}

ExtrapTrackDataZeroMQIncomingAdapter::ExtrapTrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver,
    const std::string& endpoint,
    const std::string& group)
    : endpoint_(endpoint)
    , group_(group)
    , adapterName_(group + "-InAdapter")
    , socket_(nullptr)
    , dataReceiver_(std::move(dataReceiver))
    , running_{false} {
    
    socket_ = createDefaultSocket();
    Logger::info("ExtrapTrackDataZeroMQIncomingAdapter created (custom) - endpoint: ", endpoint_, ", group: ", group_);
}

ExtrapTrackDataZeroMQIncomingAdapter::ExtrapTrackDataZeroMQIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::IExtrapTrackDataIncomingPort> dataReceiver,
    std::unique_ptr<IMessageSocket> socket,
    const std::string& group)
    : endpoint_("injected")
    , group_(group)
    , adapterName_(group + "-InAdapter")
    , socket_(std::move(socket))
    , dataReceiver_(std::move(dataReceiver))
    , running_{false} {
    
    Logger::info("ExtrapTrackDataZeroMQIncomingAdapter created (injected socket) - group: ", group_);
}

ExtrapTrackDataZeroMQIncomingAdapter::~ExtrapTrackDataZeroMQIncomingAdapter() noexcept {
    stop();
    Logger::debug("ExtrapTrackDataZeroMQIncomingAdapter destroyed: ", adapterName_);
}

// ==================== Socket Factory ====================

std::unique_ptr<IMessageSocket> ExtrapTrackDataZeroMQIncomingAdapter::createDefaultSocket() {
    auto socket = std::make_unique<ZeroMQSocket>(ZeroMQSocket::SocketType::DISH);
    
    if (!socket->connect(endpoint_)) {
        throw std::runtime_error("Failed to bind DISH socket to: " + endpoint_);
    }
    
    if (!socket->subscribe(group_)) {
        throw std::runtime_error("Failed to subscribe to group: " + group_);
    }
    
    Logger::debug("DISH socket initialized via IMessageSocket - endpoint: ", endpoint_, ", group: ", group_);
    return socket;
}

// ==================== IAdapter Interface ====================

bool ExtrapTrackDataZeroMQIncomingAdapter::start() {
    if (running_.load()) {
        Logger::warn("Adapter already running: ", adapterName_);
        return true;
    }
    
    if (!dataReceiver_) {
        Logger::error("Cannot start adapter: dataReceiver is null");
        return false;
    }
    
    if (!socket_ || !socket_->isConnected()) {
        Logger::error("Cannot start adapter: socket not connected");
        return false;
    }
    
    running_.store(true);
    
    // Start dedicated worker thread
    workerThread_ = std::thread([this]() {
        // Configure real-time thread priority on Linux
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = REALTIME_THREAD_PRIORITY;
        int result = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        if (result == 0) {
            Logger::debug("Real-time priority set for: ", adapterName_);
        }
        
        // Set CPU affinity
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        workerThread();
    });
    
    Logger::info("Adapter started: ", adapterName_);
    return true;
}

void ExtrapTrackDataZeroMQIncomingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    Logger::info("Stopping adapter: ", adapterName_);
    running_.store(false);
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
    
    if (socket_) {
        socket_->close();
    }
    
    Logger::info("Adapter stopped: ", adapterName_);
}

bool ExtrapTrackDataZeroMQIncomingAdapter::isRunning() const {
    return running_.load();
}

std::string ExtrapTrackDataZeroMQIncomingAdapter::getName() const noexcept {
    return adapterName_;
}

// ==================== Worker Thread ====================

void ExtrapTrackDataZeroMQIncomingAdapter::workerThread() {
    Logger::debug("Worker thread started: ", adapterName_);
    
    while (running_.load()) {
        try {
            // Receive via IMessageSocket abstraction
            auto result = socket_->receive(RECEIVE_TIMEOUT_MS);
            
            if (!result || result->empty()) {
                continue; // Timeout - check running flag and retry
            }
            
            const std::vector<uint8_t>& binaryData = *result;
            ExtrapTrackData data = deserializeBinary(binaryData.data(), binaryData.size());
            
            Logger::info("[", adapterName_, "] Received TrackID: ", data.getTrackId(), 
                        ", Size: ", binaryData.size(), " bytes");
            
            // Forward to domain service
            if (dataReceiver_) {
                dataReceiver_->onDataReceived(data);
            }
            
        } catch (const std::exception& ex) {
            if (running_.load()) {
                Logger::error("[", adapterName_, "] Processing error: ", ex.what());
            }
        }
    }
    
    Logger::debug("Worker thread stopped: ", adapterName_);
}

// ==================== Deserialization ====================

ExtrapTrackData ExtrapTrackDataZeroMQIncomingAdapter::deserializeBinary(const uint8_t* data, std::size_t size) {
    ExtrapTrackData extrapData;
    std::vector<uint8_t> binary_data(data, data + size);
    
    if (extrapData.deserialize(binary_data)) {
        return extrapData;
    } else {
        throw std::runtime_error("Binary deserialization failed");
    }
}
