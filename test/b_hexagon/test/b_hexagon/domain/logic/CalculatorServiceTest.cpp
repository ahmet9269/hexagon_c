/**
 * @file CalculatorServiceTest.cpp
 * @brief Unit tests for CalculatorService domain service
 * @details Tests delay calculation logic in isolation
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <cmath>

#include "domain/logic/CalculatorService.hpp"
#include "domain/ports/ExtrapTrackData.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"

using namespace domain::logic;
using namespace domain::ports;

/**
 * @brief Test fixture for CalculatorService
 */
class CalculatorServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        calculator_ = std::make_unique<CalculatorService>();
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
        data.setFirstHopSentTime(getCurrentTimeMicros() - 1000);  // 1ms ago
        return data;
    }
    
    /**
     * @brief Get current time in microseconds
     */
    long getCurrentTimeMicros() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()
        ).count();
    }
    
    std::unique_ptr<CalculatorService> calculator_;
};

// ==================== Construction Tests ====================

TEST_F(CalculatorServiceTest, DefaultConstructor_CreatesValidService) {
    CalculatorService calc;
    SUCCEED();  // Construction should not throw
}

// ==================== calculateDelay Tests ====================

TEST_F(CalculatorServiceTest, CalculateDelay_ValidInput_ReturnsResult) {
    auto testData = createTestExtrapTrackData(1234);
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    EXPECT_EQ(result.getTrackId(), 1234);
}

TEST_F(CalculatorServiceTest, CalculateDelay_PreservesTrackId) {
    auto testData = createTestExtrapTrackData(9999);
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    EXPECT_EQ(result.getTrackId(), 9999);
}

TEST_F(CalculatorServiceTest, CalculateDelay_PreservesPositionData) {
    auto testData = createTestExtrapTrackData(1234);
    testData.setXPositionECEF(1111111.0);
    testData.setYPositionECEF(2222222.0);
    testData.setZPositionECEF(3333333.0);
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    EXPECT_DOUBLE_EQ(result.getXPositionECEF(), 1111111.0);
    EXPECT_DOUBLE_EQ(result.getYPositionECEF(), 2222222.0);
    EXPECT_DOUBLE_EQ(result.getZPositionECEF(), 3333333.0);
}

TEST_F(CalculatorServiceTest, CalculateDelay_PreservesVelocityData) {
    auto testData = createTestExtrapTrackData(1234);
    testData.setXVelocityECEF(111.0);
    testData.setYVelocityECEF(222.0);
    testData.setZVelocityECEF(333.0);
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    EXPECT_DOUBLE_EQ(result.getXVelocityECEF(), 111.0);
    EXPECT_DOUBLE_EQ(result.getYVelocityECEF(), 222.0);
    EXPECT_DOUBLE_EQ(result.getZVelocityECEF(), 333.0);
}

TEST_F(CalculatorServiceTest, CalculateDelay_PreservesTimingData) {
    auto testData = createTestExtrapTrackData(1234);
    testData.setUpdateTime(9999999999L);
    testData.setOriginalUpdateTime(8888888888L);
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    EXPECT_EQ(result.getUpdateTime(), 9999999999L);
    EXPECT_EQ(result.getOriginalUpdateTime(), 8888888888L);
}

TEST_F(CalculatorServiceTest, CalculateDelay_CalculatesPositiveDelay) {
    long sentTime = getCurrentTimeMicros() - 5000;  // 5ms ago
    
    auto testData = createTestExtrapTrackData(1234);
    testData.setFirstHopSentTime(sentTime);
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    // Delay should be approximately 5000 microseconds
    EXPECT_GT(result.getFirstHopDelayTime(), 0);
    EXPECT_GE(result.getFirstHopDelayTime(), 4000);  // At least 4ms
    EXPECT_LE(result.getFirstHopDelayTime(), 10000); // Less than 10ms
}

TEST_F(CalculatorServiceTest, CalculateDelay_SetsSecondHopSentTime) {
    auto testData = createTestExtrapTrackData(1234);
    
    long beforeCall = getCurrentTimeMicros();
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    long afterCall = getCurrentTimeMicros();
    
    // SecondHopSentTime should be between beforeCall and afterCall
    EXPECT_GE(result.getSecondHopSentTime(), beforeCall);
    EXPECT_LE(result.getSecondHopSentTime(), afterCall);
}

TEST_F(CalculatorServiceTest, CalculateDelay_ZeroFirstHopSentTime_ReturnsZeroDelay) {
    auto testData = createTestExtrapTrackData(1234);
    testData.setFirstHopSentTime(0);
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    // With invalid firstHopSentTime, delay should be 0
    EXPECT_EQ(result.getFirstHopDelayTime(), 0);
}

TEST_F(CalculatorServiceTest, CalculateDelay_NegativeFirstHopSentTime_ThrowsException) {
    auto testData = createTestExtrapTrackData(1234);
    
    EXPECT_THROW(testData.setFirstHopSentTime(-1), std::out_of_range);
}

// ==================== Edge Cases ====================

TEST_F(CalculatorServiceTest, CalculateDelay_MultipleCalls_Independent) {
    auto testData1 = createTestExtrapTrackData(111);
    auto testData2 = createTestExtrapTrackData(222);
    
    DelayCalcTrackData result1 = calculator_->calculateDelay(testData1);
    DelayCalcTrackData result2 = calculator_->calculateDelay(testData2);
    
    EXPECT_EQ(result1.getTrackId(), 111);
    EXPECT_EQ(result2.getTrackId(), 222);
}

TEST_F(CalculatorServiceTest, CalculateDelay_LargeTrackId_Preserved) {
    auto testData = createTestExtrapTrackData(9999);  // Max valid trackId
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    EXPECT_EQ(result.getTrackId(), 9999);
}

TEST_F(CalculatorServiceTest, CalculateDelay_ExtremePositionValues_Preserved) {
    auto testData = createTestExtrapTrackData(1234);
    testData.setXPositionECEF(9.9e10);  // Near max valid
    testData.setYPositionECEF(-9.9e10); // Near min valid
    testData.setZPositionECEF(1e-12);   // Very small positive
    
    DelayCalcTrackData result = calculator_->calculateDelay(testData);
    
    EXPECT_DOUBLE_EQ(result.getXPositionECEF(), 9.9e10);
    EXPECT_DOUBLE_EQ(result.getYPositionECEF(), -9.9e10);
    EXPECT_DOUBLE_EQ(result.getZPositionECEF(), 1e-12);
}
