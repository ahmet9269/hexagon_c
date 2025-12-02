/**
 * @file ProcessTrackUseCaseTest.cpp
 * @brief Unit tests for ProcessTrackUseCase domain logic
 * @details Tests track processing logic using MockCalculatorService and MockOutgoingPort
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

#include "domain/logic/ProcessTrackUseCase.hpp"
#include "domain/logic/ICalculatorService.hpp"
#include "mocks/MockCalculatorService.hpp"
#include "mocks/MockOutgoingPort.hpp"
#include "domain/ports/ExtrapTrackData.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"

using namespace domain::logic;
using namespace domain::ports;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;

/**
 * @brief Test fixture for ProcessTrackUseCase
 */
class ProcessTrackUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockCalculator_ = std::make_unique<mocks::MockCalculatorService>();
        mockCalculatorPtr_ = mockCalculator_.get();
        
        mockOutgoing_ = std::make_shared<mocks::MockOutgoingPort>();
    }
    
    void TearDown() override {
        // Cleanup handled by smart pointers
    }
    
    /**
     * @brief Create use case with mock dependencies
     */
    void createUseCase() {
        useCase_ = std::make_unique<ProcessTrackUseCase>(
            std::move(mockCalculator_),
            mockOutgoing_
        );
    }
    
    /**
     * @brief Create valid test ExtrapTrackData
     */
    ExtrapTrackData createTestExtrapTrackData(int32_t trackId) {
        ExtrapTrackData data;
        data.setTrackId(trackId);
        data.setXPositionECEF(4000000.0);
        data.setYPositionECEF(3000000.0);
        data.setZPositionECEF(5000000.0);
        data.setXVelocityECEF(100.0);
        data.setYVelocityECEF(200.0);
        data.setZVelocityECEF(50.0);
        data.setUpdateTime(1700000000000L);
        data.setOriginalUpdateTime(1700000000000L);
        data.setFirstHopSentTime(1700000000100L);
        return data;
    }

    /**
     * @brief Create expected DelayCalcTrackData for mock return
     */
    DelayCalcTrackData createExpectedDelayCalcData(int32_t trackId, int64_t delay) {
        DelayCalcTrackData data;
        data.setTrackId(trackId);
        data.setXPositionECEF(4000000.0);
        data.setYPositionECEF(3000000.0);
        data.setZPositionECEF(5000000.0);
        data.setFirstHopDelayTime(delay);
        return data;
    }
    
    std::unique_ptr<mocks::MockCalculatorService> mockCalculator_;
    mocks::MockCalculatorService* mockCalculatorPtr_{nullptr};
    std::shared_ptr<mocks::MockOutgoingPort> mockOutgoing_;
    std::unique_ptr<ProcessTrackUseCase> useCase_;
};

// ==================== Constructor Tests ====================

TEST_F(ProcessTrackUseCaseTest, Constructor_WithValidDependencies_CreatesUseCase) {
    createUseCase();
    
    ASSERT_NE(useCase_, nullptr);
}

// ==================== onDataReceived Tests ====================

TEST_F(ProcessTrackUseCaseTest, OnDataReceived_ValidData_CallsCalculator) {
    auto expectedResult = createExpectedDelayCalcData(1234, 100);
    
    EXPECT_CALL(*mockCalculatorPtr_, calculateDelay(_))
        .Times(1)
        .WillOnce(Return(expectedResult));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .Times(1);
    
    createUseCase();
    
    auto testData = createTestExtrapTrackData(1234);
    useCase_->onDataReceived(testData);
}

TEST_F(ProcessTrackUseCaseTest, OnDataReceived_ValidData_ForwardsToOutgoingPort) {
    auto expectedResult = createExpectedDelayCalcData(1234, 42);
    
    EXPECT_CALL(*mockCalculatorPtr_, calculateDelay(_))
        .WillOnce(Return(expectedResult));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .Times(1);
    
    createUseCase();
    
    auto testData = createTestExtrapTrackData(1234);
    useCase_->onDataReceived(testData);
}

TEST_F(ProcessTrackUseCaseTest, OnDataReceived_ValidData_PreservesTrackId) {
    DelayCalcTrackData capturedData;
    auto expectedResult = createExpectedDelayCalcData(9999, 100);
    
    EXPECT_CALL(*mockCalculatorPtr_, calculateDelay(_))
        .WillOnce(Return(expectedResult));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .WillOnce(Invoke([&capturedData](const DelayCalcTrackData& data) {
            capturedData = data;
        }));
    
    createUseCase();
    
    auto testData = createTestExtrapTrackData(9999);
    useCase_->onDataReceived(testData);
    
    EXPECT_EQ(capturedData.getTrackId(), 9999);
}

TEST_F(ProcessTrackUseCaseTest, OnDataReceived_MultipleCalls_ProcessesAll) {
    auto expectedResult = createExpectedDelayCalcData(1, 100);
    
    EXPECT_CALL(*mockCalculatorPtr_, calculateDelay(_))
        .Times(10)
        .WillRepeatedly(Return(expectedResult));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .Times(10);
    
    createUseCase();
    
    for (int i = 1; i <= 10; ++i) {
        auto testData = createTestExtrapTrackData(i);
        useCase_->onDataReceived(testData);
    }
}

// ==================== Data Integrity Tests ====================

TEST_F(ProcessTrackUseCaseTest, OnDataReceived_PreservesPositionData) {
    DelayCalcTrackData capturedData;
    
    DelayCalcTrackData expectedResult;
    expectedResult.setTrackId(1234);
    expectedResult.setXPositionECEF(1111111.0);
    expectedResult.setYPositionECEF(2222222.0);
    expectedResult.setZPositionECEF(3333333.0);
    
    EXPECT_CALL(*mockCalculatorPtr_, calculateDelay(_))
        .WillOnce(Return(expectedResult));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .WillOnce(Invoke([&capturedData](const DelayCalcTrackData& data) {
            capturedData = data;
        }));
    
    createUseCase();
    
    auto testData = createTestExtrapTrackData(1234);
    testData.setXPositionECEF(1111111.0);
    testData.setYPositionECEF(2222222.0);
    testData.setZPositionECEF(3333333.0);
    
    useCase_->onDataReceived(testData);
    
    EXPECT_DOUBLE_EQ(capturedData.getXPositionECEF(), 1111111.0);
    EXPECT_DOUBLE_EQ(capturedData.getYPositionECEF(), 2222222.0);
    EXPECT_DOUBLE_EQ(capturedData.getZPositionECEF(), 3333333.0);
}

TEST_F(ProcessTrackUseCaseTest, OnDataReceived_PreservesVelocityData) {
    DelayCalcTrackData capturedData;
    
    DelayCalcTrackData expectedResult;
    expectedResult.setTrackId(1234);
    expectedResult.setXVelocityECEF(111.0);
    expectedResult.setYVelocityECEF(222.0);
    expectedResult.setZVelocityECEF(333.0);
    
    EXPECT_CALL(*mockCalculatorPtr_, calculateDelay(_))
        .WillOnce(Return(expectedResult));
    
    EXPECT_CALL(*mockOutgoing_, sendDelayCalcTrackData(_))
        .WillOnce(Invoke([&capturedData](const DelayCalcTrackData& data) {
            capturedData = data;
        }));
    
    createUseCase();
    
    auto testData = createTestExtrapTrackData(1234);
    testData.setXVelocityECEF(111.0);
    testData.setYVelocityECEF(222.0);
    testData.setZVelocityECEF(333.0);
    
    useCase_->onDataReceived(testData);
    
    EXPECT_DOUBLE_EQ(capturedData.getXVelocityECEF(), 111.0);
    EXPECT_DOUBLE_EQ(capturedData.getYVelocityECEF(), 222.0);
    EXPECT_DOUBLE_EQ(capturedData.getZVelocityECEF(), 333.0);
}
