/**
 * @file c_hexagon.cpp
 * @brief Main entry point for C_Hexagon application
 * @details Initializes the async logger and starts the hexagonal architecture components
 *          using Thread-per-Type architecture for adapter isolation
 */

#include <iostream>
#include <csignal>
#include <atomic>
#include "utils/Logger.hpp"
#include "adapters/common/AdapterManager.hpp"
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"
#include "adapters/outgoing/zeromq/FinalCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "domain/logic/FinalCalculationService.hpp"

// Global flag for graceful shutdown
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
        utils::Logger::init("c_hexagon");
        
        LOG_INFO("=================================================");
        LOG_INFO("  C_Hexagon Application Starting");
        LOG_INFO("  Thread-per-Type Architecture");
        LOG_INFO("=================================================");
        
        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // Create AdapterManager for pipeline management
        adapters::AdapterManager adapter_manager;
        
        // ========================================
        // Pipeline 1: DelayCalcTrackData Pipeline
        // ========================================
        LOG_INFO("Creating DelayCalcTrackData pipeline...");
        
        // Create outgoing adapter first (implements ITrackDataStatisticOutgoingPort)
        auto final_calc_outgoing = std::make_shared<adapters::outgoing::zeromq::FinalCalcTrackDataZeroMQOutgoingAdapter>(
            // TCP localhost endpoint (for development/container environment)
            "tcp://127.0.0.1:15003",
            "FinalCalcTrackData"
        );
        
        // Create domain service with outgoing port injection (Dependency Inversion)
        // FinalCalculationService needs unique_ptr, but we also need shared_ptr for adapter
        // So we create a separate unique_ptr wrapping the same interface
        auto delay_calc_service = std::make_shared<domain::logic::FinalCalculationService>(
            std::make_unique<adapters::outgoing::zeromq::FinalCalcTrackDataZeroMQOutgoingAdapter>(
                "tcp://127.0.0.1:15003",
                "FinalCalcTrackData"
            )
        );
        
        // Create incoming adapter
        auto delay_calc_adapter = std::make_shared<adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter>(
            delay_calc_service,
            // Original UDP multicast endpoint (for production environment)
            // "udp://udn;239.1.1.1:9002",
            // TCP localhost endpoint (for development/container environment)
            "tcp://127.0.0.1:15002",
            "DelayCalcTrackData"
        );
        
        // Create pipeline and register with manager
        adapters::MessagePipeline delay_calc_pipeline(
            "DelayCalcTrackData",
            delay_calc_adapter,
            final_calc_outgoing  // Now includes outgoing adapter
        );
        
        adapter_manager.registerPipeline(std::move(delay_calc_pipeline));
        
        // ========================================
        // Future Pipeline Examples:
        // ========================================
        // Pipeline 2: RadarData Pipeline
        // auto radar_service = std::make_shared<RadarDataService>();
        // auto radar_adapter = std::make_shared<RadarDataZeroMQIncomingAdapter>(radar_service);
        // adapter_manager.registerPipeline(MessagePipeline::create("RadarData", radar_adapter));
        //
        // Pipeline 3: SensorFusion Pipeline
        // auto fusion_service = std::make_shared<SensorFusionService>();
        // auto fusion_adapter = std::make_shared<FusionDataZeroMQIncomingAdapter>(fusion_service);
        // adapter_manager.registerPipeline(MessagePipeline::create("SensorFusion", fusion_adapter));
        
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
        LOG_INFO("  C_Hexagon Application Shutdown Complete");
        LOG_INFO("=================================================");
        
        // Shutdown logger gracefully
        utils::Logger::shutdown();
        
    } catch (const std::exception& e) {
        LOG_CRITICAL("c_hexagon application main error: {}", e.what());
        utils::Logger::shutdown();
        return 1;
    }
    
    return 0;
}
