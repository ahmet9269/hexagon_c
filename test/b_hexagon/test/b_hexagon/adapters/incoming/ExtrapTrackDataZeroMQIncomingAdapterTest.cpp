/**
 * @file ExtrapTrackDataZeroMQIncomingAdapterTest.cpp
 * @brief Unit tests for ExtrapTrackDataZeroMQIncomingAdapter
 * @details Tests incoming adapter with mock socket injection (DIP)
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

#include "adapters/incoming/zeromq/ExtrapTrackDataZeroMQIncomingAdapter.hpp"
#include "mocks/MockMessageSocket.hpp"
#include "mocks/MockIncomingPort.hpp"
#include "domain/ports/ExtrapTrackData.hpp"

using namespace domain::ports;
using namespace adapters::common::messaging;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::AtLeast;
using ::testing::DoAll;

/**
 * @brief Test fixture for ExtrapTrackDataZeroMQIncomingAdapter
 */
class ExtrapTrackDataZeroMQIncomingAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_mockIncomingPort = std::make_shared<mocks::MockIncomingPort>();
    }

    void TearDown() override {
        // Clean up any running adapters
    }

    /**
     * @brief Create valid serialized ExtrapTrackData
     */
    std::vector<uint8_t> createValidSerializedData() {
        ExtrapTrackData data;
        data.setTrackId(1234);
        data.setXPositionECEF(4000000.0);
        data.setYPositionECEF(3000000.0);
        data.setZPositionECEF(5000000.0);
        data.setXVelocityECEF(100.0);
        data.setYVelocityECEF(200.0);
        data.setZVelocityECEF(50.0);
        data.setUpdateTime(1700000000000L);
        return data.serialize();
    }

    std::shared_ptr<mocks::MockIncomingPort> m_mockIncomingPort;
};

// ==================== Constructor Tests ====================

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, Constructor_WithPort_CreatesSuccessfully) {
    EXPECT_NO_THROW({
        ExtrapTrackDataZeroMQIncomingAdapter adapter(m_mockIncomingPort);
    });
}

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, Constructor_WithCustomEndpoint_CreatesSuccessfully) {
    EXPECT_NO_THROW({
        ExtrapTrackDataZeroMQIncomingAdapter adapter(
            m_mockIncomingPort,
            "udp://239.192.1.1:5555",
            "ExtrapTrack"
        );
    });
}

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, Constructor_NullPort_CreatesSuccessfully) {
    // Adapter can be created with nullptr port (uses default configuration)
    EXPECT_NO_THROW({
        ExtrapTrackDataZeroMQIncomingAdapter adapter(nullptr);
    });
}

// ==================== Adapter Info Tests ====================

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, GetName_ReturnsNonEmpty) {
    ExtrapTrackDataZeroMQIncomingAdapter adapter(m_mockIncomingPort);
    
    EXPECT_FALSE(adapter.getName().empty());
}

// ==================== Lifecycle Tests ====================

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, IsRunning_BeforeStart_ReturnsFalse) {
    ExtrapTrackDataZeroMQIncomingAdapter adapter(m_mockIncomingPort);
    
    EXPECT_FALSE(adapter.isRunning());
}

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, Start_ReturnsTrue) {
    ExtrapTrackDataZeroMQIncomingAdapter adapter(m_mockIncomingPort);
    
    bool result = adapter.start();
    
    // Give time for startup
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(adapter.isRunning());
    
    adapter.stop();
}

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, Stop_AfterStart_StopsSuccessfully) {
    ExtrapTrackDataZeroMQIncomingAdapter adapter(m_mockIncomingPort);
    
    adapter.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    adapter.stop();
    
    // Give time for shutdown
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    EXPECT_FALSE(adapter.isRunning());
}

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, StartStop_SingleCycle_WorksCorrectly) {
    ExtrapTrackDataZeroMQIncomingAdapter adapter(m_mockIncomingPort);
    
    adapter.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_TRUE(adapter.isRunning());
    
    adapter.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_FALSE(adapter.isRunning());
}

// ==================== Mock Socket Injection Test ====================

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, Constructor_WithMockSocket_Accepted) {
    auto mockSocket = std::make_unique<mocks::MockMessageSocket>();
    
    EXPECT_CALL(*mockSocket, connect(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*mockSocket, subscribe(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*mockSocket, receive(_)).WillRepeatedly(Return(std::nullopt));
    EXPECT_CALL(*mockSocket, close()).Times(AtLeast(0));
    
    EXPECT_NO_THROW({
        ExtrapTrackDataZeroMQIncomingAdapter adapter(
            m_mockIncomingPort,
            std::move(mockSocket),
            "TestGroup"
        );
    });
}

// ==================== Thread Safety Tests ====================

TEST_F(ExtrapTrackDataZeroMQIncomingAdapterTest, Destructor_WhileRunning_StopsGracefully) {
    {
        ExtrapTrackDataZeroMQIncomingAdapter adapter(m_mockIncomingPort);
        adapter.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        // Destructor called here - should stop gracefully
    }
    
    // No crash = success
    SUCCEED();
}
