/**
 * @file main.hpp
 * @brief Main application header for a_hexagon
 * @details Thread-per-Type Hexagonal Architecture declarations
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 */

#ifndef MAIN_HPP
#define MAIN_HPP

#include <csignal>
#include <atomic>

/**
 * @brief Global flag for shutdown signaling
 */
extern std::atomic<bool> g_shutdown_requested;

/**
 * @brief Signal handler for graceful shutdown
 * @param signal Signal number (SIGINT, SIGTERM)
 */
void signalHandler(int signal);

/**
 * @brief Initialize application logging
 * @return true if successful
 */
bool initializeLogging();

/**
 * @brief Setup signal handlers
 */
void setupSignalHandlers();

#endif //MAIN_HPP
