#include <iostream>
#include <chrono>
#include <csignal>
#include <atomic>
#include <memory>
#include <thread>

#include "../domain/model/DelayCalcTrackData.hpp"
#include "../domain/model/FinalCalcTrackData.hpp"
#include "../domain/logic/FinalCalculationService.hpp"
#include "../adapters/outgoing/CustomFinalTrackDataAdapter.hpp"
#include "../adapters/incoming/zeromq/ZeroMQDishTrackDataSubscriber.hpp"

// Using declarations for convenience
using domain::model::DelayCalcTrackData;
using domain::model::FinalCalcTrackData;

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\nShutdown signal received. Exiting..." << std::endl;
    running.store(false);
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== C_Hexagon - Final Track Data Processing System ===" << std::endl;
        std::cout << "Architecture: Hexagonal (Ports & Adapters)" << std::endl;
        std::cout << "Input: DelayCalcTrackData from B_hexagon via ZeroMQ DISH" << std::endl;
        std::cout << "Output: FinalCalcTrackData via Custom Adapter" << std::endl;
        std::cout << "Group: DelayCalcTrackData" << std::endl;
        std::cout << "Endpoint: udp://239.1.1.5:9595" << std::endl;
        std::cout << "========================================================" << std::endl;

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        // Hexagonal Architecture: Dependency Injection
        // 1. Create Outgoing Adapter (Infrastructure)
        auto customOutgoingAdapter = std::make_unique<adapters::outgoing::CustomFinalTrackDataAdapter>();
        
        // 2. Create Domain Service (Business Logic) - implements incoming port
        auto finalCalculationService = std::make_shared<domain::logic::FinalCalculationService>(
            std::move(customOutgoingAdapter));
        
        // 3. Create Incoming Adapter (Infrastructure) - uses domain service via port
        adapters::incoming::zeromq::ZeroMQDishTrackDataSubscriber subscriber(finalCalculationService);
        
        std::cout << "🚀 Starting DelayCalcTrackData reception from B_hexagon..." << std::endl;
        std::cout << "📡 Listening on udp://239.1.1.5:9595 (DelayCalcTrackData group)" << std::endl;
        std::cout << "⏹️  Press Ctrl+C to stop..." << std::endl;

        // Start subscriber in background thread
        if (!subscriber.start()) {
            std::cerr << "❌ Failed to start ZeroMQ subscriber" << std::endl;
            return 1;
        }

        // Main thread waits for shutdown signal
        while (running.load() && subscriber.isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Graceful shutdown
        subscriber.stop();

        std::cout << "✅ C_hexagon shutdown complete." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ FATAL ERROR in C_hexagon main: " << e.what() << std::endl;
        return 1;
    }
}