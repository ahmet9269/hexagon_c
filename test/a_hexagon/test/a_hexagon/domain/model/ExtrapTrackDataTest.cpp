/**
 * @file ExtrapTrackDataTest.cpp
 * @brief Unit tests for ExtrapTrackData model class
 * @details GTest based tests for MISRA C++ 2023 compliant ExtrapTrackData
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "domain/ports/ExtrapTrackData.hpp"
#include <vector>
#include <cstdint>
#include <limits>

using namespace domain::model;

/**
 * @brief Test fixture for ExtrapTrackData tests
 */
class ExtrapTrackDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a valid ExtrapTrackData instance for testing
        validExtrapData_.setTrackId(2001);
        validExtrapData_.setXPositionECEF(4000000.0);
        validExtrapData_.setYPositionECEF(3000000.0);
        validExtrapData_.setZPositionECEF(5000000.0);
        validExtrapData_.setXVelocityECEF(100.0);
        validExtrapData_.setYVelocityECEF(200.0);
        validExtrapData_.setZVelocityECEF(50.0);
        validExtrapData_.setOriginalUpdateTime(1700000000000);
        validExtrapData_.setUpdateTime(1700000000100);
        validExtrapData_.setFirstHopSentTime(1700000000050);
    }

    ExtrapTrackData validExtrapData_;
};

// ==================== Constructor Tests ====================

TEST_F(ExtrapTrackDataTest, DefaultConstructor_CreatesValidObject) {
    ExtrapTrackData extrap;
    
    EXPECT_EQ(extrap.getTrackId(), 0);
    EXPECT_DOUBLE_EQ(extrap.getXPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(extrap.getYPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(extrap.getZPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(extrap.getXVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(extrap.getYVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(extrap.getZVelocityECEF(), 0.0);
    EXPECT_EQ(extrap.getOriginalUpdateTime(), 0);
    EXPECT_EQ(extrap.getUpdateTime(), 0);
    EXPECT_EQ(extrap.getFirstHopSentTime(), 0);
}

TEST_F(ExtrapTrackDataTest, CopyConstructor_CopiesAllFields) {
    ExtrapTrackData copy(validExtrapData_);
    
    EXPECT_EQ(copy.getTrackId(), validExtrapData_.getTrackId());
    EXPECT_DOUBLE_EQ(copy.getXPositionECEF(), validExtrapData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(copy.getYPositionECEF(), validExtrapData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(copy.getZPositionECEF(), validExtrapData_.getZPositionECEF());
    EXPECT_EQ(copy.getOriginalUpdateTime(), validExtrapData_.getOriginalUpdateTime());
    EXPECT_EQ(copy.getUpdateTime(), validExtrapData_.getUpdateTime());
    EXPECT_EQ(copy.getFirstHopSentTime(), validExtrapData_.getFirstHopSentTime());
}

TEST_F(ExtrapTrackDataTest, MoveConstructor_MovesAllFields) {
    ExtrapTrackData original;
    original.setTrackId(777);
    original.setXPositionECEF(12345.6);
    original.setUpdateTime(999999);
    
    ExtrapTrackData moved(std::move(original));
    
    EXPECT_EQ(moved.getTrackId(), 777);
    EXPECT_DOUBLE_EQ(moved.getXPositionECEF(), 12345.6);
    EXPECT_EQ(moved.getUpdateTime(), 999999);
}

// ==================== Timing Field Tests ====================

TEST_F(ExtrapTrackDataTest, SetUpdateTime_GetUpdateTime_ReturnsCorrectValue) {
    ExtrapTrackData extrap;
    int64_t updateTime = 1700000000500;
    
    extrap.setUpdateTime(updateTime);
    
    EXPECT_EQ(extrap.getUpdateTime(), updateTime);
}

TEST_F(ExtrapTrackDataTest, SetFirstHopSentTime_GetFirstHopSentTime_ReturnsCorrectValue) {
    ExtrapTrackData extrap;
    int64_t hopTime = 1700000000300;
    
    extrap.setFirstHopSentTime(hopTime);
    
    EXPECT_EQ(extrap.getFirstHopSentTime(), hopTime);
}

TEST_F(ExtrapTrackDataTest, AllTimingFields_CanBeSetAndRetrieved) {
    ExtrapTrackData extrap;
    
    extrap.setOriginalUpdateTime(1000000);
    extrap.setUpdateTime(1000100);
    extrap.setFirstHopSentTime(1000050);
    
    EXPECT_EQ(extrap.getOriginalUpdateTime(), 1000000);
    EXPECT_EQ(extrap.getUpdateTime(), 1000100);
    EXPECT_EQ(extrap.getFirstHopSentTime(), 1000050);
}

// ==================== Position/Velocity Tests ====================

TEST_F(ExtrapTrackDataTest, SetPosition_ECEF_CoordinatesSet) {
    ExtrapTrackData extrap;
    
    extrap.setXPositionECEF(1234567.89);
    extrap.setYPositionECEF(-9876543.21);
    extrap.setZPositionECEF(5555555.55);
    
    EXPECT_DOUBLE_EQ(extrap.getXPositionECEF(), 1234567.89);
    EXPECT_DOUBLE_EQ(extrap.getYPositionECEF(), -9876543.21);
    EXPECT_DOUBLE_EQ(extrap.getZPositionECEF(), 5555555.55);
}

TEST_F(ExtrapTrackDataTest, SetVelocity_ECEF_VelocitiesSet) {
    ExtrapTrackData extrap;
    
    extrap.setXVelocityECEF(250.5);
    extrap.setYVelocityECEF(-125.25);
    extrap.setZVelocityECEF(0.0);
    
    EXPECT_DOUBLE_EQ(extrap.getXVelocityECEF(), 250.5);
    EXPECT_DOUBLE_EQ(extrap.getYVelocityECEF(), -125.25);
    EXPECT_DOUBLE_EQ(extrap.getZVelocityECEF(), 0.0);
}

// ==================== Validation Tests ====================

TEST_F(ExtrapTrackDataTest, IsValid_ValidData_ReturnsTrue) {
    EXPECT_TRUE(validExtrapData_.isValid());
}

TEST_F(ExtrapTrackDataTest, IsValid_DefaultConstructed_ReturnsFalse) {
    ExtrapTrackData extrap;
    EXPECT_FALSE(extrap.isValid());
}

// ==================== Serialization Tests ====================

TEST_F(ExtrapTrackDataTest, Serialize_ReturnsNonEmptyVector) {
    std::vector<uint8_t> serialized = validExtrapData_.serialize();
    
    EXPECT_FALSE(serialized.empty());
    EXPECT_EQ(serialized.size(), validExtrapData_.getSerializedSize());
}

TEST_F(ExtrapTrackDataTest, GetSerializedSize_ReturnsExpectedSize) {
    // int32_t (4) + 6 * double (48) + 3 * int64_t (24) = 76 bytes
    std::size_t expectedSize = sizeof(int32_t) + 6 * sizeof(double) + 3 * sizeof(int64_t);
    
    EXPECT_EQ(validExtrapData_.getSerializedSize(), expectedSize);
}

TEST_F(ExtrapTrackDataTest, SerializeDeserialize_RoundTrip_DataPreserved) {
    // Serialize
    std::vector<uint8_t> serialized = validExtrapData_.serialize();
    
    // Deserialize into new object
    ExtrapTrackData deserialized;
    bool success = deserialized.deserialize(serialized);
    
    EXPECT_TRUE(success);
    EXPECT_EQ(deserialized.getTrackId(), validExtrapData_.getTrackId());
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), validExtrapData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), validExtrapData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), validExtrapData_.getZPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getXVelocityECEF(), validExtrapData_.getXVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYVelocityECEF(), validExtrapData_.getYVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZVelocityECEF(), validExtrapData_.getZVelocityECEF());
    EXPECT_EQ(deserialized.getOriginalUpdateTime(), validExtrapData_.getOriginalUpdateTime());
    EXPECT_EQ(deserialized.getUpdateTime(), validExtrapData_.getUpdateTime());
    EXPECT_EQ(deserialized.getFirstHopSentTime(), validExtrapData_.getFirstHopSentTime());
}

TEST_F(ExtrapTrackDataTest, Deserialize_InvalidSize_ReturnsFalse) {
    ExtrapTrackData extrap;
    std::vector<uint8_t> tooSmall = {0x01, 0x02, 0x03};
    
    bool success = extrap.deserialize(tooSmall);
    
    EXPECT_FALSE(success);
}

TEST_F(ExtrapTrackDataTest, Deserialize_EmptyVector_ReturnsFalse) {
    ExtrapTrackData extrap;
    std::vector<uint8_t> empty;
    
    bool success = extrap.deserialize(empty);
    
    EXPECT_FALSE(success);
}

// ==================== Boundary Value Tests ====================

TEST_F(ExtrapTrackDataTest, TimingFields_MaxInt64_ThrowsException) {
    ExtrapTrackData extrap;
    int64_t maxValue = std::numeric_limits<int64_t>::max();
    
    // Max int64 exceeds validation range, should throw
    EXPECT_THROW(extrap.setOriginalUpdateTime(maxValue), std::out_of_range);
    EXPECT_THROW(extrap.setUpdateTime(maxValue), std::out_of_range);
    EXPECT_THROW(extrap.setFirstHopSentTime(maxValue), std::out_of_range);
}

// ==================== Network Configuration Tests ====================

TEST_F(ExtrapTrackDataTest, MulticastAddress_CorrectValue) {
    EXPECT_STREQ(ExtrapTrackData::MULTICAST_ADDRESS, "239.1.1.5");
}

TEST_F(ExtrapTrackDataTest, Port_CorrectValue) {
    EXPECT_EQ(ExtrapTrackData::PORT, 9596);
}

TEST_F(ExtrapTrackDataTest, SocketType_IsRADIO) {
    EXPECT_STREQ(ExtrapTrackData::ZMQ_SOCKET_TYPE, "RADIO");
}

TEST_F(ExtrapTrackDataTest, IsPublisher_True) {
    EXPECT_TRUE(ExtrapTrackData::IS_PUBLISHER);
}

// ==================== Extrapolation Timing Consistency ====================

TEST_F(ExtrapTrackDataTest, TimingConsistency_UpdateTimeAfterOriginal) {
    ExtrapTrackData extrap;
    
    extrap.setOriginalUpdateTime(1000000);
    extrap.setUpdateTime(1000100);
    
    EXPECT_GT(extrap.getUpdateTime(), extrap.getOriginalUpdateTime());
}

TEST_F(ExtrapTrackDataTest, TimingConsistency_FirstHopBetweenOriginalAndUpdate) {
    ExtrapTrackData extrap;
    
    extrap.setOriginalUpdateTime(1000000);
    extrap.setFirstHopSentTime(1000050);
    extrap.setUpdateTime(1000100);
    
    EXPECT_GT(extrap.getFirstHopSentTime(), extrap.getOriginalUpdateTime());
    EXPECT_LT(extrap.getFirstHopSentTime(), extrap.getUpdateTime());
}
