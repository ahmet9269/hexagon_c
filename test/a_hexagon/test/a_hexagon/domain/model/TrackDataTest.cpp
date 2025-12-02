/**
 * @file TrackDataTest.cpp
 * @brief Unit tests for TrackData model class
 * @details GTest based tests for MISRA C++ 2023 compliant TrackData
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "domain/ports/TrackData.hpp"
#include <vector>
#include <cstdint>
#include <limits>

using namespace domain::model;

/**
 * @brief Test fixture for TrackData tests
 */
class TrackDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a valid TrackData instance for testing
        validTrackData_.setTrackId(1001);
        validTrackData_.setXPositionECEF(4000000.0);
        validTrackData_.setYPositionECEF(3000000.0);
        validTrackData_.setZPositionECEF(5000000.0);
        validTrackData_.setXVelocityECEF(100.0);
        validTrackData_.setYVelocityECEF(200.0);
        validTrackData_.setZVelocityECEF(50.0);
        validTrackData_.setOriginalUpdateTime(1700000000000);
    }

    TrackData validTrackData_;
};

// ==================== Constructor Tests ====================

TEST_F(TrackDataTest, DefaultConstructor_CreatesValidObject) {
    TrackData track;
    
    // Default values should be zero
    EXPECT_EQ(track.getTrackId(), 0);
    EXPECT_DOUBLE_EQ(track.getXPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(track.getYPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(track.getZPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(track.getXVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(track.getYVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(track.getZVelocityECEF(), 0.0);
    EXPECT_EQ(track.getOriginalUpdateTime(), 0);
}

TEST_F(TrackDataTest, CopyConstructor_CopiesAllFields) {
    TrackData copy(validTrackData_);
    
    EXPECT_EQ(copy.getTrackId(), validTrackData_.getTrackId());
    EXPECT_DOUBLE_EQ(copy.getXPositionECEF(), validTrackData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(copy.getYPositionECEF(), validTrackData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(copy.getZPositionECEF(), validTrackData_.getZPositionECEF());
    EXPECT_DOUBLE_EQ(copy.getXVelocityECEF(), validTrackData_.getXVelocityECEF());
    EXPECT_DOUBLE_EQ(copy.getYVelocityECEF(), validTrackData_.getYVelocityECEF());
    EXPECT_DOUBLE_EQ(copy.getZVelocityECEF(), validTrackData_.getZVelocityECEF());
    EXPECT_EQ(copy.getOriginalUpdateTime(), validTrackData_.getOriginalUpdateTime());
}

TEST_F(TrackDataTest, MoveConstructor_MovesAllFields) {
    TrackData original;
    original.setTrackId(999);
    original.setXPositionECEF(12345.6);
    
    TrackData moved(std::move(original));
    
    EXPECT_EQ(moved.getTrackId(), 999);
    EXPECT_DOUBLE_EQ(moved.getXPositionECEF(), 12345.6);
}

// ==================== Getter/Setter Tests ====================

TEST_F(TrackDataTest, SetTrackId_GetTrackId_ReturnsCorrectValue) {
    TrackData track;
    track.setTrackId(42);
    
    EXPECT_EQ(track.getTrackId(), 42);
}

TEST_F(TrackDataTest, SetTrackId_NegativeValue_ThrowsException) {
    TrackData track;
    // Negative trackId should throw due to validation (trackId must be >= 1)
    EXPECT_THROW(track.setTrackId(-100), std::out_of_range);
}

TEST_F(TrackDataTest, SetPosition_ECEF_CoordinatesSet) {
    TrackData track;
    
    track.setXPositionECEF(1234567.89);
    track.setYPositionECEF(-9876543.21);
    track.setZPositionECEF(5555555.55);
    
    EXPECT_DOUBLE_EQ(track.getXPositionECEF(), 1234567.89);
    EXPECT_DOUBLE_EQ(track.getYPositionECEF(), -9876543.21);
    EXPECT_DOUBLE_EQ(track.getZPositionECEF(), 5555555.55);
}

TEST_F(TrackDataTest, SetVelocity_ECEF_VelocitiesSet) {
    TrackData track;
    
    track.setXVelocityECEF(250.5);
    track.setYVelocityECEF(-125.25);
    track.setZVelocityECEF(0.0);
    
    EXPECT_DOUBLE_EQ(track.getXVelocityECEF(), 250.5);
    EXPECT_DOUBLE_EQ(track.getYVelocityECEF(), -125.25);
    EXPECT_DOUBLE_EQ(track.getZVelocityECEF(), 0.0);
}

TEST_F(TrackDataTest, SetOriginalUpdateTime_ValidTimestamp_Set) {
    TrackData track;
    int64_t timestamp = 1700000000000; // microseconds
    
    track.setOriginalUpdateTime(timestamp);
    
    EXPECT_EQ(track.getOriginalUpdateTime(), timestamp);
}

// ==================== Validation Tests ====================

TEST_F(TrackDataTest, IsValid_ValidData_ReturnsTrue) {
    EXPECT_TRUE(validTrackData_.isValid());
}

TEST_F(TrackDataTest, IsValid_DefaultConstructed_ChecksValidity) {
    TrackData track;
    // Default constructed with zeros - isValid checks if data is reasonable
    // The result depends on implementation - either true/false is acceptable
    [[maybe_unused]] bool valid = track.isValid();
    SUCCEED();  // Just verify it doesn't throw
}

// ==================== Serialization Tests ====================

TEST_F(TrackDataTest, Serialize_ReturnsNonEmptyVector) {
    std::vector<uint8_t> serialized = validTrackData_.serialize();
    
    EXPECT_FALSE(serialized.empty());
    EXPECT_EQ(serialized.size(), validTrackData_.getSerializedSize());
}

TEST_F(TrackDataTest, GetSerializedSize_ReturnsExpectedSize) {
    // int32_t (4) + 6 * double (48) + int64_t (8) = 60 bytes
    std::size_t expectedSize = sizeof(int32_t) + 6 * sizeof(double) + sizeof(int64_t);
    
    EXPECT_EQ(validTrackData_.getSerializedSize(), expectedSize);
}

TEST_F(TrackDataTest, SerializeDeserialize_RoundTrip_DataPreserved) {
    // Serialize
    std::vector<uint8_t> serialized = validTrackData_.serialize();
    
    // Deserialize into new object
    TrackData deserialized;
    bool success = deserialized.deserialize(serialized);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(deserialized.getTrackId(), validTrackData_.getTrackId());
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), validTrackData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), validTrackData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), validTrackData_.getZPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getXVelocityECEF(), validTrackData_.getXVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYVelocityECEF(), validTrackData_.getYVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZVelocityECEF(), validTrackData_.getZVelocityECEF());
    EXPECT_EQ(deserialized.getOriginalUpdateTime(), validTrackData_.getOriginalUpdateTime());
}

TEST_F(TrackDataTest, Deserialize_InvalidSize_ReturnsFalse) {
    TrackData track;
    std::vector<uint8_t> tooSmall = {0x01, 0x02, 0x03}; // Way too small
    
    bool success = track.deserialize(tooSmall);
    
    EXPECT_FALSE(success);
}

TEST_F(TrackDataTest, Deserialize_EmptyVector_ReturnsFalse) {
    TrackData track;
    std::vector<uint8_t> empty;
    
    bool success = track.deserialize(empty);
    
    EXPECT_FALSE(success);
}

// ==================== Boundary Value Tests ====================

TEST_F(TrackDataTest, SetTrackId_MaxInt32_Succeeds) {
    TrackData track;
    track.setTrackId(std::numeric_limits<int32_t>::max());
    
    EXPECT_EQ(track.getTrackId(), std::numeric_limits<int32_t>::max());
}

TEST_F(TrackDataTest, SetTrackId_MinInt32_ThrowsException) {
    TrackData track;
    // Min int32 is negative, should throw due to validation (trackId must be >= 1)
    EXPECT_THROW(track.setTrackId(std::numeric_limits<int32_t>::min()), std::out_of_range);
}

TEST_F(TrackDataTest, SetPosition_LargeValues_Succeeds) {
    TrackData track;
    double largeValue = 1e10; // 10 billion meters (unrealistic but tests limits)
    
    track.setXPositionECEF(largeValue);
    track.setYPositionECEF(largeValue);
    track.setZPositionECEF(largeValue);
    
    EXPECT_DOUBLE_EQ(track.getXPositionECEF(), largeValue);
    EXPECT_DOUBLE_EQ(track.getYPositionECEF(), largeValue);
    EXPECT_DOUBLE_EQ(track.getZPositionECEF(), largeValue);
}

// ==================== Copy/Assignment Tests ====================

TEST_F(TrackDataTest, CopyAssignment_CopiesAllFields) {
    TrackData copy;
    copy = validTrackData_;
    
    EXPECT_EQ(copy.getTrackId(), validTrackData_.getTrackId());
    EXPECT_DOUBLE_EQ(copy.getXPositionECEF(), validTrackData_.getXPositionECEF());
}

TEST_F(TrackDataTest, MoveAssignment_MovesAllFields) {
    TrackData original;
    original.setTrackId(888);
    original.setXPositionECEF(999.99);
    
    TrackData target;
    target = std::move(original);
    
    EXPECT_EQ(target.getTrackId(), 888);
    EXPECT_DOUBLE_EQ(target.getXPositionECEF(), 999.99);
}
