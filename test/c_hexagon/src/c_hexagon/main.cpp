/**
 * @file c_hexagon.cpp
 * @brief Main entry point for C_Hexagon application
 * @details Initializes the async logger and starts the hexagonal architecture components
 */

#include <iostream>
#include "utils/Logger.hpp"

int main() {
    try {
        // Initialize async logger (call once at startup)
        utils::Logger::init("c_hexagon");
        
        LOG_INFO("c_hexagon application is starting...");
        LOG_DEBUG("Async logger initialized with queue size: 8192");
        
        // TODO: Initialize hexagonal architecture components
        // - ZeroMQ incoming adapter
        // - Domain services
        // - Outgoing adapters
        
        LOG_INFO("c_hexagon application started successfully");
        
        // Shutdown logger gracefully
        utils::Logger::shutdown();
        
    } catch (const std::exception& e) {
        LOG_CRITICAL("c_hexagon application main error: {}", e.what());
        utils::Logger::shutdown();
        return 1;
    }
    
    return 0;
}
