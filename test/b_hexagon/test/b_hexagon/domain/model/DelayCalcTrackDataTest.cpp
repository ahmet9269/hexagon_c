/**
 * @file DelayCalcTrackDataTest.cpp
 * @brief Unit tests for DelayCalcTrackData domain model
 * @details Tests serialization, deserialization, and validation
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <vector>
#include <cstdint>

#include "domain/ports/DelayCalcTrackData.hpp"

using namespace domain::ports;

/**
 * @brief Test fixture for DelayCalcTrackData
 */
class DelayCalcTrackDataTest : public ::testing::Test {
protected:
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

// ==================== Default Construction Tests ====================

TEST_F(DelayCalcTrackDataTest, DefaultConstructor_ZeroInitialized) {
    DelayCalcTrackData data;
    
    EXPECT_EQ(data.getTrackId(), 0);
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), 0.0);
    EXPECT_EQ(data.getFirstHopDelayTime(), 0);
}

// ==================== Setter/Getter Tests ====================

TEST_F(DelayCalcTrackDataTest, SetTrackId_ValidId_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(9999);
    
    EXPECT_EQ(data.getTrackId(), 9999);
}

TEST_F(DelayCalcTrackDataTest, SetPositionECEF_ValidValues_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setXPositionECEF(1111111.0);
    data.setYPositionECEF(2222222.0);
    data.setZPositionECEF(3333333.0);
    
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 1111111.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), 2222222.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), 3333333.0);
}

TEST_F(DelayCalcTrackDataTest, SetVelocityECEF_ValidValues_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setXVelocityECEF(111.0);
    data.setYVelocityECEF(222.0);
    data.setZVelocityECEF(333.0);
    
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), 111.0);
    EXPECT_DOUBLE_EQ(data.getYVelocityECEF(), 222.0);
    EXPECT_DOUBLE_EQ(data.getZVelocityECEF(), 333.0);
}

TEST_F(DelayCalcTrackDataTest, SetTimingData_ValidValues_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setUpdateTime(9999999999L);
    data.setOriginalUpdateTime(8888888888L);
    data.setFirstHopSentTime(7777777777L);
    data.setSecondHopSentTime(6666666666L);
    
    EXPECT_EQ(data.getUpdateTime(), 9999999999L);
    EXPECT_EQ(data.getOriginalUpdateTime(), 8888888888L);
    EXPECT_EQ(data.getFirstHopSentTime(), 7777777777L);
    EXPECT_EQ(data.getSecondHopSentTime(), 6666666666L);
}

TEST_F(DelayCalcTrackDataTest, SetFirstHopDelayTime_ValidValue_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setFirstHopDelayTime(12345);
    
    EXPECT_EQ(data.getFirstHopDelayTime(), 12345);
}

// ==================== Validation Tests ====================

TEST_F(DelayCalcTrackDataTest, IsValid_ValidData_ReturnsTrue) {
    auto data = createValidData();
    
    EXPECT_TRUE(data.isValid());
}

// ==================== Serialization Tests ====================

TEST_F(DelayCalcTrackDataTest, Serialize_ValidData_ReturnsNonEmptyVector) {
    auto data = createValidData();
    
    std::vector<uint8_t> serialized = data.serialize();
    
    EXPECT_FALSE(serialized.empty());
}

TEST_F(DelayCalcTrackDataTest, Serialize_Deserialize_RoundTrip) {
    auto original = createValidData();
    
    std::vector<uint8_t> serialized = original.serialize();
    
    DelayCalcTrackData deserialized;
    bool success = deserialized.deserialize(serialized);
    
    ASSERT_TRUE(success);
    EXPECT_EQ(deserialized.getTrackId(), original.getTrackId());
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), original.getXPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), original.getYPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), original.getZPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getXVelocityECEF(), original.getXVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYVelocityECEF(), original.getYVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZVelocityECEF(), original.getZVelocityECEF());
    EXPECT_EQ(deserialized.getFirstHopDelayTime(), original.getFirstHopDelayTime());
    EXPECT_EQ(deserialized.getSecondHopSentTime(), original.getSecondHopSentTime());
}

TEST_F(DelayCalcTrackDataTest, Deserialize_EmptyVector_ReturnsFalse) {
    DelayCalcTrackData data;
    std::vector<uint8_t> empty;
    
    bool success = data.deserialize(empty);
    
    EXPECT_FALSE(success);
}

TEST_F(DelayCalcTrackDataTest, Deserialize_TooShortVector_ReturnsFalse) {
    DelayCalcTrackData data;
    std::vector<uint8_t> tooShort = {0x01, 0x02, 0x03};
    
    bool success = data.deserialize(tooShort);
    
    EXPECT_FALSE(success);
}

// ==================== Delay Time Tests ====================

TEST_F(DelayCalcTrackDataTest, FirstHopDelayTime_ZeroDelay_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setFirstHopDelayTime(0);
    
    EXPECT_EQ(data.getFirstHopDelayTime(), 0);
}

TEST_F(DelayCalcTrackDataTest, FirstHopDelayTime_LargeDelay_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setFirstHopDelayTime(1000000);  // 1 second in microseconds
    
    EXPECT_EQ(data.getFirstHopDelayTime(), 1000000);
}

// ==================== Edge Cases ====================

TEST_F(DelayCalcTrackDataTest, SetPosition_NegativeValues_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setXPositionECEF(-4000000.0);
    data.setYPositionECEF(-3000000.0);
    data.setZPositionECEF(-5000000.0);
    
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), -4000000.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), -3000000.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), -5000000.0);
}

TEST_F(DelayCalcTrackDataTest, SetVelocity_NegativeValues_StoresCorrectly) {
    DelayCalcTrackData data;
    data.setXVelocityECEF(-100.0);
    data.setYVelocityECEF(-200.0);
    data.setZVelocityECEF(-50.0);
    
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), -100.0);
    EXPECT_DOUBLE_EQ(data.getYVelocityECEF(), -200.0);
    EXPECT_DOUBLE_EQ(data.getZVelocityECEF(), -50.0);
}

TEST_F(DelayCalcTrackDataTest, Serialize_ExtremeValues_PreservesPrecision) {
    DelayCalcTrackData original;
    original.setTrackId(1);
    original.setXPositionECEF(9e10);
    original.setYPositionECEF(-9e10);
    original.setZPositionECEF(1e-10);
    original.setFirstHopDelayTime(999999);
    
    auto serialized = original.serialize();
    
    DelayCalcTrackData deserialized;
    deserialized.deserialize(serialized);
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), 9e10);
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), -9e10);
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), 1e-10);
    EXPECT_EQ(deserialized.getFirstHopDelayTime(), 999999);
}

// ==================== Timing Consistency Tests ====================

TEST_F(DelayCalcTrackDataTest, TimingConsistency_SecondHopAfterFirstHop) {
    auto data = createValidData();
    
    EXPECT_GT(data.getSecondHopSentTime(), data.getFirstHopSentTime());
}

TEST_F(DelayCalcTrackDataTest, TimingConsistency_FirstHopAfterOriginal) {
    auto data = createValidData();
    
    EXPECT_GT(data.getFirstHopSentTime(), data.getOriginalUpdateTime());
}
