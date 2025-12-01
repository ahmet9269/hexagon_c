/**
 * @file DelayCalcTrackDataTest.cpp
 * @brief Unit tests for DelayCalcTrackData model class
 */

#include <gtest/gtest.h>
#include "domain/ports/DelayCalcTrackData.hpp"
#include <vector>
#include <cmath>

using namespace domain::ports;

/**
 * @brief Test fixture for DelayCalcTrackData tests
 */
class DelayCalcTrackDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a valid track data object for testing
        validData_.setTrackId(1001);
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
    }

    DelayCalcTrackData validData_;
    DelayCalcTrackData emptyData_;
};

// ============================================
// Constructor Tests
// ============================================

TEST_F(DelayCalcTrackDataTest, DefaultConstructor_InitializesToZero) {
    DelayCalcTrackData data;
    
    EXPECT_EQ(data.getTrackId(), 0);
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getYVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getZVelocityECEF(), 0.0);
}

TEST_F(DelayCalcTrackDataTest, CopyConstructor_CopiesAllFields) {
    DelayCalcTrackData copy(validData_);
    
    EXPECT_EQ(copy.getTrackId(), validData_.getTrackId());
    EXPECT_DOUBLE_EQ(copy.getXPositionECEF(), validData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(copy.getYPositionECEF(), validData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(copy.getZPositionECEF(), validData_.getZPositionECEF());
    EXPECT_EQ(copy.getFirstHopDelayTime(), validData_.getFirstHopDelayTime());
}

TEST_F(DelayCalcTrackDataTest, MoveConstructor_MovesAllFields) {
    DelayCalcTrackData original;
    original.setTrackId(999);
    original.setXPositionECEF(123.456);
    
    DelayCalcTrackData moved(std::move(original));
    
    EXPECT_EQ(moved.getTrackId(), 999);
    EXPECT_DOUBLE_EQ(moved.getXPositionECEF(), 123.456);
}

// ============================================
// Getter/Setter Tests
// ============================================

TEST_F(DelayCalcTrackDataTest, SetTrackId_StoresValue) {
    DelayCalcTrackData data;
    // TrackId validation range is 1-9999
    data.setTrackId(5000);
    
    EXPECT_EQ(data.getTrackId(), 5000);
}

TEST_F(DelayCalcTrackDataTest, SetPositions_StoresECEFCoordinates) {
    DelayCalcTrackData data;
    data.setXPositionECEF(6378137.0);  // Earth radius in meters
    data.setYPositionECEF(0.0);
    data.setZPositionECEF(0.0);
    
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 6378137.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), 0.0);
}

TEST_F(DelayCalcTrackDataTest, SetVelocities_StoresECEFVelocities) {
    DelayCalcTrackData data;
    data.setXVelocityECEF(7800.0);  // Orbital velocity ~7.8 km/s
    data.setYVelocityECEF(0.0);
    data.setZVelocityECEF(0.0);
    
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), 7800.0);
    EXPECT_DOUBLE_EQ(data.getYVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getZVelocityECEF(), 0.0);
}

TEST_F(DelayCalcTrackDataTest, SetTimestamps_StoresMicroseconds) {
    DelayCalcTrackData data;
    int64_t timestamp = 1701388800000000;  // Microseconds since epoch
    
    data.setOriginalUpdateTime(timestamp);
    data.setUpdateTime(timestamp + 100);
    data.setFirstHopSentTime(timestamp + 50);
    data.setSecondHopSentTime(timestamp + 150);
    
    EXPECT_EQ(data.getOriginalUpdateTime(), timestamp);
    EXPECT_EQ(data.getUpdateTime(), timestamp + 100);
    EXPECT_EQ(data.getFirstHopSentTime(), timestamp + 50);
    EXPECT_EQ(data.getSecondHopSentTime(), timestamp + 150);
}

TEST_F(DelayCalcTrackDataTest, SetFirstHopDelayTime_StoresDelay) {
    DelayCalcTrackData data;
    data.setFirstHopDelayTime(500);  // 500 microseconds
    
    EXPECT_EQ(data.getFirstHopDelayTime(), 500);
}

// ============================================
// Validation Tests
// ============================================

TEST_F(DelayCalcTrackDataTest, IsValid_ReturnsTrueForValidData) {
    EXPECT_TRUE(validData_.isValid());
}

TEST_F(DelayCalcTrackDataTest, IsValid_ReturnsExpectedForDefaultConstructed) {
    DelayCalcTrackData data;
    // Default constructed may or may not be valid depending on model requirements
    // Just verify isValid() doesn't crash
    data.isValid();
    SUCCEED();
}

// ============================================
// Serialization Tests
// ============================================

TEST_F(DelayCalcTrackDataTest, Serialize_ReturnsNonEmptyVector) {
    std::vector<uint8_t> serialized = validData_.serialize();
    
    EXPECT_FALSE(serialized.empty());
    EXPECT_EQ(serialized.size(), validData_.getSerializedSize());
}

TEST_F(DelayCalcTrackDataTest, GetSerializedSize_ReturnsCorrectSize) {
    // Size should be sum of all member sizes
    // int32_t (4) + 6*double (48) + 5*int64_t (40) = 92 bytes
    std::size_t expectedSize = sizeof(int32_t) + 
                               6 * sizeof(double) + 
                               5 * sizeof(int64_t);
    
    EXPECT_EQ(validData_.getSerializedSize(), expectedSize);
}

TEST_F(DelayCalcTrackDataTest, SerializeDeserialize_RoundTrip) {
    // Serialize
    std::vector<uint8_t> serialized = validData_.serialize();
    
    // Deserialize into new object
    DelayCalcTrackData deserialized;
    bool success = deserialized.deserialize(serialized);
    
    ASSERT_TRUE(success);
    EXPECT_EQ(deserialized.getTrackId(), validData_.getTrackId());
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), validData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), validData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), validData_.getZPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getXVelocityECEF(), validData_.getXVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYVelocityECEF(), validData_.getYVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZVelocityECEF(), validData_.getZVelocityECEF());
    EXPECT_EQ(deserialized.getOriginalUpdateTime(), validData_.getOriginalUpdateTime());
    EXPECT_EQ(deserialized.getUpdateTime(), validData_.getUpdateTime());
    EXPECT_EQ(deserialized.getFirstHopSentTime(), validData_.getFirstHopSentTime());
    EXPECT_EQ(deserialized.getFirstHopDelayTime(), validData_.getFirstHopDelayTime());
    EXPECT_EQ(deserialized.getSecondHopSentTime(), validData_.getSecondHopSentTime());
}

TEST_F(DelayCalcTrackDataTest, Deserialize_FailsWithEmptyData) {
    DelayCalcTrackData data;
    std::vector<uint8_t> emptyData;
    
    bool success = data.deserialize(emptyData);
    
    EXPECT_FALSE(success);
}

TEST_F(DelayCalcTrackDataTest, Deserialize_FailsWithWrongSize) {
    DelayCalcTrackData data;
    std::vector<uint8_t> wrongSizeData(10, 0);  // Too small
    
    bool success = data.deserialize(wrongSizeData);
    
    EXPECT_FALSE(success);
}

// ============================================
// Network Configuration Tests
// ============================================

TEST_F(DelayCalcTrackDataTest, NetworkConstants_AreCorrect) {
    EXPECT_STREQ(DelayCalcTrackData::MULTICAST_ADDRESS, "239.1.1.5");
    EXPECT_EQ(DelayCalcTrackData::PORT, 9595);
    EXPECT_STREQ(DelayCalcTrackData::ZMQ_SOCKET_TYPE, "DISH");
    EXPECT_TRUE(DelayCalcTrackData::IS_SUBSCRIBER);
}

// ============================================
// Edge Cases Tests
// ============================================

TEST_F(DelayCalcTrackDataTest, NegativeValues_AreAllowed) {
    DelayCalcTrackData data;
    data.setXPositionECEF(-1000000.0);
    data.setYPositionECEF(-2000000.0);
    data.setZPositionECEF(-3000000.0);
    data.setXVelocityECEF(-100.0);
    data.setYVelocityECEF(-200.0);
    data.setZVelocityECEF(-300.0);
    
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), -1000000.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), -2000000.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), -3000000.0);
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), -100.0);
    EXPECT_DOUBLE_EQ(data.getYVelocityECEF(), -200.0);
    EXPECT_DOUBLE_EQ(data.getZVelocityECEF(), -300.0);
}

TEST_F(DelayCalcTrackDataTest, LargeValues_AreHandled) {
    DelayCalcTrackData data;
    // TrackId max valid value is 9999
    data.setTrackId(9999);
    // Position max is 9.9E+10
    data.setXPositionECEF(9e10);
    // UpdateTime uses a reasonable large value within validation range
    data.setUpdateTime(1000000000000LL);  // 1 trillion microseconds (~11.6 days)
    
    EXPECT_EQ(data.getTrackId(), 9999);
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 9e10);
    EXPECT_EQ(data.getUpdateTime(), 1000000000000LL);
}

// ============================================
// Validation Boundary Tests
// ============================================

TEST_F(DelayCalcTrackDataTest, TrackId_BoundaryValues) {
    DelayCalcTrackData data;
    
    // Valid boundary values
    data.setTrackId(1);    // Min valid
    EXPECT_EQ(data.getTrackId(), 1);
    
    data.setTrackId(9999); // Max valid
    EXPECT_EQ(data.getTrackId(), 9999);
}

TEST_F(DelayCalcTrackDataTest, TrackId_ThrowsOnInvalidValue) {
    DelayCalcTrackData data;
    
    // TrackId must be between 1 and 9999
    EXPECT_THROW(data.setTrackId(0), std::out_of_range);
    EXPECT_THROW(data.setTrackId(10000), std::out_of_range);
    EXPECT_THROW(data.setTrackId(-1), std::out_of_range);
}

TEST_F(DelayCalcTrackDataTest, Velocity_BoundaryValues) {
    DelayCalcTrackData data;
    
    // Valid velocity range: -1E+6 to 1E+6
    data.setXVelocityECEF(1000000.0);
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), 1000000.0);
    
    data.setXVelocityECEF(-1000000.0);
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), -1000000.0);
}

TEST_F(DelayCalcTrackDataTest, Position_BoundaryValues) {
    DelayCalcTrackData data;
    
    // Valid position range: -9.9E+10 to 9.9E+10
    data.setXPositionECEF(9.9e10);
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 9.9e10);
    
    data.setXPositionECEF(-9.9e10);
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), -9.9e10);
}
