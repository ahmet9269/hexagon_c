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
 * └─────────────────────────────────────────────────────────────────────┘
 * 
 * @author a_hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
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

int main() {
    try {
        // Initialize async logger (call once at startup)
        utils::Logger::init("a_hexagon");
        
        LOG_INFO("=================================================");
        LOG_INFO("  A_Hexagon Application Starting");
        LOG_INFO("  Thread-per-Type Architecture");
        LOG_INFO("=================================================");
        
        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // Create AdapterManager for pipeline management
        adapters::AdapterManager adapter_manager;
        
        // ========================================
        // Pipeline 1: TrackData Pipeline
        // ========================================
        LOG_INFO("Creating TrackData processing pipeline...");
        
        // Create outgoing adapter (ExtrapTrackData sender)
        auto outgoing_adapter = std::make_shared<adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter>();
        
        // Create domain service with outgoing port
        auto extrapolator = std::make_shared<domain::logic::TrackDataExtrapolator>(outgoing_adapter.get());
        
        // Create incoming adapter (TrackData receiver)
        auto incoming_adapter = std::make_shared<adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter>(extrapolator.get());
        
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
        // auto sensor_service = std::make_shared<SensorDataService>();
        // auto sensor_adapter = std::make_shared<SensorDataZeroMQIncomingAdapter>(sensor_service);
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
