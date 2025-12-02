/**
 * @file main.cpp
 * @brief Application entry point demonstrating Thread-per-Type Architecture
 * @details Scalable architecture supporting 20+ concurrent adapters with isolated threads
 *          SOLID compliant - uses abstractions (ICalculatorService, IMessageSocket)
 * @version 2.1 - SOLID compliant with Dependency Inversion
 */

#include "domain/logic/ICalculatorService.hpp"
#include "domain/logic/CalculatorService.hpp"
#include "domain/logic/ProcessTrackUseCase.hpp"
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "adapters/incoming/zeromq/ExtrapTrackDataZeroMQIncomingAdapter.hpp"
#include "adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "adapters/common/IAdapter.hpp"
#include "adapters/common/MessagePipeline.hpp"
#include "adapters/common/AdapterManager.hpp"
#include "utils/Logger.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

// Using declarations for convenience
using domain::ports::ExtrapTrackData;
using domain::ports::DelayCalcTrackData;
using domain::logic::ICalculatorService;
using domain::logic::CalculatorService;
using domain::logic::ProcessTrackUseCase;
using adapters::AdapterManager;
using adapters::MessagePipeline;

// Global pointer for signal handler access
static std::atomic<bool> g_running{true};
static AdapterManager* g_adapterManager{nullptr};

/**
 * @brief Signal handler for graceful shutdown
 * @param signum Signal number received
 */
void signalHandler(int signum) {
    Logger::info("Received signal ", signum, ", initiating graceful shutdown...");
    g_running.store(false);
    if (g_adapterManager != nullptr) {
        g_adapterManager->stopAll();
    }
}

/**
 * @brief Application entry point
 * 
 * Demonstrates Thread-per-Type Architecture where:
 * - Each message type has its own isolated pipeline
 * - Each adapter runs in a dedicated thread
 * - Zero contention between different message types
 * - Supports scaling to 20+ concurrent adapters
 * 
 * SOLID Compliance:
 * - ICalculatorService: Abstract calculator (mockable for tests)
 * - IMessageSocket: Abstract socket (mockable for tests)
 * - IDelayCalcTrackDataOutgoingPort: Abstract outgoing port
 */
int main() {
    Logger::info("=== B_Hexagon Track Processing System Starting ===");
    Logger::info("Architecture: Thread-per-Type (Scalable to 20+ adapters)");
    Logger::info("SOLID: Dependency Inversion enabled for high test coverage");
    
    try {
        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        Logger::info("Initializing application components...");
        
        // ==================== Create AdapterManager ====================
        AdapterManager adapterManager;
        g_adapterManager = &adapterManager;
        
        // ==================== Create Domain Services (via abstractions) ====================
        Logger::debug("Creating CalculatorService (implements ICalculatorService)...");
        std::unique_ptr<ICalculatorService> calculatorService = std::make_unique<CalculatorService>();
        
        // ==================== Create ExtrapTrackData Pipeline ====================
        Logger::info("Creating ExtrapTrackData Pipeline...");
        
        // Create outgoing adapter (RADIO socket for UDP multicast)
        Logger::debug("Creating DelayCalcTrackDataZeroMQOutgoingAdapter (RADIO socket)...");
        auto outgoingAdapter = std::make_shared<DelayCalcTrackDataZeroMQOutgoingAdapter>();
        
        // Create use case with dependencies
        Logger::debug("Creating ProcessTrackUseCase with dependencies...");
        auto useCase = std::make_shared<ProcessTrackUseCase>(
            std::move(calculatorService),
            outgoingAdapter  // Pass shared_ptr directly
        );
        
        // Create incoming adapter (DISH socket) with shared_ptr to use case
        Logger::debug("Creating ExtrapTrackDataZeroMQIncomingAdapter (DISH socket)...");
        auto incomingAdapter = std::make_shared<ExtrapTrackDataZeroMQIncomingAdapter>(useCase);
        
        // Create and register ExtrapTrackData pipeline
        MessagePipeline extrapPipeline(
            "ExtrapTrackData",
            incomingAdapter,
            outgoingAdapter
        );
        adapterManager.registerPipeline(std::move(extrapPipeline));
        
        // ==================== System Information ====================
        Logger::info("=== System Configuration ===");
        Logger::info("Architecture: Thread-per-Type (Hexagonal)");
        Logger::info("Messaging: ZeroMQ RADIO/DISH UDP multicast");
        Logger::info("Input Group: ExtrapTrackData");
        Logger::info("Output Group: DelayCalcTrackData");
        Logger::info("Thread Safety: Enabled with per-adapter isolation");
        Logger::info("Registered Pipelines: ", adapterManager.getPipelineCount());
        Logger::info("Status: Ready to receive track data");
        Logger::info("Press Ctrl+C to shutdown gracefully");
        Logger::info("===============================");
        
        // ==================== Start All Adapters ====================
        Logger::info("Starting all adapters in dedicated threads...");
        if (!adapterManager.startAll()) {
            Logger::error("Failed to start all adapters");
            return 1;
        }
        
        // ==================== Main Loop ====================
        Logger::info("All adapters running. Entering main loop...");
        while (g_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // ==================== Graceful Shutdown ====================
        Logger::info("Main loop exited, stopping all adapters...");
        adapterManager.stopAll();
        
        // Clear global pointer
        g_adapterManager = nullptr;
        
    } catch (const std::exception& ex) {
        Logger::error("Application error: ", ex.what());
        return 1;
    }
    
    Logger::info("=== B_Hexagon Application Shutdown Complete ===");
    return 0;
}