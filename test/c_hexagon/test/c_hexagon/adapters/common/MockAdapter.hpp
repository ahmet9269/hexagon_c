/**
 * @file MockAdapter.hpp
 * @brief Mock implementation of IAdapter interface for testing
 * @details Provides configurable behavior for testing adapter lifecycle
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include <atomic>
#include <string>

namespace adapters {
namespace test {

/**
 * @brief Mock adapter for testing AdapterManager and MessagePipeline
 */
class MockAdapter : public IAdapter {
public:
    /**
     * @brief Constructor with optional name
     * @param name Adapter name for identification
     */
    explicit MockAdapter(std::string name = "MockAdapter")
        : name_(std::move(name))
        , running_(false)
        , start_should_fail_(false)
        , start_count_(0)
        , stop_count_(0) {}

    ~MockAdapter() override = default;

    // IAdapter interface implementation
    [[nodiscard]] bool start() override {
        start_count_++;
        if (start_should_fail_) {
            return false;
        }
        running_ = true;
        return true;
    }

    void stop() override {
        stop_count_++;
        running_ = false;
    }

    [[nodiscard]] bool isRunning() const override {
        return running_.load();
    }

    [[nodiscard]] std::string getName() const override {
        return name_;
    }

    // Test helper methods

    /**
     * @brief Set whether start() should fail
     * @param should_fail true to make start() return false
     */
    void setStartShouldFail(bool should_fail) {
        start_should_fail_ = should_fail;
    }

    /**
     * @brief Get number of times start() was called
     * @return Start call count
     */
    int getStartCount() const {
        return start_count_.load();
    }

    /**
     * @brief Get number of times stop() was called
     * @return Stop call count
     */
    int getStopCount() const {
        return stop_count_.load();
    }

    /**
     * @brief Reset all counters and state
     */
    void reset() {
        running_ = false;
        start_should_fail_ = false;
        start_count_ = 0;
        stop_count_ = 0;
    }

    /**
     * @brief Set adapter name
     * @param name New name
     */
    void setName(std::string name) {
        name_ = std::move(name);
    }

private:
    std::string name_;
    std::atomic<bool> running_;
    bool start_should_fail_;
    std::atomic<int> start_count_;
    std::atomic<int> stop_count_;
};

} // namespace test
} // namespace adapters
