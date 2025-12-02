/**
 * @file main.cpp
 * @brief Main entry point for A_Hexagon application
 * @details Initializes the async logger and starts the hexagonal architecture components
 *          using Thread-per-Type architecture for adapter isolation
 * 
 * Architecture Overview:
 * ┌─────────────────────────────────────────────────────────────────────┐
 * │                    THREAD-PER-TYPE ARCHITECTURE                     │
 * ├─────────────────────────────────────────────────────────────────────┤
 * │                                                                     │
 * │  ┌─────────────────────────────────────────────────────────────┐   │
 * │  │                  TrackData Pipeline                         │   │
 * │  ├─────────────────────────────────────────────────────────────┤   │
 * │  │  TrackDataZeroMQIncomingAdapter (Thread T1)                 │   │
 * │  │           ↓                                                 │   │
 * │  │  TrackDataExtrapolator (Domain Service)                     │   │
 * │  │           ↓                                                 │   │
 * │  │  ExtrapTrackDataZeroMQOutgoingAdapter                       │   │
 * │  └─────────────────────────────────────────────────────────────┘   │
 * │                                                                     │
 * │  Each pipeline operates in COMPLETE ISOLATION - zero contention!   │
 * │                                                                     │
 * │  DIP Compliant: Adapters depend on IMessageSocket abstraction      │
 * │  Production uses ZeroMQSocket, Tests use MockMessageSocket         │
 * └─────────────────────────────────────────────────────────────────────┘
 * 
 * @author a_hexagon Team
 * @version 2.1
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @note DIP compliant - socket injection for testability
 */

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>

#include "utils/Logger.hpp"

// Adapter infrastructure
#include "adapters/common/AdapterManager.hpp"
#include "adapters/common/MessagePipeline.hpp"

// Socket abstraction (DIP)
#include "adapters/common/messaging/IMessageSocket.hpp"
#include "adapters/common/messaging/ZeroMQSocket.hpp"

// Incoming adapters
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"

// Outgoing adapters
#include "adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.hpp"

// Domain services
#include "domain/logic/TrackDataExtrapolator.hpp"

// Global flag for graceful shutdown - MISRA Rule 8-5-2: Brace initialization
static std::atomic<bool> g_running{true};

/**
 * @brief Signal handler for graceful shutdown
 * @param signum Signal number received
 */
void signalHandler(int signum) {
    LOG_INFO("Received signal {}, initiating graceful shutdown...", signum);
    g_running.store(false);
}

// ==================== Socket Configuration ====================
// These can be moved to a configuration file for production

namespace config {
    // TrackData incoming socket configuration (UDP DISH for multicast)
    static constexpr const char* TRACK_DATA_INCOMING_ENDPOINT = "udp://239.1.1.1:9000";
    static constexpr const char* TRACK_DATA_INCOMING_GROUP = "TrackData";
    
    // ExtrapTrackData outgoing socket configuration (UDP RADIO for multicast)
    // NOTE: b_hexagon DISH binds to this endpoint - must match!
    static constexpr const char* EXTRAP_DATA_OUTGOING_ENDPOINT = "udp://239.1.1.2:9001";
    static constexpr const char* EXTRAP_DATA_OUTGOING_GROUP = "ExtrapTrackData";
}

/**
 * @brief Create socket for incoming TrackData (DIP compliant)
 * @return Configured socket ready for binding (UDP DISH)
 */
std::unique_ptr<adapters::common::messaging::IMessageSocket> createIncomingSocket() {
    auto socket = std::make_unique<adapters::common::messaging::ZeroMQSocket>(
        adapters::common::messaging::ZeroMQSocket::SocketType::DISH
    );
    
    // Bind to UDP multicast endpoint first
    if (!socket->connect(config::TRACK_DATA_INCOMING_ENDPOINT, 
                         adapters::common::messaging::ZeroMQSocket::ConnectionMode::Bind)) {
        LOG_ERROR("Failed to bind incoming socket to {}", config::TRACK_DATA_INCOMING_ENDPOINT);
        return nullptr;
    }
    
    // Join TrackData group after bind
    socket->joinGroup(config::TRACK_DATA_INCOMING_GROUP);
    
    LOG_INFO("Incoming DISH socket bound to {} with group {}", 
             config::TRACK_DATA_INCOMING_ENDPOINT, config::TRACK_DATA_INCOMING_GROUP);
    return socket;
}

/**
 * @brief Create socket for outgoing ExtrapTrackData (DIP compliant)
 * @return Configured socket ready for connecting (UDP RADIO)
 */
std::unique_ptr<adapters::common::messaging::IMessageSocket> createOutgoingSocket() {
    auto socket = std::make_unique<adapters::common::messaging::ZeroMQSocket>(
        adapters::common::messaging::ZeroMQSocket::SocketType::RADIO
    );
    
    // Connect to UDP multicast endpoint (b_hexagon DISH socket binds)
    if (!socket->connect(config::EXTRAP_DATA_OUTGOING_ENDPOINT,
                         adapters::common::messaging::ZeroMQSocket::ConnectionMode::Connect)) {
        LOG_ERROR("Failed to connect outgoing socket to {}", config::EXTRAP_DATA_OUTGOING_ENDPOINT);
        return nullptr;
    }
    
    LOG_INFO("Outgoing RADIO socket connected to {} with group {}", 
             config::EXTRAP_DATA_OUTGOING_ENDPOINT, config::EXTRAP_DATA_OUTGOING_GROUP);
    return socket;
}

int main() {
    try {
        // Initialize async logger (call once at startup)
        utils::Logger::init("a_hexagon");
        
        LOG_INFO("=================================================");
        LOG_INFO("  A_Hexagon Application Starting");
        LOG_INFO("  Thread-per-Type Architecture (DIP Compliant)");
        LOG_INFO("=================================================");
        
        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // Create AdapterManager for pipeline management
        adapters::AdapterManager adapter_manager;
        
        // ========================================
        // Pipeline 1: TrackData Pipeline (DIP)
        // ========================================
        LOG_INFO("Creating TrackData processing pipeline with DIP...");
        
        // Create sockets via factory functions (DIP compliant)
        auto outgoingSocket = createOutgoingSocket();
        if (!outgoingSocket) {
            LOG_ERROR("Failed to create outgoing socket");
            utils::Logger::shutdown();
            return 1;
        }
        
        auto incomingSocket = createIncomingSocket();
        if (!incomingSocket) {
            LOG_ERROR("Failed to create incoming socket");
            utils::Logger::shutdown();
            return 1;
        }
        
        // Create outgoing adapter with injected socket (DIP)
        auto outgoing_adapter = std::make_shared<adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter>(
            std::move(outgoingSocket)
        );
        
        // Create domain service with outgoing port
        auto extrapolator = std::make_shared<domain::logic::TrackDataExtrapolator>(outgoing_adapter.get());
        
        // Create incoming adapter with injected socket (DIP)
        auto incoming_adapter = std::make_shared<adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter>(
            extrapolator,
            std::move(incomingSocket)
        );
        
        // Create pipeline and register with manager
        adapters::MessagePipeline track_data_pipeline(
            "TrackData",
            incoming_adapter,
            outgoing_adapter
        );
        
        adapter_manager.registerPipeline(std::move(track_data_pipeline));
        
        // ========================================
        // Future Pipeline Examples:
        // ========================================
        // Pipeline 2: SensorData Pipeline
        // auto sensorSocket = createSensorSocket();
        // auto sensor_adapter = std::make_shared<SensorDataZeroMQIncomingAdapter>(
        //     sensor_service, std::move(sensorSocket));
        // adapter_manager.registerPipeline(MessagePipeline::create("SensorData", sensor_adapter));
        
        // Start all registered pipelines
        LOG_INFO("Starting all pipelines...");
        if (!adapter_manager.startAll()) {
            LOG_ERROR("Failed to start all pipelines");
            utils::Logger::shutdown();
            return 1;
        }
        
        LOG_INFO("All pipelines started successfully");
        LOG_INFO("Registered pipelines: {}", adapter_manager.getPipelineCount());
        LOG_INFO("Press Ctrl+C to shutdown gracefully");
        
        // Main loop - wait for shutdown signal
        while (g_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Graceful shutdown
        LOG_INFO("Stopping all pipelines...");
        adapter_manager.stopAll();
        
        LOG_INFO("=================================================");
        LOG_INFO("  A_Hexagon Application Shutdown Complete");
        LOG_INFO("=================================================");
        
        // Shutdown logger gracefully
        utils::Logger::shutdown();
        
    } catch (const std::exception& e) {
        LOG_CRITICAL("a_hexagon application main error: {}", e.what());
        utils::Logger::shutdown();
        return 1;
    }
    
    return 0;
}
