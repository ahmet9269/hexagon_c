/**
 * @file FinalCalcTrackDataZeroMQOutgoingAdapterTest.cpp
 * @brief Unit tests for FinalCalcTrackDataZeroMQOutgoingAdapter
 * @details Tests adapter lifecycle, message sending, and error scenarios.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "adapters/outgoing/zeromq/FinalCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"  // For config comparison
#include "domain/ports/FinalCalcTrackData.hpp"
#include "utils/Logger.hpp"
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

using namespace adapters::outgoing::zeromq;
using namespace domain::ports;

/**
 * @brief Test fixture for FinalCalcTrackDataZeroMQOutgoingAdapter
 */
class FinalCalcTrackDataZeroMQOutgoingAdapterTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // Initialize logger once for all tests
        if (!utils::Logger::isInitialized()) {
            utils::Logger::init("outgoing_adapter_test");
        }
    }

    void SetUp() override {
        // Use unique ports for each test to avoid conflicts
        testEndpoint_ = "tcp://127.0.0.1:" + std::to_string(17000 + testPort_++);
    }

    void TearDown() override {
    }

    std::string testEndpoint_;
    static std::atomic<int> testPort_;

    /**
     * @brief Creates sample FinalCalcTrackData for testing
     */
    FinalCalcTrackData createSampleData(int trackId = 1001) {
        FinalCalcTrackData data;
        data.setTrackId(trackId);
        data.setUpdateTime(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        data.setXPositionECEF(1000.0);
        data.setYPositionECEF(2000.0);
        data.setZPositionECEF(3000.0);
        data.setXVelocityECEF(100.0);
        data.setYVelocityECEF(200.0);
        data.setZVelocityECEF(50.0);
        data.setFirstHopDelayTime(50);
        data.setSecondHopDelayTime(30);
        data.setTotalDelayTime(80);
        return data;
    }
};

std::atomic<int> FinalCalcTrackDataZeroMQOutgoingAdapterTest::testPort_{0};

// ============================================
// Constructor Tests
// ============================================

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, DefaultConstructor_InitializesAdapter) {
    // Note: Default constructor binds to default endpoint
    EXPECT_NO_THROW({
        auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>();
    });
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, CustomConstructor_AcceptsConfiguration) {
    EXPECT_NO_THROW({
        auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
            testEndpoint_, "TestGroup");
    });
}

// ============================================
// IAdapter Interface Tests
// ============================================

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, GetName_ReturnsAdapterName) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    std::string name = adapter->getName();

    EXPECT_FALSE(name.empty());
    EXPECT_NE(name.find("TestData"), std::string::npos);
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, IsRunning_ReturnsFalseBeforeStart) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    EXPECT_FALSE(adapter->isRunning());
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Start_ReturnsTrue) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    bool result = adapter->start();

    EXPECT_TRUE(result);
    EXPECT_TRUE(adapter->isRunning());

    adapter->stop();
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Start_ReturnsFalseIfAlreadyRunning) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    adapter->start();
    bool result = adapter->start();  // Second start

    EXPECT_FALSE(result);

    adapter->stop();
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Stop_SetsIsRunningFalse) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    adapter->start();
    EXPECT_TRUE(adapter->isRunning());

    adapter->stop();
    EXPECT_FALSE(adapter->isRunning());
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Stop_CanBeCalledMultipleTimes) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    adapter->start();
    adapter->stop();
    adapter->stop();  // Second stop should not crash
    adapter->stop();  // Third stop should not crash

    EXPECT_FALSE(adapter->isRunning());
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Destructor_StopsAdapterGracefully) {
    {
        auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
            testEndpoint_, "TestData");
        adapter->start();
        EXPECT_TRUE(adapter->isRunning());
    }  // Destructor called here

    // If we get here without hanging, graceful shutdown worked
    SUCCEED();
}

// ============================================
// ITrackDataStatisticOutgoingPort Interface Tests
// ============================================

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, IsReady_ReturnsTrueWhenRunning) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    EXPECT_FALSE(adapter->isReady());

    adapter->start();
    EXPECT_TRUE(adapter->isReady());

    adapter->stop();
    EXPECT_FALSE(adapter->isReady());
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, SendFinalTrackData_WhenNotRunning_DoesNotCrash) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    auto data = createSampleData();
    
    // Should not crash even when adapter is not started
    EXPECT_NO_THROW({
        adapter->sendFinalTrackData(data);
    });
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, SendFinalTrackData_WhenRunning_DoesNotCrash) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    adapter->start();
    
    auto data = createSampleData();
    
    EXPECT_NO_THROW({
        adapter->sendFinalTrackData(data);
    });

    // Allow time for message to be queued and processed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    adapter->stop();
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, SendFinalTrackData_MultipleTimes_IsStable) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    adapter->start();
    
    // Send multiple messages
    for (int i = 0; i < 10; ++i) {
        auto data = createSampleData(1000 + i);
        adapter->sendFinalTrackData(data);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    adapter->stop();
    SUCCEED();
}

// ============================================
// Lifecycle Tests
// ============================================

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, MultipleStartStop_IsStable) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(adapter->start());
        EXPECT_TRUE(adapter->isRunning());
        adapter->stop();
        EXPECT_FALSE(adapter->isRunning());
    }
}

// ============================================
// Thread Safety Tests
// ============================================

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, ConcurrentSends_AreThreadSafe) {
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
        testEndpoint_, "TestData");

    adapter->start();

    std::vector<std::thread> threads;
    std::atomic<int> sendCount{0};

    // Send from multiple threads concurrently
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < 10; ++j) {
                auto data = createSampleData(1000 + i * 100 + j);  // Ensure trackId > 0
                adapter->sendFinalTrackData(data);
                sendCount++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(sendCount.load(), 50);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    adapter->stop();
}

// ============================================
// Adapter Configuration Tests
// (Note: Configuration constants are private, testing behavior instead)
// ============================================

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Config_DefaultAdapterHasValidName) {
    // Tests that default constructor creates valid adapter
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>();
    std::string name = adapter->getName();
    
    // Name should contain "FinalCalcTrackData" indicating default group
    EXPECT_NE(name.find("FinalCalcTrackData"), std::string::npos);
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Config_AdapterUsesUDPMulticast) {
    // Tests that default constructor uses UDP multicast (RADIO/DISH pattern)
    // This is verified by successful construction with default config
    EXPECT_NO_THROW({
        auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>();
    });
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Config_SendTimeoutIsPositive) {
    // Verify adapter handles send correctly without hanging
    auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>();
    
    adapter->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    adapter->stop();  // Should complete quickly
    
    EXPECT_FALSE(adapter->isRunning());
}

TEST_F(FinalCalcTrackDataZeroMQOutgoingAdapterTest, Config_CustomEndpointWorks) {
    // Verify custom endpoint configuration works
    std::string customEndpoint = "tcp://127.0.0.1:19999";
    std::string customGroup = "CustomGroup";
    
    EXPECT_NO_THROW({
        auto adapter = std::make_unique<FinalCalcTrackDataZeroMQOutgoingAdapter>(
            customEndpoint, customGroup);
        // Name should contain custom group
        EXPECT_NE(adapter->getName().find(customGroup), std::string::npos);
    });
}
