/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapterTest.cpp
 * @brief Unit tests for DelayCalcTrackDataZeroMQOutgoingAdapter
 * @details Tests outgoing adapter with mock socket injection (DIP)
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>

#include "adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "mocks/MockMessageSocket.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"

using namespace domain::ports;
using namespace adapters::common::messaging;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::SaveArg;

/**
 * @brief Test fixture for DelayCalcTrackDataZeroMQOutgoingAdapter
 */
class DelayCalcTrackDataZeroMQOutgoingAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup is done per test
    }

    void TearDown() override {
        // Clean up
    }

    /**
     * @brief Create valid DelayCalcTrackData for testing
     */
    DelayCalcTrackData createValidData() {
        DelayCalcTrackData data;
        data.setTrackId(1234);
        data.setXPositionECEF(4000000.0);
        data.setYPositionECEF(3000000.0);
        data.setZPositionECEF(5000000.0);
        data.setXVelocityECEF(100.0);
        data.setYVelocityECEF(200.0);
        data.setZVelocityECEF(50.0);
        data.setUpdateTime(1700000000000L);
        data.setOriginalUpdateTime(1700000000000L);
        data.setFirstHopSentTime(1700000000100L);
        data.setFirstHopDelayTime(150);
        data.setSecondHopSentTime(1700000000200L);
        return data;
    }
};

// ==================== Constructor Tests ====================

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, DefaultConstructor_CreatesSuccessfully) {
    EXPECT_NO_THROW({
        DelayCalcTrackDataZeroMQOutgoingAdapter adapter;
    });
}

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, Constructor_WithEndpoint_CreatesSuccessfully) {
    EXPECT_NO_THROW({
        DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
            "tcp://127.0.0.1:5556",
            "DelayCalcTrack"
        );
    });
}

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, Constructor_WithMockSocket_CreatesSuccessfully) {
    auto mockSocket = std::make_unique<mocks::MockMessageSocket>();
    
    EXPECT_CALL(*mockSocket, connect(_)).WillRepeatedly(Return(true));
    
    EXPECT_NO_THROW({
        DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
            std::move(mockSocket),
            "TestGroup"
        );
    });
}

// ==================== Adapter Info Tests ====================

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, GetName_ReturnsNonEmpty) {
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter;
    
    EXPECT_FALSE(adapter.getName().empty());
}

// ==================== Lifecycle Tests ====================

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, IsRunning_BeforeStart_ReturnsFalse) {
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter;
    
    EXPECT_FALSE(adapter.isRunning());
}

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, Start_ReturnsTrue) {
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter;
    
    bool result = adapter.start();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(adapter.isRunning());
    
    adapter.stop();
}

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, Stop_AfterStart_StopsSuccessfully) {
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter;
    
    adapter.start();
    adapter.stop();
    
    EXPECT_FALSE(adapter.isRunning());
}

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, StartStop_SingleCycle_WorksCorrectly) {
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter;
    
    adapter.start();
    EXPECT_TRUE(adapter.isRunning());
    
    adapter.stop();
    EXPECT_FALSE(adapter.isRunning());
}

// ==================== Mock Socket Injection Test ====================

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, SendData_WithMockSocket_CallsSend) {
    auto mockSocket = std::make_unique<mocks::MockMessageSocket>();
    
    // Socket is already injected, no connect call expected
    EXPECT_CALL(*mockSocket, isConnected()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mockSocket, send(_, _)).WillOnce(Return(true));
    
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
        std::move(mockSocket),
        "TestGroup"
    );
    
    adapter.start();
    
    auto data = createValidData();
    adapter.sendDelayCalcTrackData(data);
    
    adapter.stop();
}

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, SendData_MultipleTimes_CallsSendMultipleTimes) {
    auto mockSocket = std::make_unique<mocks::MockMessageSocket>();
    
    // Socket is already injected, no connect call expected
    EXPECT_CALL(*mockSocket, isConnected()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mockSocket, send(_, _)).Times(3).WillRepeatedly(Return(true));
    
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
        std::move(mockSocket),
        "TestGroup"
    );
    
    adapter.start();
    
    for (int i = 0; i < 3; ++i) {
        auto data = createValidData();
        data.setTrackId(i + 1);
        adapter.sendDelayCalcTrackData(data);
    }
    
    adapter.stop();
}

// ==================== Thread Safety Tests ====================

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, ConcurrentSend_ThreadSafe) {
    auto mockSocket = std::make_unique<mocks::MockMessageSocket>();
    std::atomic<int> sendCount{0};
    
    // Socket is already injected, no connect call expected
    EXPECT_CALL(*mockSocket, isConnected()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mockSocket, send(_, _))
        .WillRepeatedly(DoAll(
            Invoke([&sendCount](const std::vector<uint8_t>&, const std::string&) { 
                ++sendCount; 
            }),
            Return(true)
        ));
    
    DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
        std::move(mockSocket),
        "TestGroup"
    );
    
    adapter.start();
    
    // Send from multiple threads
    std::vector<std::thread> threads;
    const int numThreads = 5;
    const int messagesPerThread = 10;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&adapter, i, messagesPerThread, this]() {
            for (int j = 0; j < messagesPerThread; ++j) {
                auto data = createValidData();
                data.setTrackId((i + 1) * 100 + j + 1);  // Ensure trackId >= 1
                adapter.sendDelayCalcTrackData(data);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    adapter.stop();
    
    // All sends should complete without crash
    EXPECT_EQ(sendCount.load(), numThreads * messagesPerThread);
}

// ==================== Destructor Tests ====================

TEST_F(DelayCalcTrackDataZeroMQOutgoingAdapterTest, Destructor_WhileRunning_StopsGracefully) {
    {
        DelayCalcTrackDataZeroMQOutgoingAdapter adapter;
        adapter.start();
        // Destructor called here - should stop gracefully
    }
    
    // No crash = success
    SUCCEED();
}
