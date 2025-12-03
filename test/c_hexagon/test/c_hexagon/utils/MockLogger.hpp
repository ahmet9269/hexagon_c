/**
 * @file MockLogger.hpp
 * @brief Mock logger implementation for unit testing
 * @details Captures log messages for verification in tests
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "utils/ILogger.hpp"
#include <vector>
#include <string>
#include <mutex>
#include <sstream>

namespace utils {
namespace test {

/**
 * @brief Mock logger that captures all log messages for test verification
 */
class MockLogger : public ILogger {
public:
    /**
     * @brief Log entry structure for verification
     */
    struct LogEntry {
        Level level;
        std::string message;
    };

    MockLogger() : current_level_(Level::Trace) {}
    ~MockLogger() override = default;

    // ILogger interface implementation
    void trace(const std::string& message) override {
        addEntry(Level::Trace, message);
    }

    void debug(const std::string& message) override {
        addEntry(Level::Debug, message);
    }

    void info(const std::string& message) override {
        addEntry(Level::Info, message);
    }

    void warn(const std::string& message) override {
        addEntry(Level::Warn, message);
    }

    void error(const std::string& message) override {
        addEntry(Level::Error, message);
    }

    void critical(const std::string& message) override {
        addEntry(Level::Critical, message);
    }

    void setLevel(Level level) override {
        current_level_ = level;
    }

    void logLatency(const std::string& component,
                    const std::string& operation,
                    int64_t latency_us) override {
        std::ostringstream oss;
        oss << "[" << component << "] " << operation << " latency: " << latency_us << " μs";
        addEntry(Level::Info, oss.str());
    }

    void logTrackReceived(int32_t track_id,
                          int64_t hop1_latency,
                          int64_t hop2_latency) override {
        std::ostringstream oss;
        oss << "Track " << track_id << " received - Hop1: " << hop1_latency 
            << " μs, Hop2: " << hop2_latency << " μs";
        addEntry(Level::Debug, oss.str());
    }

    // Test helper methods
    
    /**
     * @brief Get all logged entries
     * @return Vector of log entries
     */
    std::vector<LogEntry> getEntries() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_;
    }

    /**
     * @brief Get entry count
     * @return Number of logged entries
     */
    std::size_t getEntryCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_.size();
    }

    /**
     * @brief Get entries by level
     * @param level Level to filter by
     * @return Vector of matching entries
     */
    std::vector<LogEntry> getEntriesByLevel(Level level) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<LogEntry> result;
        for (const auto& entry : entries_) {
            if (entry.level == level) {
                result.push_back(entry);
            }
        }
        return result;
    }

    /**
     * @brief Check if any entry contains a substring
     * @param substring Substring to search for
     * @return true if found
     */
    bool containsMessage(const std::string& substring) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& entry : entries_) {
            if (entry.message.find(substring) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Clear all entries
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.clear();
    }

    /**
     * @brief Get last logged message
     * @return Last message or empty string if no entries
     */
    std::string getLastMessage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return entries_.empty() ? "" : entries_.back().message;
    }

private:
    void addEntry(Level level, const std::string& message) {
        if (static_cast<int>(level) >= static_cast<int>(current_level_)) {
            std::lock_guard<std::mutex> lock(mutex_);
            entries_.push_back({level, message});
        }
    }

    mutable std::mutex mutex_;
    std::vector<LogEntry> entries_;
    Level current_level_;
};

} // namespace test
} // namespace utils
