/**
 * @file MockIncomingPort.hpp
 * @brief Mock implementation of IDelayCalcTrackDataIncomingPort for testing
 * @details Captures submitted data for test verification
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "domain/ports/incoming/IDelayCalcTrackDataIncomingPort.hpp"
#include <vector>
#include <mutex>
#include <atomic>

namespace domain {
namespace ports {
namespace incoming {
namespace test {

/**
 * @brief Mock incoming port that captures all submitted data
 */
class MockIncomingPort : public IDelayCalcTrackDataIncomingPort {
public:
    MockIncomingPort() : submit_count_(0), should_throw_(false) {}
    ~MockIncomingPort() override = default;

    // IDelayCalcTrackDataIncomingPort interface
    void submitDelayCalcTrackData(const DelayCalcTrackData& data) override {
        if (should_throw_) {
            throw std::runtime_error("Mock submit failure");
        }
        std::lock_guard<std::mutex> lock(mutex_);
        submitted_data_.push_back(data);
        submit_count_++;
    }

    // Test helper methods

    /**
     * @brief Set whether submitDelayCalcTrackData should throw
     * @param should_throw true to throw on submit
     */
    void setShouldThrow(bool should_throw) {
        should_throw_ = should_throw;
    }

    /**
     * @brief Get all submitted data
     * @return Vector of submitted DelayCalcTrackData
     */
    std::vector<DelayCalcTrackData> getSubmittedData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return submitted_data_;
    }

    /**
     * @brief Get submit count
     * @return Number of times submitDelayCalcTrackData was called
     */
    int getSubmitCount() const {
        return submit_count_.load();
    }

    /**
     * @brief Get last submitted data
     * @return Last submitted DelayCalcTrackData (or default if none)
     */
    DelayCalcTrackData getLastSubmittedData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return submitted_data_.empty() ? DelayCalcTrackData() : submitted_data_.back();
    }

    /**
     * @brief Clear all captured data
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        submitted_data_.clear();
        submit_count_.store(0);
    }

    /**
     * @brief Check if any data was submitted
     * @return true if at least one submit occurred
     */
    bool hasSubmittedData() const {
        return submit_count_.load() > 0;
    }

private:
    mutable std::mutex mutex_;
    std::vector<DelayCalcTrackData> submitted_data_;
    std::atomic<int> submit_count_;
    bool should_throw_;
};

} // namespace test
} // namespace incoming
} // namespace ports
} // namespace domain
