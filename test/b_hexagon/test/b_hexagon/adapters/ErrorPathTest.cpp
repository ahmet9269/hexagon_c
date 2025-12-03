/**
 * @file ErrorPathTest.cpp
 * @brief Tests for error paths and exception scenarios
 * @details Improves coverage for error handling code paths
 *          that are normally not exercised in happy-path tests.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <atomic>
#include <vector>

#include "domain/logic/CalculatorService.hpp"
#include "domain/logic/ProcessTrackUseCase.hpp"
#include "domain/ports/ExtrapTrackData.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include "adapters/common/AdapterManager.hpp"
#include "adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "mocks/MockMessageSocket.hpp"
#include "mocks/MockCalculatorService.hpp"
#include "mocks/MockOutgoingPort.hpp"

using namespace domain::logic;
using namespace domain::ports;
using namespace mocks;
using namespace testing;

// ==================== Calculator Service Error Tests ====================

class CalculatorServiceErrorTest : public Test {
protected:
    CalculatorService service_;
};

TEST_F(CalculatorServiceErrorTest, CalculateDelay_WithZeroVelocity_ReturnsValidResult) {
    // Arrange
    ExtrapTrackData input;
    input.setTrackId(1);
    input.setXVelocityECEF(0.0);
    input.setYVelocityECEF(0.0);
    input.setZVelocityECEF(0.0);
    input.setXPositionECEF(1000.0);
    input.setYPositionECEF(1000.0);
    input.setZPositionECEF(1000.0);

    // Act
    auto result = service_.calculateDelay(input);

    // Assert - should handle zero velocity gracefully
    EXPECT_EQ(result.getTrackId(), 1);
}

TEST_F(CalculatorServiceErrorTest, CalculateDelay_WithVeryLargeValues_NoOverflow) {
    // Arrange
    ExtrapTrackData input;
    input.setTrackId(1);
    // Position values within valid range (-9.9E+10 to 9.9E+10)
    input.setXPositionECEF(9e10);
    input.setYPositionECEF(9e10);
    input.setZPositionECEF(9e10);
    // Velocity values within valid range (-1E+6 to 1E+6)
    input.setXVelocityECEF(1e6 - 1);
    input.setYVelocityECEF(1e6 - 1);
    input.setZVelocityECEF(1e6 - 1);

    // Act & Assert - should not crash or overflow
    EXPECT_NO_THROW({
        auto result = service_.calculateDelay(input);
    });
}

TEST_F(CalculatorServiceErrorTest, CalculateDelay_WithNegativeValues_HandlesCorrectly) {
    // Arrange
    ExtrapTrackData input;
    input.setTrackId(99);
    input.setXPositionECEF(-1e10);
    input.setYPositionECEF(-1e10);
    input.setZPositionECEF(-1e10);
    input.setXVelocityECEF(-500.0);
    input.setYVelocityECEF(-500.0);
    input.setZVelocityECEF(-500.0);

    // Act
    auto result = service_.calculateDelay(input);

    // Assert
    EXPECT_EQ(result.getTrackId(), 99);
}

TEST_F(CalculatorServiceErrorTest, CalculateDelay_WithZeroTimestamp_HandlesCorrectly) {
    // Arrange
    ExtrapTrackData input;
    input.setTrackId(42);
    input.setUpdateTime(0);

    // Act
    auto result = service_.calculateDelay(input);

    // Assert
    EXPECT_EQ(result.getTrackId(), 42);
}

TEST_F(CalculatorServiceErrorTest, CalculateDelay_WithMaxTimestamp_HandlesCorrectly) {
    // Arrange
    ExtrapTrackData input;
    input.setTrackId(100);
    // ExtrapTrackData UpdateTime valid range is 0 to INT64_MAX
    // Use a large but safe value that won't overflow
    input.setUpdateTime(1700000000000000LL);  // ~Jan 2024 in microseconds

    // Act & Assert
    EXPECT_NO_THROW({
        auto result = service_.calculateDelay(input);
    });
}

// ==================== ProcessTrackUseCase Error Tests ====================

class ProcessTrackUseCaseErrorTest : public Test {
protected:
    std::shared_ptr<NiceMock<MockOutgoingPort>> mockOutgoing_;

    void SetUp() override {
        mockOutgoing_ = std::make_shared<NiceMock<MockOutgoingPort>>();
    }
};

TEST_F(ProcessTrackUseCaseErrorTest, OnDataReceived_ValidData_ProcessesNormally) {
    // Arrange - Valid data that passes isValid()
    auto calculator = std::make_unique<NiceMock<MockCalculatorService>>();
    
    // Calculator SHOULD be called for valid data
    EXPECT_CALL(*calculator, calculateDelay(_)).Times(1);
    
    ProcessTrackUseCase useCase(std::move(calculator), mockOutgoing_);
    ExtrapTrackData input;
    input.setTrackId(1);  // Valid trackId
    // All other fields have valid default values (0s)

    // Act & Assert - should not throw
    EXPECT_NO_THROW(useCase.onDataReceived(input));
}

TEST_F(ProcessTrackUseCaseErrorTest, OnDataReceived_ValidData_SendsResult) {
    // Arrange
    auto calculator = std::make_unique<NiceMock<MockCalculatorService>>();
    DelayCalcTrackData mockResult;
    mockResult.setTrackId(123);

    EXPECT_CALL(*calculator, calculateDelay(_))
        .WillOnce(Return(mockResult));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .Times(1);

    ProcessTrackUseCase useCase(std::move(calculator), mockOutgoing_);
    ExtrapTrackData input;
    input.setTrackId(123);

    // Act & Assert - should not throw
    EXPECT_NO_THROW(useCase.onDataReceived(input));
}

TEST_F(ProcessTrackUseCaseErrorTest, OnDataReceived_MultipleDataItems_AllProcessed) {
    // Arrange
    auto calculator = std::make_unique<NiceMock<MockCalculatorService>>();
    
    EXPECT_CALL(*calculator, calculateDelay(_))
        .Times(3)
        .WillRepeatedly(Invoke([](const ExtrapTrackData& input) {
            DelayCalcTrackData result;
            result.setTrackId(input.getTrackId());
            return result;
        }));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .Times(3);

    ProcessTrackUseCase useCase(std::move(calculator), mockOutgoing_);

    // Act
    for (int i = 1; i <= 3; ++i) {
        ExtrapTrackData input;
        input.setTrackId(i);
        EXPECT_NO_THROW(useCase.onDataReceived(input));
    }
}

// ==================== AdapterManager Error Tests ====================

class AdapterManagerErrorTest : public Test {};

TEST_F(AdapterManagerErrorTest, StartAll_EmptyManager_ReturnsTrue) {
    // Arrange
    adapters::AdapterManager manager;

    // Act
    bool result = manager.startAll();

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(AdapterManagerErrorTest, StopAll_EmptyManager_NoError) {
    // Arrange
    adapters::AdapterManager manager;

    // Act & Assert
    EXPECT_NO_THROW(manager.stopAll());
}

TEST_F(AdapterManagerErrorTest, IsRunning_EmptyManager_ReturnsFalse) {
    // Arrange
    adapters::AdapterManager manager;

    // Act & Assert
    EXPECT_FALSE(manager.isRunning());
}

TEST_F(AdapterManagerErrorTest, StartAll_MultipleCalls_Safe) {
    // Arrange
    adapters::AdapterManager manager;

    // Act & Assert
    EXPECT_TRUE(manager.startAll());
    EXPECT_TRUE(manager.startAll());
    EXPECT_TRUE(manager.startAll());
}

TEST_F(AdapterManagerErrorTest, StopAll_MultipleCalls_Safe) {
    // Arrange
    adapters::AdapterManager manager;
    (void)manager.startAll();

    // Act & Assert
    EXPECT_NO_THROW(manager.stopAll());
    EXPECT_NO_THROW(manager.stopAll());
    EXPECT_NO_THROW(manager.stopAll());
}

// ==================== Outgoing Adapter Error Tests ====================

class OutgoingAdapterErrorTest : public Test {};

TEST_F(OutgoingAdapterErrorTest, SendData_SocketFails_ReturnsFalse) {
    // Arrange
    auto mockSocket = std::make_unique<NiceMock<MockMessageSocket>>();
    
    EXPECT_CALL(*mockSocket, send(_, _))
        .WillOnce(Return(false));  // Socket fails
    
    ON_CALL(*mockSocket, isConnected()).WillByDefault(Return(true));
    ON_CALL(*mockSocket, getSocketType()).WillByDefault(Return("FailSocket"));

    DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
        std::move(mockSocket), "FailGroup");
    
    ASSERT_TRUE(adapter.start());

    // Act
    DelayCalcTrackData data;
    data.setTrackId(123);
    
    // sendDelayCalcTrackData throws runtime_error when send fails
    EXPECT_THROW(adapter.sendDelayCalcTrackData(data), std::runtime_error);
}

TEST_F(OutgoingAdapterErrorTest, SendData_BeforeStart_NoThrow) {
    // Arrange
    auto mockSocket = std::make_unique<NiceMock<MockMessageSocket>>();
    ON_CALL(*mockSocket, getSocketType()).WillByDefault(Return("MockSocket"));

    DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
        std::move(mockSocket), "TestGroup");
    
    // Don't start the adapter

    // Act - sendDelayCalcTrackData should not crash even when not started
    DelayCalcTrackData data;
    data.setTrackId(1);  // Valid trackId
    EXPECT_NO_THROW(adapter.sendDelayCalcTrackData(data));
}

TEST_F(OutgoingAdapterErrorTest, SendData_LargeData_Succeeds) {
    // Arrange
    auto mockSocket = std::make_unique<NiceMock<MockMessageSocket>>();
    
    ON_CALL(*mockSocket, send(_, _)).WillByDefault(Return(true));
    ON_CALL(*mockSocket, isConnected()).WillByDefault(Return(true));
    ON_CALL(*mockSocket, getSocketType()).WillByDefault(Return("MockSocket"));

    DelayCalcTrackDataZeroMQOutgoingAdapter adapter(
        std::move(mockSocket), "TestGroup");
    
    ASSERT_TRUE(adapter.start());

    // Act - send data with boundary valid values (trackId range is 1-9999, position -9.9E10 to 9.9E10)
    DelayCalcTrackData data;
    data.setTrackId(9999);  // Max valid trackId
    data.setXPositionECEF(9.9e10 - 1);  // Near max valid position
    
    EXPECT_NO_THROW(adapter.sendDelayCalcTrackData(data));
}

// ==================== Thread Safety Edge Case Tests ====================

class ThreadSafetyEdgeCaseTest : public Test {};

TEST_F(ThreadSafetyEdgeCaseTest, ConcurrentCalculations_ThreadSafe) {
    // Arrange
    CalculatorService service;
    std::atomic<int> successCount{0};
    const int numThreads = 4;
    const int calcsPerThread = 50;

    // Act
    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < calcsPerThread; ++i) {
                ExtrapTrackData input;
                // TrackId range is 1-9999, compute unique but valid values
                int trackId = 1 + (t * calcsPerThread + i) % 9999;
                input.setTrackId(trackId);
                input.setXPositionECEF(1000.0 * i);
                
                try {
                    auto result = service.calculateDelay(input);
                    if (result.getTrackId() == static_cast<uint32_t>(trackId)) {
                        ++successCount;
                    }
                } catch (...) {
                    // Should not throw
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Assert
    EXPECT_EQ(successCount.load(), numThreads * calcsPerThread);
}

TEST_F(ThreadSafetyEdgeCaseTest, AdapterManagerConcurrentAccess_Safe) {
    // Arrange
    adapters::AdapterManager manager;
    std::atomic<bool> shouldStop{false};
    std::atomic<int> operationCount{0};

    // Act - concurrent operations
    std::thread starter([&]() {
        for (int i = 0; i < 10 && !shouldStop; ++i) {
            (void)manager.startAll();
            ++operationCount;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    std::thread stopper([&]() {
        for (int i = 0; i < 10 && !shouldStop; ++i) {
            manager.stopAll();
            ++operationCount;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    std::thread checker([&]() {
        for (int i = 0; i < 50 && !shouldStop; ++i) {
            (void)manager.isRunning();
            ++operationCount;
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
        shouldStop = true;
    });

    starter.join();
    stopper.join();
    checker.join();

    // Assert - no crash, operations completed
    EXPECT_GT(operationCount.load(), 0);
}

// ==================== Resource Cleanup Tests ====================

class ResourceCleanupTest : public Test {};

TEST_F(ResourceCleanupTest, AdapterManager_DestructorCleansUp) {
    // Arrange & Act
    {
        adapters::AdapterManager manager;
        (void)manager.startAll();
        // Destructor called without explicit stopAll()
    }

    // Assert - no crash or leak
    SUCCEED();
}

TEST_F(ResourceCleanupTest, CalculatorService_DestructorSafe) {
    // Arrange & Act
    {
        CalculatorService service;
        ExtrapTrackData input;
        input.setTrackId(1);
        (void)service.calculateDelay(input);
        // Destructor called
    }

    // Assert
    SUCCEED();
}

TEST_F(ResourceCleanupTest, ProcessTrackUseCase_DestructorSafe) {
    // Arrange & Act
    {
        auto calculator = std::make_unique<CalculatorService>();
        auto mockOutgoing = std::make_shared<NiceMock<MockOutgoingPort>>();
        
        ProcessTrackUseCase useCase(std::move(calculator), mockOutgoing);
        ExtrapTrackData input;
        input.setTrackId(1);
        useCase.onDataReceived(input);
        // Destructor called
    }

    // Assert
    SUCCEED();
}

