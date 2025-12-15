/**
 * @file main.cpp
 * @brief Application entry point with Event Queue based Thread Architecture
 * @details Each component runs in dedicated thread with event queues for communication
 *          SOLID compliant - uses abstractions (ICalculatorService, domain ports)
 * @version 3.0 - Event Queue Pattern with 5 isolated threads
 */

#include "domain/logic/ICalculatorService.hpp"
#include "domain/logic/CalculatorService.hpp"
#include "domain/logic/ProcessTrackUseCase.hpp"
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "adapters/incoming/zeromq/ExtrapTrackDataZeroMQIncomingAdapter.hpp"
#include "adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "adapters/outgoing/custom/DelayCalcTrackDataCustomOutgoingAdapter.hpp"
#include "utils/Logger.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <vector>

// Using declarations for convenience
using domain::ports::ExtrapTrackData;
using domain::ports::DelayCalcTrackData;
using domain::logic::ICalculatorService;
using domain::logic::CalculatorService;
using domain::logic::ProcessTrackUseCase;

// Global resources for signal handler access
// NOTE: Signal handlers cannot access member variables, so we use static globals
// All pointers are set to nullptr after use to prevent dangling references
static std::atomic<bool> g_running{true};  // Thread-safe shutdown flag
static ProcessTrackUseCase* g_domainProcessor{nullptr};
static ExtrapTrackDataZeroMQIncomingAdapter* g_incomingAdapter{nullptr};
static DelayCalcTrackDataZeroMQOutgoingAdapter* g_outgoingZeroMQAdapter{nullptr};
static DelayCalcTrackDataCustomOutgoingAdapter* g_outgoingCustomAdapter{nullptr};

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
    if (g_domainProcessor != nullptr) {
        g_domainProcessor->stop();
    }
    if (g_outgoingZeroMQAdapter != nullptr) {
        g_outgoingZeroMQAdapter->stop();
    }
    if (g_outgoingCustomAdapter != nullptr) {
        g_outgoingCustomAdapter->stop();
    }
}

/**
 * @brief Application entry point
 * 
 * Event Queue Based Thread Architecture:
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │                    5 ISOLATED THREADS WITH EVENT QUEUES                 │
 * ├─────────────────────────────────────────────────────────────────────────┤
 * │                                                                         │
 * │  Thread 1: Incoming Adapter (CPU 1)                                    │
 * │    └─ ZeroMQ DISH → Deserialize → submitExtrapTrackData()              │
 * │                                                                         │
 * │  Thread 2: Domain Processing (CPU 3)                                   │
 * │    └─ Event Queue → CalculatorService → sendDelayCalcTrackData()       │
 * │                                                                         │
 * │  Thread 3: Outgoing ZeroMQ (CPU 2)                                     │
 * │    └─ Event Queue → Serialize → ZeroMQ RADIO                           │
 * │                                                                         │
 *  Thread 4: Outgoing Custom Adapter (CPU 4)                              │
 *    └─ Event Queue → Moving Average Calculation                          │
 * │                                                                         │
 * │  Thread 5: Main Thread                                                 │
 * │    └─ Lifecycle management + Signal handling                           │
 * │                                                                         │
 * │  All threads communicate via non-blocking event queues (~20ns enqueue) │
 * └─────────────────────────────────────────────────────────────────────────┘
 * 
 * SOLID Compliance:
 * - ICalculatorService: Abstract calculator (mockable for tests)
 * - Direct ZeroMQ usage for messaging simplicity
 * - IDelayCalcTrackDataOutgoingPort: Abstract outgoing port
 */
int main() {
    Logger::info("=== B_Hexagon Track Processing System Starting ===");
    Logger::info("Architecture: Event Queue Based (5 isolated threads)");
    Logger::info("SOLID: Dependency Inversion enabled for high test coverage");
    
    try {
        // Register signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        Logger::info("Initializing application components...");
        
        // ==================== Create Domain Services ====================
        Logger::debug("Creating CalculatorService (implements ICalculatorService)...");
        std::unique_ptr<ICalculatorService> calculatorService = std::make_unique<CalculatorService>();
        
        // ==================== Create Outgoing Adapters ====================
        Logger::info("Creating Outgoing Adapters...");
        
        // ZeroMQ Outgoing Adapter (RADIO socket)
        Logger::debug("Creating DelayCalcTrackDataZeroMQOutgoingAdapter (RADIO socket)...");
        auto zmqOutgoingAdapter = std::make_shared<DelayCalcTrackDataZeroMQOutgoingAdapter>();
        g_outgoingZeroMQAdapter = zmqOutgoingAdapter.get();
        
        // Custom Adapter (Moving Average Calculation)
        Logger::debug("Creating DelayCalcTrackDataCustomOutgoingAdapter (Custom Processing)...");
        auto customOutgoingAdapter = std::make_shared<DelayCalcTrackDataCustomOutgoingAdapter>();
        g_outgoingCustomAdapter = customOutgoingAdapter.get();
        
        // NOTE: Custom adapter calculates moving average of FirstHopDelayTime
        // Primary output: ZeroMQ adapter
        // Analytics output: Custom adapter (100-sample moving average)
        
        // ==================== Create Domain Use Case ====================
        Logger::debug("Creating ProcessTrackUseCase with dependencies...");
        auto domainProcessor = std::make_shared<ProcessTrackUseCase>(
            std::move(calculatorService),
            zmqOutgoingAdapter  // Primary outgoing adapter
        );
        g_domainProcessor = domainProcessor.get();
        
        // ==================== Create Incoming Adapter ====================
        Logger::debug("Creating ExtrapTrackDataZeroMQIncomingAdapter (DISH socket)...");
        auto incomingAdapter = std::make_shared<ExtrapTrackDataZeroMQIncomingAdapter>(domainProcessor);
        g_incomingAdapter = incomingAdapter.get();
        
        // ==================== System Information ====================
        Logger::info("=== System Configuration ===");
        Logger::info("Architecture: Event Queue Based (5 threads)");
        Logger::info("Messaging Input: ZeroMQ DISH (UDP multicast)");
        Logger::info("Messaging Output: ZeroMQ RADIO (TCP) + Custom Processing");
        Logger::info("Input Group: ExtrapTrackData");
        Logger::info("Output Groups: DelayCalcTrackData (ZeroMQ + Analytics)");
        Logger::info("Thread 1: Incoming Adapter (CPU 1)");
        Logger::info("Thread 2: Domain Processing (CPU 3)");
        Logger::info("Thread 3: Outgoing ZeroMQ (CPU 2)");
        Logger::info("Thread 4: Outgoing Custom (CPU 4 - Moving Average)");
        Logger::info("Thread 5: Main (lifecycle management)");
        Logger::info("Press Ctrl+C to shutdown gracefully");
        Logger::info("===============================");
        
        // ==================== Start All Components ====================
        // Component startup order is critical for event queue architecture:
        // 1. Outgoing adapters start first - ready to receive processed data
        // 2. Domain processor starts next - ready to process incoming data
        // 3. Incoming adapter starts last - begins data flow into system
        // This prevents message loss by ensuring downstream is ready before upstream
        Logger::info("Starting all components in correct order...");
        
        // Step 1: Start outgoing adapters first (must be ready to receive)
        Logger::info("Step 1: Starting outgoing adapters...");
        if (!zmqOutgoingAdapter->start()) {
            Logger::error("Failed to start ZeroMQ outgoing adapter");
            return 1;
        }
        if (!customOutgoingAdapter->start()) {
            Logger::error("Failed to start Custom outgoing adapter");
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Allow initialization
        
        // Step 2: Start domain processor
        Logger::info("Step 2: Starting domain processor...");
        if (!domainProcessor->start()) {
            Logger::error("Failed to start domain processor");
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
        // 2. Domain processor stops next - completes processing of queued messages
        // 3. Outgoing adapters stop last - ensures all processed data is sent
        // This ordering prevents data loss during shutdown
        incomingAdapter->stop();
        domainProcessor->stop();
        zmqOutgoingAdapter->stop();
        customOutgoingAdapter->stop();
        
        // Clear global pointers
        g_incomingAdapter = nullptr;
        g_domainProcessor = nullptr;
        g_outgoingZeroMQAdapter = nullptr;
        g_outgoingCustomAdapter = nullptr;
        
    } catch (const std::exception& ex) {
        Logger::error("Application error: ", ex.what());
        return 1;
    }
    
    Logger::info("=== B_Hexagon Application Shutdown Complete ===");
    return 0;
}