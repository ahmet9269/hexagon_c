/**
 * @file main.hpp
 * @brief Main application header for a_hexagon
 * @details Thread-per-Type Hexagonal Architecture declarations.
 *          Provides signal handling and initialization functions
 *          for graceful startup and shutdown.
 * 
 * @author A Hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see main.cpp
 */

#ifndef MAIN_HPP
#define MAIN_HPP

#include <csignal>
#include <atomic>

/**
 * @brief Global flag for shutdown signaling
 * @details Atomic boolean that signals application shutdown.
 *          Set to true by signal handlers (SIGINT, SIGTERM).
 * @note Thread-safe via std::atomic
 */
extern std::atomic<bool> g_shutdown_requested;

/**
 * @brief Signal handler for graceful shutdown
 * @param signum Signal number received (SIGINT, SIGTERM)
 * @post g_shutdown_requested is set to true
 * @note Registered via std::signal() in main()
 */
void signalHandler(int signum);

/**
 * @brief Initialize application logging
 * @details Initializes spdlog async logger with console output.
 *          Must be called before any logging statements.
 * @return true if logging initialized successfully, false otherwise
 * @post utils::Logger::isInitialized() returns true
 */
bool initializeLogging();

/**
 * @brief Setup signal handlers for graceful shutdown
 * @details Registers signalHandler() for SIGINT and SIGTERM.
 *          Enables clean shutdown via Ctrl+C or kill command.
 * @post Signal handlers are registered
 */
void setupSignalHandlers();

#endif //MAIN_HPP
