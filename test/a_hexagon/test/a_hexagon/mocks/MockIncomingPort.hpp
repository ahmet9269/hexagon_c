/**
 * @file MockIncomingPort.hpp
 * @brief Mock implementation of ITrackDataIncomingPort for unit testing
 * @details Records received track data for verification without side effects
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#ifndef A_HEXAGON_TEST_MOCKS_MOCK_INCOMING_PORT_HPP
#define A_HEXAGON_TEST_MOCKS_MOCK_INCOMING_PORT_HPP

#include "domain/ports/incoming/ITrackDataIncomingPort.hpp"
#include "domain/ports/TrackData.hpp"
#include <vector>
#include <mutex>
#include <optional>

namespace a_hexagon {
namespace test {
namespace mocks {

/**
 * @class MockIncomingPort
 * @brief Mock implementation for unit testing
 * @details Records all received TrackData for verification
 */
class MockIncomingPort : public domain::ports::incoming::ITrackDataIncomingPort {
public:
    MockIncomingPort() = default;
    ~MockIncomingPort() override = default;

    /**
     * @brief Record track data (mock implementation)
     * @param trackData Received track data
     */
    void processAndForwardTrackData(const domain::model::TrackData& trackData) override {
        std::lock_guard<std::mutex> lock(mutex_);
        receivedTrackData_.push_back(trackData);
        processCallCount_++;
    }

    // ==================== Test Verification Methods ====================

    /**
     * @brief Get number of received track data
     */
    [[nodiscard]] int getReceivedTrackDataCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return static_cast<int>(receivedTrackData_.size());
    }

    /**
     * @brief Get last received track data
     */
    [[nodiscard]] std::optional<domain::model::TrackData> getLastReceivedTrackData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (receivedTrackData_.empty()) {
            return std::nullopt;
        }
        return receivedTrackData_.back();
    }

    /**
     * @brief Get all received track data
     */
    [[nodiscard]] std::vector<domain::model::TrackData> getAllReceivedTrackData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return receivedTrackData_;
    }

    /**
     * @brief Get track data at specific index
     */
    [[nodiscard]] std::optional<domain::model::TrackData> getReceivedTrackDataAt(size_t index) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (index >= receivedTrackData_.size()) {
            return std::nullopt;
        }
        return receivedTrackData_[index];
    }

    /**
     * @brief Get process call count
     */
    [[nodiscard]] int getProcessCallCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return processCallCount_;
    }

    /**
     * @brief Clear all recorded data
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        receivedTrackData_.clear();
        processCallCount_ = 0;
    }

    /**
     * @brief Wait for expected number of messages with timeout
     * @param expectedCount Expected message count
     * @param timeoutMs Timeout in milliseconds
     * @return true if count reached, false on timeout
     */
    [[nodiscard]] bool waitForMessages(int expectedCount, int timeoutMs) const {
        auto start = std::chrono::steady_clock::now();
        while (true) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (static_cast<int>(receivedTrackData_.size()) >= expectedCount) {
                    return true;
                }
            }
            
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count();
            if (elapsed >= timeoutMs) {
                return false;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // Delete copy/move for thread safety
    MockIncomingPort(const MockIncomingPort&) = delete;
    MockIncomingPort& operator=(const MockIncomingPort&) = delete;
    MockIncomingPort(MockIncomingPort&&) = delete;
    MockIncomingPort& operator=(MockIncomingPort&&) = delete;

private:
    mutable std::mutex mutex_;
    std::vector<domain::model::TrackData> receivedTrackData_;
    int processCallCount_{0};
};

} // namespace mocks
} // namespace test
} // namespace a_hexagon

#endif // A_HEXAGON_TEST_MOCKS_MOCK_INCOMING_PORT_HPP
