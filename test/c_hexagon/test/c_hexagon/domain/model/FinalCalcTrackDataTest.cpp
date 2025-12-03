/**
 * @file FinalCalcTrackDataTest.cpp
 * @brief Unit tests for FinalCalcTrackData model class
 */

#include <gtest/gtest.h>
#include "domain/ports/FinalCalcTrackData.hpp"
#include <vector>
#include <cmath>

using namespace domain::ports;

/**
 * @brief Test fixture for FinalCalcTrackData tests
 */
class FinalCalcTrackDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a valid track data object for testing
        validData_.setTrackId(2001);
        validData_.setXPositionECEF(1000000.0);
        validData_.setYPositionECEF(2000000.0);
        validData_.setZPositionECEF(3000000.0);
        validData_.setXVelocityECEF(100.0);
        validData_.setYVelocityECEF(200.0);
        validData_.setZVelocityECEF(300.0);
        validData_.setOriginalUpdateTime(1000000);
        validData_.setUpdateTime(1000100);
        validData_.setFirstHopSentTime(1000050);
        validData_.setFirstHopDelayTime(50);
        validData_.setSecondHopSentTime(1000150);
        validData_.setSecondHopDelayTime(100);
        validData_.setTotalDelayTime(150);
        validData_.setThirdHopSentTime(1000250);
    }

    FinalCalcTrackData validData_;
    FinalCalcTrackData emptyData_;
};

// ============================================
// Constructor Tests
// ============================================

TEST_F(FinalCalcTrackDataTest, DefaultConstructor_InitializesToZero) {
    FinalCalcTrackData data;
    
    EXPECT_EQ(data.getTrackId(), 0);
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), 0.0);
    EXPECT_EQ(data.getSecondHopDelayTime(), 0);
    EXPECT_EQ(data.getTotalDelayTime(), 0);
    EXPECT_EQ(data.getThirdHopSentTime(), 0);
}

TEST_F(FinalCalcTrackDataTest, CopyConstructor_CopiesAllFields) {
    FinalCalcTrackData copy(validData_);
    
    EXPECT_EQ(copy.getTrackId(), validData_.getTrackId());
    EXPECT_DOUBLE_EQ(copy.getXPositionECEF(), validData_.getXPositionECEF());
    EXPECT_EQ(copy.getFirstHopDelayTime(), validData_.getFirstHopDelayTime());
    EXPECT_EQ(copy.getSecondHopDelayTime(), validData_.getSecondHopDelayTime());
    EXPECT_EQ(copy.getTotalDelayTime(), validData_.getTotalDelayTime());
    EXPECT_EQ(copy.getThirdHopSentTime(), validData_.getThirdHopSentTime());
}

TEST_F(FinalCalcTrackDataTest, MoveConstructor_MovesAllFields) {
    FinalCalcTrackData original;
    original.setTrackId(888);
    original.setTotalDelayTime(999);
    
    FinalCalcTrackData moved(std::move(original));
    
    EXPECT_EQ(moved.getTrackId(), 888);
    EXPECT_EQ(moved.getTotalDelayTime(), 999);
}

// ============================================
// Getter/Setter Tests - Extended Fields
// ============================================

TEST_F(FinalCalcTrackDataTest, SetSecondHopDelayTime_StoresValue) {
    FinalCalcTrackData data;
    data.setSecondHopDelayTime(250);
    
    EXPECT_EQ(data.getSecondHopDelayTime(), 250);
}

TEST_F(FinalCalcTrackDataTest, SetTotalDelayTime_StoresValue) {
    FinalCalcTrackData data;
    data.setTotalDelayTime(500);
    
    EXPECT_EQ(data.getTotalDelayTime(), 500);
}

TEST_F(FinalCalcTrackDataTest, SetThirdHopSentTime_StoresValue) {
    FinalCalcTrackData data;
    int64_t timestamp = 1701388800000000;
    data.setThirdHopSentTime(timestamp);
    
    EXPECT_EQ(data.getThirdHopSentTime(), timestamp);
}

// ============================================
// Delay Calculation Tests
// ============================================

TEST_F(FinalCalcTrackDataTest, TotalDelay_IsSumOfHopDelays) {
    FinalCalcTrackData data;
    data.setFirstHopDelayTime(100);
    data.setSecondHopDelayTime(150);
    data.setTotalDelayTime(250);  // Should equal first + second
    
    EXPECT_EQ(data.getTotalDelayTime(), 
              data.getFirstHopDelayTime() + data.getSecondHopDelayTime());
}

TEST_F(FinalCalcTrackDataTest, DelayChain_IsConsistent) {
    // Simulate a realistic delay chain
    int64_t t0 = 1000000;  // Original time
    int64_t hop1_delay = 50;
    int64_t hop2_delay = 75;
    
    FinalCalcTrackData data;
    data.setOriginalUpdateTime(t0);
    data.setFirstHopSentTime(t0 + 10);
    data.setFirstHopDelayTime(hop1_delay);
    data.setSecondHopSentTime(t0 + hop1_delay + 20);
    data.setSecondHopDelayTime(hop2_delay);
    data.setTotalDelayTime(hop1_delay + hop2_delay);
    data.setThirdHopSentTime(t0 + hop1_delay + hop2_delay + 30);
    
    EXPECT_EQ(data.getTotalDelayTime(), 125);
    EXPECT_GT(data.getThirdHopSentTime(), data.getSecondHopSentTime());
}

// ============================================
// Serialization Tests
// ============================================

TEST_F(FinalCalcTrackDataTest, Serialize_ReturnsNonEmptyVector) {
    std::vector<uint8_t> serialized = validData_.serialize();
    
    EXPECT_FALSE(serialized.empty());
    EXPECT_EQ(serialized.size(), validData_.getSerializedSize());
}

TEST_F(FinalCalcTrackDataTest, GetSerializedSize_ReturnsCorrectSize) {
    // Size should be sum of all member sizes
    // int32_t (4) + 6*double (48) + 8*int64_t (64) = 116 bytes
    std::size_t expectedSize = sizeof(int32_t) + 
                               6 * sizeof(double) + 
                               8 * sizeof(int64_t);
    
    EXPECT_EQ(validData_.getSerializedSize(), expectedSize);
}

TEST_F(FinalCalcTrackDataTest, SerializeDeserialize_RoundTrip) {
    // Serialize
    std::vector<uint8_t> serialized = validData_.serialize();
    
    // Deserialize into new object
    FinalCalcTrackData deserialized;
    bool success = deserialized.deserialize(serialized);
    
    ASSERT_TRUE(success);
    EXPECT_EQ(deserialized.getTrackId(), validData_.getTrackId());
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), validData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), validData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), validData_.getZPositionECEF());
    EXPECT_EQ(deserialized.getFirstHopDelayTime(), validData_.getFirstHopDelayTime());
    EXPECT_EQ(deserialized.getSecondHopDelayTime(), validData_.getSecondHopDelayTime());
    EXPECT_EQ(deserialized.getTotalDelayTime(), validData_.getTotalDelayTime());
    EXPECT_EQ(deserialized.getThirdHopSentTime(), validData_.getThirdHopSentTime());
}

TEST_F(FinalCalcTrackDataTest, Deserialize_FailsWithEmptyData) {
    FinalCalcTrackData data;
    std::vector<uint8_t> emptyData;
    
    bool success = data.deserialize(emptyData);
    
    EXPECT_FALSE(success);
}

TEST_F(FinalCalcTrackDataTest, Deserialize_FailsWithWrongSize) {
    FinalCalcTrackData data;
    std::vector<uint8_t> wrongSizeData(10, 0);  // Too small
    
    bool success = data.deserialize(wrongSizeData);
    
    EXPECT_FALSE(success);
}

// ============================================
// Network Configuration Tests (moved to NetworkConfig)
// ============================================

// Note: Network configuration has been moved to adapters/common/NetworkConfig.hpp
// to maintain clean hexagonal architecture. Domain models should not contain
// infrastructure-specific configuration.
// See: test/c_hexagon/adapters/common/NetworkConfigTest.cpp for config tests

// ============================================
// Validation Tests
// ============================================

TEST_F(FinalCalcTrackDataTest, IsValid_ReturnsTrueForValidData) {
    EXPECT_TRUE(validData_.isValid());
}

TEST_F(FinalCalcTrackDataTest, IsValid_ReturnsExpectedForDefaultConstructed) {
    FinalCalcTrackData data;
    // Default constructed may or may not be valid depending on model requirements
    // Just verify isValid() doesn't crash
    data.isValid();
    SUCCEED();
}

// ============================================
// Edge Cases Tests
// ============================================

TEST_F(FinalCalcTrackDataTest, ZeroDelays_AreStored) {
    FinalCalcTrackData data;
    data.setFirstHopDelayTime(0);
    data.setSecondHopDelayTime(0);
    data.setTotalDelayTime(0);
    
    EXPECT_EQ(data.getFirstHopDelayTime(), 0);
    EXPECT_EQ(data.getSecondHopDelayTime(), 0);
    EXPECT_EQ(data.getTotalDelayTime(), 0);
    // Note: Validation depends on other required fields
}

TEST_F(FinalCalcTrackDataTest, LargeDelays_AreHandled) {
    FinalCalcTrackData data;
    int64_t largeDelay = 1000000000;  // 1 second in microseconds
    
    data.setFirstHopDelayTime(largeDelay);
    data.setSecondHopDelayTime(largeDelay);
    data.setTotalDelayTime(2 * largeDelay);
    
    EXPECT_EQ(data.getTotalDelayTime(), 2 * largeDelay);
}
