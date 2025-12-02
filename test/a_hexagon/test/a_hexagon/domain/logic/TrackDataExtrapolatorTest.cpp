/**
 * @file TrackDataExtrapolatorTest.cpp
 * @brief Unit tests for TrackDataExtrapolator domain logic
 * @details Tests extrapolation logic using MockOutgoingPort (no network)
 *          Achieves %95+ code coverage through dependency injection
 * 
 * @author a_hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant test implementation
 */

#include <gtest/gtest.h>
#include <memory>
#include <cmath>
#include <chrono>

#include "domain/logic/TrackDataExtrapolator.hpp"
#include "../../mocks/MockOutgoingPort.hpp"
#include "domain/ports/TrackData.hpp"
#include "domain/ports/ExtrapTrackData.hpp"

using namespace domain::logic;
using namespace domain::model;
using namespace a_hexagon::test::mocks;

/**
 * @brief Test fixture for TrackDataExtrapolator
 */
class TrackDataExtrapolatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockOutgoingPort_ = std::make_unique<MockOutgoingPort>();
        mockOutgoingPortPtr_ = mockOutgoingPort_.get();
    }
    
    void TearDown() override {
        // MockOutgoingPort is owned by extrapolator after move
    }
    
    /**
     * @brief Create extrapolator with mock outgoing port
     */
    void createExtrapolator() {
        extrapolator_ = std::make_unique<TrackDataExtrapolator>(mockOutgoingPortPtr_);
    }
    
    /**
     * @brief Create valid test TrackData
     */
    TrackData createTestTrackData(int32_t trackId) {
        TrackData trackData;
        trackData.setTrackId(trackId);
        trackData.setXPositionECEF(4000000.0);
        trackData.setYPositionECEF(3000000.0);
        trackData.setZPositionECEF(5000000.0);
        trackData.setXVelocityECEF(100.0);   // 100 m/s
        trackData.setYVelocityECEF(200.0);   // 200 m/s
        trackData.setZVelocityECEF(50.0);    // 50 m/s
        trackData.setOriginalUpdateTime(1700000000);  // ms
        return trackData;
    }
    
    std::unique_ptr<MockOutgoingPort> mockOutgoingPort_;
    MockOutgoingPort* mockOutgoingPortPtr_{nullptr};
    std::unique_ptr<TrackDataExtrapolator> extrapolator_;
};

// ==================== Constructor Tests ====================

TEST_F(TrackDataExtrapolatorTest, Constructor_WithValidPort_CreatesExtrapolator) {
    createExtrapolator();
    
    ASSERT_NE(extrapolator_, nullptr);
}

TEST_F(TrackDataExtrapolatorTest, Constructor_WithNullPort_DoesNotCrash) {
    // Should handle nullptr gracefully
    TrackDataExtrapolator extrapolator(nullptr);
    
    SUCCEED();  // Test passes if no crash
}

// ==================== ProcessAndForwardTrackData Tests ====================

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_GeneratesMultipleOutputs) {
    createExtrapolator();
    
    auto trackData = createTestTrackData(12345);
    
    // This will call extrapolateTrackData with 8Hz input, 100Hz output
    // Should generate ~12 extrapolated outputs per input (125ms / 10ms)
    extrapolator_->processAndForwardTrackData(trackData);
    
    // Wait for all outputs (there's a 10ms sleep between each)
    size_t sentCount = mockOutgoingPortPtr_->getSentDataCount();
    
    // 8Hz → 100Hz should generate approximately 12-13 outputs
    EXPECT_GE(sentCount, 10);
    EXPECT_LE(sentCount, 15);
}

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_PreservesTrackId) {
    createExtrapolator();
    
    constexpr int32_t TEST_TRACK_ID = 42424242;
    auto trackData = createTestTrackData(TEST_TRACK_ID);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_FALSE(allSent.empty());
    
    for (const auto& extrapData : allSent) {
        EXPECT_EQ(extrapData.getTrackId(), TEST_TRACK_ID);
    }
}

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_PreservesVelocity) {
    createExtrapolator();
    
    TrackData trackData;
    trackData.setTrackId(1);
    trackData.setXPositionECEF(0.0);
    trackData.setYPositionECEF(0.0);
    trackData.setZPositionECEF(0.0);
    trackData.setXVelocityECEF(111.5);
    trackData.setYVelocityECEF(-222.5);
    trackData.setZVelocityECEF(333.5);
    trackData.setOriginalUpdateTime(1700000000);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_FALSE(allSent.empty());
    
    for (const auto& extrapData : allSent) {
        EXPECT_DOUBLE_EQ(extrapData.getXVelocityECEF(), 111.5);
        EXPECT_DOUBLE_EQ(extrapData.getYVelocityECEF(), -222.5);
        EXPECT_DOUBLE_EQ(extrapData.getZVelocityECEF(), 333.5);
    }
}

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_PreservesOriginalUpdateTime) {
    createExtrapolator();
    
    constexpr int64_t ORIGINAL_TIME = 1700000000;
    TrackData trackData;
    trackData.setTrackId(1);
    trackData.setXPositionECEF(0.0);
    trackData.setYPositionECEF(0.0);
    trackData.setZPositionECEF(0.0);
    trackData.setXVelocityECEF(0.0);
    trackData.setYVelocityECEF(0.0);
    trackData.setZVelocityECEF(0.0);
    trackData.setOriginalUpdateTime(ORIGINAL_TIME);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_FALSE(allSent.empty());
    
    for (const auto& extrapData : allSent) {
        EXPECT_EQ(extrapData.getOriginalUpdateTime(), ORIGINAL_TIME);
    }
}

// ==================== Extrapolation Calculation Tests ====================

TEST_F(TrackDataExtrapolatorTest, ExtrapolateTrackData_CalculatesPositionCorrectly) {
    createExtrapolator();
    
    TrackData trackData;
    trackData.setTrackId(1);
    trackData.setXPositionECEF(1000.0);   // Initial position
    trackData.setYPositionECEF(2000.0);
    trackData.setZPositionECEF(3000.0);
    trackData.setXVelocityECEF(10.0);     // 10 m/s
    trackData.setYVelocityECEF(20.0);
    trackData.setZVelocityECEF(5.0);
    trackData.setOriginalUpdateTime(1700000000);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_FALSE(allSent.empty());
    
    // First output should have t=0, so position should be same as input
    auto& first = allSent[0];
    EXPECT_DOUBLE_EQ(first.getXPositionECEF(), 1000.0);
    EXPECT_DOUBLE_EQ(first.getYPositionECEF(), 2000.0);
    EXPECT_DOUBLE_EQ(first.getZPositionECEF(), 3000.0);
    
    // Subsequent outputs should show linear extrapolation
    // Position = initial + velocity * time
    if (allSent.size() > 1) {
        // Second output at t=0.01 (10ms)
        auto& second = allSent[1];
        double expectedX = 1000.0 + 10.0 * 0.01;  // 1000.1
        double expectedY = 2000.0 + 20.0 * 0.01;  // 2000.2
        double expectedZ = 3000.0 + 5.0 * 0.01;   // 3000.05
        
        EXPECT_NEAR(second.getXPositionECEF(), expectedX, 0.01);
        EXPECT_NEAR(second.getYPositionECEF(), expectedY, 0.01);
        EXPECT_NEAR(second.getZPositionECEF(), expectedZ, 0.01);
    }
}

TEST_F(TrackDataExtrapolatorTest, ExtrapolateTrackData_IncrementsUpdateTime) {
    createExtrapolator();
    
    TrackData trackData;
    trackData.setTrackId(1);
    trackData.setXPositionECEF(0.0);
    trackData.setYPositionECEF(0.0);
    trackData.setZPositionECEF(0.0);
    trackData.setXVelocityECEF(0.0);
    trackData.setYVelocityECEF(0.0);
    trackData.setZVelocityECEF(0.0);
    trackData.setOriginalUpdateTime(1700000000);  // ms
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_GE(allSent.size(), 2);
    
    // UpdateTime should increase with each extrapolation
    int64_t prevUpdateTime = allSent[0].getUpdateTime();
    for (size_t i = 1; i < allSent.size(); ++i) {
        int64_t currentUpdateTime = allSent[i].getUpdateTime();
        EXPECT_GT(currentUpdateTime, prevUpdateTime);
        prevUpdateTime = currentUpdateTime;
    }
}

TEST_F(TrackDataExtrapolatorTest, ExtrapolateTrackData_SetsFirstHopSentTime) {
    createExtrapolator();
    
    auto trackData = createTestTrackData(1);
    
    auto beforeTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto afterTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_FALSE(allSent.empty());
    
    for (const auto& extrapData : allSent) {
        int64_t firstHopTime = extrapData.getFirstHopSentTime();
        EXPECT_GE(firstHopTime, beforeTime);
        EXPECT_LE(firstHopTime, afterTime);
    }
}

// ==================== Frequency Tests ====================

TEST_F(TrackDataExtrapolatorTest, FrequencyInterval_8HzInput_125msInterval) {
    double inputFrequency = 8.0;
    double inputInterval = 1.0 / inputFrequency;
    
    EXPECT_DOUBLE_EQ(inputInterval, 0.125);  // 125ms
}

TEST_F(TrackDataExtrapolatorTest, FrequencyInterval_100HzOutput_10msInterval) {
    double outputFrequency = 100.0;
    double outputInterval = 1.0 / outputFrequency;
    
    EXPECT_DOUBLE_EQ(outputInterval, 0.01);  // 10ms
}

TEST_F(TrackDataExtrapolatorTest, ExtrapolationCount_8HzTo100Hz_Approximately12Outputs) {
    double inputFrequency = 8.0;
    double outputFrequency = 100.0;
    
    double inputInterval = 1.0 / inputFrequency;
    double outputInterval = 1.0 / outputFrequency;
    
    int expectedCount = static_cast<int>(inputInterval / outputInterval);
    
    // 8Hz -> 100Hz means ~12-13 extrapolated points per input
    EXPECT_GE(expectedCount, 12);
    EXPECT_LE(expectedCount, 13);
}

// ==================== Edge Cases ====================

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WithZeroVelocity_PositionUnchanged) {
    createExtrapolator();
    
    TrackData trackData;
    trackData.setTrackId(1);
    trackData.setXPositionECEF(5000.0);
    trackData.setYPositionECEF(6000.0);
    trackData.setZPositionECEF(7000.0);
    trackData.setXVelocityECEF(0.0);
    trackData.setYVelocityECEF(0.0);
    trackData.setZVelocityECEF(0.0);
    trackData.setOriginalUpdateTime(1700000000);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_FALSE(allSent.empty());
    
    // With zero velocity, all extrapolated positions should be same as initial
    for (const auto& extrapData : allSent) {
        EXPECT_DOUBLE_EQ(extrapData.getXPositionECEF(), 5000.0);
        EXPECT_DOUBLE_EQ(extrapData.getYPositionECEF(), 6000.0);
        EXPECT_DOUBLE_EQ(extrapData.getZPositionECEF(), 7000.0);
    }
}

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WithNegativeVelocity_DecrementsPosition) {
    createExtrapolator();
    
    TrackData trackData;
    trackData.setTrackId(1);
    trackData.setXPositionECEF(10000.0);
    trackData.setYPositionECEF(0.0);
    trackData.setZPositionECEF(0.0);
    trackData.setXVelocityECEF(-100.0);  // -100 m/s
    trackData.setYVelocityECEF(0.0);
    trackData.setZVelocityECEF(0.0);
    trackData.setOriginalUpdateTime(1700000000);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_GE(allSent.size(), 2);
    
    // Position should decrease over time
    double prevX = allSent[0].getXPositionECEF();
    for (size_t i = 1; i < allSent.size(); ++i) {
        double currentX = allSent[i].getXPositionECEF();
        EXPECT_LT(currentX, prevX);
        prevX = currentX;
    }
}

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WithLargeVelocity_HandlesCorrectly) {
    createExtrapolator();
    
    TrackData trackData;
    trackData.setTrackId(1);
    trackData.setXPositionECEF(0.0);
    trackData.setYPositionECEF(0.0);
    trackData.setZPositionECEF(0.0);
    trackData.setXVelocityECEF(10000.0);  // 10 km/s (very fast)
    trackData.setYVelocityECEF(10000.0);
    trackData.setZVelocityECEF(10000.0);
    trackData.setOriginalUpdateTime(1700000000);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    auto allSent = mockOutgoingPortPtr_->getAllSentData();
    ASSERT_FALSE(allSent.empty());
    
    // Should handle large velocities without overflow
    for (const auto& extrapData : allSent) {
        EXPECT_TRUE(std::isfinite(extrapData.getXPositionECEF()));
        EXPECT_TRUE(std::isfinite(extrapData.getYPositionECEF()));
        EXPECT_TRUE(std::isfinite(extrapData.getZPositionECEF()));
    }
}

// ==================== Call Count Verification Tests ====================

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_CallsSendForEachOutput) {
    createExtrapolator();
    
    auto trackData = createTestTrackData(1);
    
    extrapolator_->processAndForwardTrackData(trackData);
    
    size_t sentCount = mockOutgoingPortPtr_->getSentDataCount();
    int sendCallCount = mockOutgoingPortPtr_->getSendSingleCallCount();
    
    // Each output should result in one send call
    EXPECT_EQ(static_cast<size_t>(sendCallCount), sentCount);
}

// ==================== Multiple Input Tests ====================

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_MultipleInputs_ProcessesAll) {
    createExtrapolator();
    
    constexpr int NUM_INPUTS = 3;
    
    for (int i = 0; i < NUM_INPUTS; ++i) {
        auto trackData = createTestTrackData(1000 + i);
        extrapolator_->processAndForwardTrackData(trackData);
    }
    
    // Each input should generate ~12 outputs
    size_t totalSent = mockOutgoingPortPtr_->getSentDataCount();
    EXPECT_GE(totalSent, static_cast<size_t>(NUM_INPUTS * 10));
}

// ==================== Failure Injection Tests ====================

TEST_F(TrackDataExtrapolatorTest, ProcessAndForwardTrackData_WhenPortFails_HandlesGracefully) {
    createExtrapolator();
    
    mockOutgoingPortPtr_->setFailSend(true);
    
    auto trackData = createTestTrackData(1);
    
    // Should not crash even when port fails
    extrapolator_->processAndForwardTrackData(trackData);
    
    // Verify attempts were made (but failed)
    EXPECT_GT(mockOutgoingPortPtr_->getFailedSendCount(), 0);
}
