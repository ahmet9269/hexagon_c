/**
 * @file main.cpp
 * @brief Application entry point for C_Hexagon with Thread-per-Type Architecture
 * @details Each component runs in dedicated thread with background worker threads.
 *          SOLID compliant - uses domain port abstractions for dependency inversion.
 * @version 2.0 - Thread-per-Type Pattern with isolated threads
 */

#include "domain/logic/TargetStatisticService.hpp"
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"
#include "adapters/outgoing/zeromq/FinalCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "utils/Logger.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;
using domain::ports::FinalCalcTrackData;
using utils::Logger;

// Global resources for signal handler access
// NOTE: Signal handlers cannot access member variables, so we use static globals
// All pointers are set to nullptr after use to prevent dangling references
static std::atomic<bool> g_running{true};  // Thread-safe shutdown flag
static domain::logic::TargetStatisticService* g_domainService{nullptr};
static adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter* g_incomingAdapter{nullptr};
static adapters::outgoing::zeromq::FinalCalcTrackDataZeroMQOutgoingAdapter* g_outgoingAdapter{nullptr};

/**
 * @brief Signal handler for graceful shutdown
 * @param signum Signal number received
 */
void signalHandler(int signum) {
    Logger::info("Received signal ", signum, ", initiating graceful shutdown...");
    g_running.store(false);
    
    // Stop all components in reverse order (incoming first to stop new data flow)
    if (g_incomingAdapter != nullptr) {
        g_incomingAdapter->stop();
    }
    if (g_domainService != nullptr) {
        g_domainService->stop();
    }
    if (g_outgoingAdapter != nullptr) {
        g_outgoingAdapter->stop();
    }
}

/**
 * @brief Application entry point
 * 
 * Thread-per-Type Architecture:
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │                    3 ISOLATED THREADS + MAIN THREAD                     │
 * ├─────────────────────────────────────────────────────────────────────────┤
 * │                                                                         │
 * │  Thread 1: Incoming Adapter (CPU 2)                                    │
 * │    └─ ZeroMQ DISH → Deserialize → submitDelayCalcTrackData()           │
 * │                                                                         │
 * │  Thread 2: Domain Service (CPU 3)                                      │
 * │    └─ Background Queue → TargetStatisticService → sendFinalTrackData() │
 * │                                                                         │
 * │  Thread 3: Outgoing Adapter (CPU 4)                                    │
 * │    └─ Background Queue → Serialize → ZeroMQ RADIO                      │
 * │                                                                         │
 * │  Thread 4: Main Thread                                                 │
 * │    └─ Lifecycle management + Signal handling                           │
 * │                                                                         │
 * │  All threads communicate via background queues (non-blocking)          │
 * └─────────────────────────────────────────────────────────────────────────┘
 * 
 * SOLID Compliance:
 * - TargetStatisticService: Domain business logic
 * - Direct ZeroMQ usage for messaging simplicity
 * - ITrackDataStatisticOutgoingPort: Abstract outgoing port
 * - IDelayCalcTrackDataIncomingPort: Abstract incoming port
 */
int main() {
    Logger::info("=== C_Hexagon Track Processing System Starting ===");
    Logger::info("Architecture: Thread-per-Type (3 isolated threads)");
    Logger::info("SOLID: Dependency Inversion enabled via domain ports");
    
    try {
        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        Logger::info("Initializing application components...");
        
        // ==================== Create Outgoing Adapter ====================
        Logger::info("Creating Outgoing Adapter...");
        Logger::debug("Creating FinalCalcTrackDataZeroMQOutgoingAdapter (RADIO socket)...");
        auto outgoingAdapter = std::make_shared<adapters::outgoing::zeromq::FinalCalcTrackDataZeroMQOutgoingAdapter>();
        g_outgoingAdapter = outgoingAdapter.get();
        
        // ==================== Create Domain Service ====================
        Logger::debug("Creating TargetStatisticService with outgoing port...");
        auto domainService = std::make_shared<domain::logic::TargetStatisticService>(outgoingAdapter);
        g_domainService = domainService.get();
        
        // ==================== Create Incoming Adapter ====================
        Logger::debug("Creating TrackDataZeroMQIncomingAdapter (DISH socket)...");
        auto incomingAdapter = std::make_shared<adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter>(domainService);
        g_incomingAdapter = incomingAdapter.get();
        
        // ==================== System Information ====================
        Logger::info("=== System Configuration ===");
        Logger::info("Architecture: Thread-per-Type (3 threads + main)");
        Logger::info("Messaging Input: ZeroMQ DISH (UDP multicast)");
        Logger::info("Messaging Output: ZeroMQ RADIO (UDP multicast)");
        Logger::info("Input Group: DelayCalcTrackData");
        Logger::info("Output Group: FinalCalcTrackData");
        Logger::info("Thread 1: Incoming Adapter (CPU 2, Priority 95)");
        Logger::info("Thread 2: Domain Service (CPU 3, Priority 90)");
        Logger::info("Thread 3: Outgoing Adapter (CPU 4, Priority 95)");
        Logger::info("Thread 4: Main (lifecycle management)");
        Logger::info("Press Ctrl+C to shutdown gracefully");
        Logger::info("===============================");
        
        // ==================== Start All Components ====================
        // Component startup order is critical for Thread-per-Type architecture:
        // 1. Outgoing adapter starts first - ready to receive processed data
        // 2. Domain service starts next - ready to process incoming data
        // 3. Incoming adapter starts last - begins data flow into system
        // This prevents message loss by ensuring downstream is ready before upstream
        Logger::info("Starting all components in correct order...");
        
        // Step 1: Start outgoing adapter first (must be ready to receive)
        Logger::info("Step 1: Starting outgoing adapter...");
        if (!outgoingAdapter->start()) {
            Logger::error("Failed to start outgoing adapter");
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Allow initialization
        
        // Step 2: Start domain service
        Logger::info("Step 2: Starting domain service...");
        if (!domainService->start()) {
            Logger::error("Failed to start domain service");
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Step 3: Start incoming adapter last (begins data flow)
        Logger::info("Step 3: Starting incoming adapter...");
        if (!incomingAdapter->start()) {
            Logger::error("Failed to start incoming adapter");
            return 1;
        }
        
        // ==================== Main Loop ====================
        Logger::info("All components running. Entering main loop...");
        while (g_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // ==================== Graceful Shutdown ====================
        Logger::info("Main loop exited, stopping all components...");
        
        // Stop in reverse order of startup to ensure graceful shutdown:
        // 1. Incoming adapter stops first - prevents new data from entering system
        // 2. Domain service stops next - completes processing of queued messages
        // 3. Outgoing adapter stops last - ensures all processed data is sent
        // This ordering prevents data loss during shutdown
        incomingAdapter->stop();
        domainService->stop();
        outgoingAdapter->stop();
        
        // Clear global pointers
        g_incomingAdapter = nullptr;
        g_domainService = nullptr;
        g_outgoingAdapter = nullptr;
        
    } catch (const std::exception& ex) {
        Logger::error("Application error: ", ex.what());
        return 1;
    }
    
    Logger::info("=== C_Hexagon Application Shutdown Complete ===");
    return 0;
}
