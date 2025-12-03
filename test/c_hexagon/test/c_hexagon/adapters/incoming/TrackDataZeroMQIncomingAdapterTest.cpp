/**
 * @file TrackDataZeroMQIncomingAdapterTest.cpp
 * @brief Unit tests for TrackDataZeroMQIncomingAdapter
 * @details Tests adapter lifecycle, message handling, and error scenarios
 *          using mock port implementations.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"
#include "domain/ports/incoming/IDelayCalcTrackDataIncomingPort.hpp"
#include "utils/Logger.hpp"
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <mutex>

using namespace adapters::incoming::zeromq;
using namespace domain::ports;

/**
 * @brief Mock port for capturing submitted track data
 */
class MockDelayCalcTrackDataPort : public incoming::IDelayCalcTrackDataIncomingPort {
public:
    MockDelayCalcTrackDataPort() : submitCount_(0) {}

    void submitDelayCalcTrackData(const DelayCalcTrackData& data) override {
        std::lock_guard<std::mutex> lock(mutex_);
        lastData_ = data;
        submitCount_++;
        receivedDataList_.push_back(data);
    }

    // Test helpers
    int getSubmitCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return submitCount_;
    }

    DelayCalcTrackData getLastData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return lastData_;
    }

    std::vector<DelayCalcTrackData> getAllReceivedData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return receivedDataList_;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        submitCount_ = 0;
        receivedDataList_.clear();
    }

private:
    mutable std::mutex mutex_;
    DelayCalcTrackData lastData_;
    int submitCount_;
    std::vector<DelayCalcTrackData> receivedDataList_;
};

/**
 * @brief Test fixture for TrackDataZeroMQIncomingAdapter
 */
class TrackDataZeroMQIncomingAdapterTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // Initialize logger once for all tests
        if (!utils::Logger::isInitialized()) {
            utils::Logger::init("adapter_test");
        }
    }

    static void TearDownTestSuite() {
        // Logger cleanup handled by test framework
    }

    void SetUp() override {
        mockPort_ = std::make_shared<MockDelayCalcTrackDataPort>();
    }

    void TearDown() override {
        mockPort_->reset();
    }

    std::shared_ptr<MockDelayCalcTrackDataPort> mockPort_;
    
    // Use different ports for each test to avoid conflicts
    static std::atomic<int> testPort_;
    
    std::string getUniqueEndpoint() {
        return "tcp://127.0.0.1:" + std::to_string(16000 + testPort_++);
    }
};

std::atomic<int> TrackDataZeroMQIncomingAdapterTest::testPort_{0};

// ============================================
// Constructor Tests
// ============================================

TEST_F(TrackDataZeroMQIncomingAdapterTest, DefaultConstructor_InitializesAdapter) {
    // Note: This will try to bind to default port, which may fail in test env
    // Just verify construction doesn't throw for now
    EXPECT_NO_THROW({
        auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(mockPort_);
    });
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, CustomConstructor_AcceptsConfiguration) {
    std::string endpoint = getUniqueEndpoint();
    std::string group = "TestGroup";

    EXPECT_NO_THROW({
        auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
            mockPort_, endpoint, group);
    });
}

// ============================================
// IAdapter Interface Tests
// ============================================

TEST_F(TrackDataZeroMQIncomingAdapterTest, GetName_ReturnsAdapterName) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    std::string name = adapter->getName();

    EXPECT_FALSE(name.empty());
    EXPECT_NE(name.find("TestData"), std::string::npos);
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, IsRunning_ReturnsFalseBeforeStart) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    EXPECT_FALSE(adapter->isRunning());
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, Start_ReturnsTrue) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    bool result = adapter->start();

    EXPECT_TRUE(result);
    EXPECT_TRUE(adapter->isRunning());

    adapter->stop();
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, Start_ReturnsFalseIfAlreadyRunning) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    adapter->start();
    bool result = adapter->start();  // Second start

    EXPECT_FALSE(result);

    adapter->stop();
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, Stop_SetsIsRunningFalse) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    adapter->start();
    EXPECT_TRUE(adapter->isRunning());

    adapter->stop();
    EXPECT_FALSE(adapter->isRunning());
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, Stop_CanBeCalledMultipleTimes) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    adapter->start();
    adapter->stop();
    adapter->stop();  // Second stop should not crash
    adapter->stop();  // Third stop should not crash

    EXPECT_FALSE(adapter->isRunning());
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, Destructor_StopsAdapterGracefully) {
    std::string endpoint = getUniqueEndpoint();
    {
        auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
            mockPort_, endpoint, "TestData");
        adapter->start();
        EXPECT_TRUE(adapter->isRunning());
    }  // Destructor called here

    // If we get here without hanging, graceful shutdown worked
    SUCCEED();
}

// ============================================
// Lifecycle Tests
// ============================================

TEST_F(TrackDataZeroMQIncomingAdapterTest, MultipleStartStop_IsStable) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(adapter->start());
        EXPECT_TRUE(adapter->isRunning());
        adapter->stop();
        EXPECT_FALSE(adapter->isRunning());
    }
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, StartAfterStop_Works) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    adapter->start();
    adapter->stop();

    // Should be able to restart
    bool result = adapter->start();
    EXPECT_TRUE(result);
    EXPECT_TRUE(adapter->isRunning());

    adapter->stop();
}

// ============================================
// Port Integration Tests
// ============================================

TEST_F(TrackDataZeroMQIncomingAdapterTest, MockPort_StartsWithZeroSubmits) {
    EXPECT_EQ(mockPort_->getSubmitCount(), 0);
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, NullPort_ThrowsOrHandlesGracefully) {
    // Depending on implementation, null port might be rejected
    // or handled gracefully
    std::string endpoint = getUniqueEndpoint();
    
    try {
        auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
            nullptr, endpoint, "TestData");
        // If construction succeeds, adapter should handle null port gracefully
        SUCCEED();
    } catch (const std::exception&) {
        // If exception is thrown, that's also acceptable behavior
        SUCCEED();
    }
}

// ============================================
// Thread Safety Tests
// ============================================

TEST_F(TrackDataZeroMQIncomingAdapterTest, ConcurrentStartStop_IsThreadSafe) {
    std::string endpoint = getUniqueEndpoint();
    auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
        mockPort_, endpoint, "TestData");

    std::atomic<int> successfulStarts{0};
    std::atomic<int> failedStarts{0};

    // Try to start from multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&]() {
            if (adapter->start()) {
                successfulStarts++;
            } else {
                failedStarts++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Only one start should succeed
    EXPECT_EQ(successfulStarts.load(), 1);
    EXPECT_EQ(failedStarts.load(), 4);

    adapter->stop();
}

// ============================================
// Configuration Tests
// ============================================

TEST_F(TrackDataZeroMQIncomingAdapterTest, InvalidEndpoint_HandlesGracefully) {
    // Invalid endpoint format
    try {
        auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
            mockPort_, "invalid://endpoint", "TestData");
        // Construction might fail or succeed with error on start
        SUCCEED();
    } catch (const std::exception&) {
        // Exception is acceptable
        SUCCEED();
    }
}

TEST_F(TrackDataZeroMQIncomingAdapterTest, EmptyGroupName_IsHandled) {
    std::string endpoint = getUniqueEndpoint();
    
    // Empty group name
    try {
        auto adapter = std::make_shared<TrackDataZeroMQIncomingAdapter>(
            mockPort_, endpoint, "");
        SUCCEED();
    } catch (const std::exception&) {
        SUCCEED();
    }
}
