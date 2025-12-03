/**
 * @file MockOutgoingPort.hpp
 * @brief Mock implementation of ITrackDataStatisticOutgoingPort for testing
 * @details Captures sent data for test verification
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "domain/ports/outgoing/ITrackDataStatisticOutgoingPort.hpp"
#include <vector>
#include <mutex>
#include <atomic>

namespace domain {
namespace ports {
namespace outgoing {
namespace test {

/**
 * @brief Mock outgoing port that captures all sent data
 */
class MockOutgoingPort : public ITrackDataStatisticOutgoingPort {
public:
    MockOutgoingPort() : ready_(true), send_count_(0), should_throw_(false) {}
    ~MockOutgoingPort() override = default;

    // ITrackDataStatisticOutgoingPort interface
    void sendFinalTrackData(const FinalCalcTrackData& data) override {
        if (should_throw_) {
            throw std::runtime_error("Mock send failure");
        }
        std::lock_guard<std::mutex> lock(mutex_);
        sent_data_.push_back(data);
        send_count_++;
    }

    bool isReady() const override {
        return ready_.load();
    }

    // Test helper methods

    /**
     * @brief Set ready state
     * @param ready Whether port is ready
     */
    void setReady(bool ready) {
        ready_.store(ready);
    }

    /**
     * @brief Set whether sendFinalTrackData should throw
     * @param should_throw true to throw on send
     */
    void setShouldThrow(bool should_throw) {
        should_throw_ = should_throw;
    }

    /**
     * @brief Get all sent data
     * @return Vector of sent FinalCalcTrackData
     */
    std::vector<FinalCalcTrackData> getSentData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sent_data_;
    }

    /**
     * @brief Get send count
     * @return Number of times sendFinalTrackData was called
     */
    int getSendCount() const {
        return send_count_.load();
    }

    /**
     * @brief Get last sent data
     * @return Last sent FinalCalcTrackData (or default if none)
     */
    FinalCalcTrackData getLastSentData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sent_data_.empty() ? FinalCalcTrackData() : sent_data_.back();
    }

    /**
     * @brief Clear all captured data
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        sent_data_.clear();
        send_count_.store(0);
    }

    /**
     * @brief Check if any data was sent
     * @return true if at least one send occurred
     */
    bool hasSentData() const {
        return send_count_.load() > 0;
    }

private:
    mutable std::mutex mutex_;
    std::vector<FinalCalcTrackData> sent_data_;
    std::atomic<bool> ready_;
    std::atomic<int> send_count_;
    bool should_throw_;
};

} // namespace test
} // namespace outgoing
} // namespace ports
} // namespace domain
