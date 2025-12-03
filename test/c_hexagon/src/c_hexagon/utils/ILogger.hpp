/**
 * @file ILogger.hpp
 * @brief Logger interface for dependency injection and testability
 * @details Abstracts logging operations to enable mock implementations in tests
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#pragma once

#include <string>
#include <cstdint>

namespace utils {

/**
 * @brief Abstract interface for logging operations
 * @details Enables dependency injection of loggers for testability
 *          Implementations can use spdlog, file logging, or mock logging
 */
class ILogger {
public:
    /**
     * @brief Log levels
     */
    enum class Level {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Critical = 5,
        Off = 6
    };

    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~ILogger() = default;

    /**
     * @brief Log trace message
     * @param message Log message
     */
    virtual void trace(const std::string& message) = 0;

    /**
     * @brief Log debug message
     * @param message Log message
     */
    virtual void debug(const std::string& message) = 0;

    /**
     * @brief Log info message
     * @param message Log message
     */
    virtual void info(const std::string& message) = 0;

    /**
     * @brief Log warning message
     * @param message Log message
     */
    virtual void warn(const std::string& message) = 0;

    /**
     * @brief Log error message
     * @param message Log message
     */
    virtual void error(const std::string& message) = 0;

    /**
     * @brief Log critical message
     * @param message Log message
     */
    virtual void critical(const std::string& message) = 0;

    /**
     * @brief Set minimum log level
     * @param level Minimum level to log
     */
    virtual void setLevel(Level level) = 0;

    /**
     * @brief Log latency measurement
     * @param component Component name
     * @param operation Operation name
     * @param latency_us Latency in microseconds
     */
    virtual void logLatency(const std::string& component,
                            const std::string& operation,
                            int64_t latency_us) = 0;

    /**
     * @brief Log track data reception
     * @param track_id Track identifier
     * @param hop1_latency First hop latency in microseconds
     * @param hop2_latency Second hop latency in microseconds
     */
    virtual void logTrackReceived(int32_t track_id,
                                  int64_t hop1_latency,
                                  int64_t hop2_latency) = 0;

protected:
    ILogger() = default;
    ILogger(const ILogger&) = default;
    ILogger(ILogger&&) = default;
    ILogger& operator=(const ILogger&) = default;
    ILogger& operator=(ILogger&&) = default;
};

} // namespace utils
