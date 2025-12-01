/**
 * @file main.cpp
 * @brief Application entry point demonstrating Hexagonal Architecture assembly
 */

#include "domain/logic/CalculatorService.hpp"
#include "domain/logic/ProcessTrackUseCase.hpp"
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "adapters/incoming/zeromq/ExtrapTrackDataZeroMQIncomingAdapter.hpp"
#include "adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "utils/Logger.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

// Using declarations for convenience
using domain::ports::ExtrapTrackData;
using domain::ports::DelayCalcTrackData;
using domain::logic::ProcessTrackUseCase;


/**
 * @brief Application entry point
 * 
 * Demonstrates composition root pattern where all dependencies are wired together.
 * Uses ZeroMQ RADIO/DISH pattern for UDP multicast messaging.
 */
int main() {
    Logger::info("=== B_Hexagon Track Processing System Starting ===");
    
    try {
        Logger::info("Initializing application components...");
        
        // Create domain services
        Logger::debug("Creating CalculatorService...");
        auto calculatorService = std::make_unique<CalculatorService>();
        
        // Create outgoing adapter (PUB socket for TCP)
        Logger::debug("Creating ZeroMQDataWriter (PUB socket)...");
        auto dataSender = std::make_unique<DelayCalcTrackDataZeroMQOutgoingAdapter>();
        
        // Create use case with dependencies
        Logger::debug("Creating ProcessTrackUseCase with dependencies...");
        auto useCase = std::make_unique<ProcessTrackUseCase>(
            std::move(calculatorService),
            std::move(dataSender)
        );
        
        // Create incoming adapter (DISH socket) and wire to use case
        Logger::info("Creating ExtrapTrackDataZeroMQIncomingAdapter (DISH socket)...");
        ExtrapTrackDataZeroMQIncomingAdapter dataHandler(useCase.get());
        
        Logger::info("=== System Configuration ===");
        Logger::info("Architecture: Hexagonal (Ports & Adapters)");
        Logger::info("Messaging: ZeroMQ RADIO/DISH UDP multicast");
        Logger::info("Group: TRACK_DATA_UDP");
        Logger::info("Status: Ready to receive track data");
        Logger::info("===============================");
        
        // Start receiving messages (blocks indefinitely)
        Logger::info("Starting message reception loop...");
        dataHandler.startReceiving();
        
    } catch (const std::exception& ex) {
        Logger::error("Application error: ", ex.what());
        return 1;
    }
    
    Logger::info("Application shutting down normally");
    return 0;
}