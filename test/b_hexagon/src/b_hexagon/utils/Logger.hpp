/**
 * @file Logger.hpp
 * @brief spdlog-based logging utility for B_Hexagon application
 * @details Provides thread-safe, high-performance logging with console and file output.
 *          Uses spdlog library for asynchronous logging capabilities.
 * 
 * Features:
 * - Thread-safe logging (spdlog built-in)
 * - Console output with colors
 * - Configurable log levels
 * - Variadic template support for easy usage
 * - Timestamp with millisecond precision
 * 
 * @author b_hexagon Team
 * @version 2.0 - Migrated to spdlog
 * @date 2025
 */

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>

/**
 * @brief spdlog-based Logger wrapper class
 * @details Provides static methods for logging with automatic initialization.
 *          Thread-safe and high-performance.
 */
class Logger {
public:
    /**
     * @brief Initialize the logger (called automatically on first use)
     */
    static void init() {
        if (!initialized_) {
            try {
                // Create console logger with colors
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_level(spdlog::level::trace);
                
                // Set pattern: [timestamp] [level] message
                console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
                
                // Create logger with console sink
                logger_ = std::make_shared<spdlog::logger>("b_hexagon", console_sink);
                logger_->set_level(spdlog::level::debug);
                
                // Register as default logger
                spdlog::set_default_logger(logger_);
                
                // Flush on info level and above
                spdlog::flush_on(spdlog::level::info);
                
                initialized_ = true;
            } catch (const spdlog::spdlog_ex& ex) {
                // Fallback to stderr if spdlog fails
                std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
            }
        }
    }

    /**
     * @brief Set the minimum log level
     * @param level Log level string: "trace", "debug", "info", "warn", "error", "critical"
     */
    static void setLogLevel(const std::string& level) {
        ensureInitialized();
        logger_->set_level(spdlog::level::from_str(level));
    }

    /**
     * @brief Set log level using spdlog enum
     */
    static void setLogLevel(spdlog::level::level_enum level) {
        ensureInitialized();
        logger_->set_level(level);
    }

    /**
     * @brief Log a debug message
     */
    template<typename... Args>
    static void debug(Args&&... args) {
        ensureInitialized();
        logger_->debug(concatenate(std::forward<Args>(args)...));
    }

    /**
     * @brief Log an info message
     */
    template<typename... Args>
    static void info(Args&&... args) {
        ensureInitialized();
        logger_->info(concatenate(std::forward<Args>(args)...));
    }

    /**
     * @brief Log a warning message
     */
    template<typename... Args>
    static void warn(Args&&... args) {
        ensureInitialized();
        logger_->warn(concatenate(std::forward<Args>(args)...));
    }

    /**
     * @brief Log an error message
     */
    template<typename... Args>
    static void error(Args&&... args) {
        ensureInitialized();
        logger_->error(concatenate(std::forward<Args>(args)...));
    }

    /**
     * @brief Log a critical message
     */
    template<typename... Args>
    static void critical(Args&&... args) {
        ensureInitialized();
        logger_->critical(concatenate(std::forward<Args>(args)...));
    }

    /**
     * @brief Log a trace message
     */
    template<typename... Args>
    static void trace(Args&&... args) {
        ensureInitialized();
        logger_->trace(concatenate(std::forward<Args>(args)...));
    }

    /**
     * @brief Flush all pending log messages
     */
    static void flush() {
        if (logger_) {
            logger_->flush();
        }
    }

    /**
     * @brief Shutdown the logger
     */
    static void shutdown() {
        if (logger_) {
            logger_->flush();
            spdlog::shutdown();
            initialized_ = false;
        }
    }

private:
    static inline std::shared_ptr<spdlog::logger> logger_ = nullptr;
    static inline bool initialized_ = false;

    /**
     * @brief Ensure logger is initialized before use
     */
    static void ensureInitialized() {
        if (!initialized_) {
            init();
        }
    }

    /**
     * @brief Concatenate variadic arguments into a single string
     * @details Maintains backward compatibility with existing Logger::info(a, b, c) calls
     */
    template<typename... Args>
    static std::string concatenate(Args&&... args) {
        std::ostringstream oss;
        ((oss << std::forward<Args>(args)), ...);
        return oss.str();
    }
};
