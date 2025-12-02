/**
 * @file ExtrapTrackDataTest.cpp
 * @brief Unit tests for ExtrapTrackData domain model
 * @details Tests serialization, deserialization, and validation
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <vector>
#include <cstdint>

#include "domain/ports/ExtrapTrackData.hpp"

using namespace domain::ports;

/**
 * @brief Test fixture for ExtrapTrackData
 */
class ExtrapTrackDataTest : public ::testing::Test {
protected:
    /**
     * @brief Create valid ExtrapTrackData for testing
     */
    ExtrapTrackData createValidData() {
        ExtrapTrackData data;
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
        return data;
    }
};

// ==================== Default Construction Tests ====================

TEST_F(ExtrapTrackDataTest, DefaultConstructor_ZeroInitialized) {
    ExtrapTrackData data;
    
    EXPECT_EQ(data.getTrackId(), 0);
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), 0.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), 0.0);
}

// ==================== Setter/Getter Tests ====================

TEST_F(ExtrapTrackDataTest, SetTrackId_ValidId_StoresCorrectly) {
    ExtrapTrackData data;
    data.setTrackId(9999);
    
    EXPECT_EQ(data.getTrackId(), 9999);
}

TEST_F(ExtrapTrackDataTest, SetPositionECEF_ValidValues_StoresCorrectly) {
    ExtrapTrackData data;
    data.setXPositionECEF(1111111.0);
    data.setYPositionECEF(2222222.0);
    data.setZPositionECEF(3333333.0);
    
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), 1111111.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), 2222222.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), 3333333.0);
}

TEST_F(ExtrapTrackDataTest, SetVelocityECEF_ValidValues_StoresCorrectly) {
    ExtrapTrackData data;
    data.setXVelocityECEF(111.0);
    data.setYVelocityECEF(222.0);
    data.setZVelocityECEF(333.0);
    
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), 111.0);
    EXPECT_DOUBLE_EQ(data.getYVelocityECEF(), 222.0);
    EXPECT_DOUBLE_EQ(data.getZVelocityECEF(), 333.0);
}

TEST_F(ExtrapTrackDataTest, SetTimingData_ValidValues_StoresCorrectly) {
    ExtrapTrackData data;
    data.setUpdateTime(9999999999L);
    data.setOriginalUpdateTime(8888888888L);
    data.setFirstHopSentTime(7777777777L);
    
    EXPECT_EQ(data.getUpdateTime(), 9999999999L);
    EXPECT_EQ(data.getOriginalUpdateTime(), 8888888888L);
    EXPECT_EQ(data.getFirstHopSentTime(), 7777777777L);
}

// ==================== Validation Tests ====================

TEST_F(ExtrapTrackDataTest, IsValid_ValidData_ReturnsTrue) {
    auto data = createValidData();
    
    EXPECT_TRUE(data.isValid());
}

TEST_F(ExtrapTrackDataTest, IsValid_ZeroTrackId_ThrowsException) {
    ExtrapTrackData data;
    
    EXPECT_THROW(data.setTrackId(0), std::out_of_range);
}

TEST_F(ExtrapTrackDataTest, IsValid_NegativeTrackId_ThrowsException) {
    ExtrapTrackData data;
    
    EXPECT_THROW(data.setTrackId(-1), std::out_of_range);
}

// ==================== Serialization Tests ====================

TEST_F(ExtrapTrackDataTest, Serialize_ValidData_ReturnsNonEmptyVector) {
    auto data = createValidData();
    
    std::vector<uint8_t> serialized = data.serialize();
    
    EXPECT_FALSE(serialized.empty());
}

TEST_F(ExtrapTrackDataTest, Serialize_Deserialize_RoundTrip) {
    auto original = createValidData();
    
    std::vector<uint8_t> serialized = original.serialize();
    
    ExtrapTrackData deserialized;
    bool success = deserialized.deserialize(serialized);
    
    ASSERT_TRUE(success);
    EXPECT_EQ(deserialized.getTrackId(), original.getTrackId());
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), original.getXPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), original.getYPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), original.getZPositionECEF());
    EXPECT_DOUBLE_EQ(deserialized.getXVelocityECEF(), original.getXVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getYVelocityECEF(), original.getYVelocityECEF());
    EXPECT_DOUBLE_EQ(deserialized.getZVelocityECEF(), original.getZVelocityECEF());
}

TEST_F(ExtrapTrackDataTest, Deserialize_EmptyVector_ReturnsFalse) {
    ExtrapTrackData data;
    std::vector<uint8_t> empty;
    
    bool success = data.deserialize(empty);
    
    EXPECT_FALSE(success);
}

TEST_F(ExtrapTrackDataTest, Deserialize_TooShortVector_ReturnsFalse) {
    ExtrapTrackData data;
    std::vector<uint8_t> tooShort = {0x01, 0x02, 0x03};
    
    bool success = data.deserialize(tooShort);
    
    EXPECT_FALSE(success);
}

// ==================== Edge Cases ====================

TEST_F(ExtrapTrackDataTest, SetPosition_NegativeValues_StoresCorrectly) {
    ExtrapTrackData data;
    data.setXPositionECEF(-4000000.0);
    data.setYPositionECEF(-3000000.0);
    data.setZPositionECEF(-5000000.0);
    
    EXPECT_DOUBLE_EQ(data.getXPositionECEF(), -4000000.0);
    EXPECT_DOUBLE_EQ(data.getYPositionECEF(), -3000000.0);
    EXPECT_DOUBLE_EQ(data.getZPositionECEF(), -5000000.0);
}

TEST_F(ExtrapTrackDataTest, SetVelocity_NegativeValues_StoresCorrectly) {
    ExtrapTrackData data;
    data.setXVelocityECEF(-100.0);
    data.setYVelocityECEF(-200.0);
    data.setZVelocityECEF(-50.0);
    
    EXPECT_DOUBLE_EQ(data.getXVelocityECEF(), -100.0);
    EXPECT_DOUBLE_EQ(data.getYVelocityECEF(), -200.0);
    EXPECT_DOUBLE_EQ(data.getZVelocityECEF(), -50.0);
}

TEST_F(ExtrapTrackDataTest, Serialize_ExtremeValues_PreservesPrecision) {
    ExtrapTrackData original;
    original.setTrackId(1);
    original.setXPositionECEF(9e10);
    original.setYPositionECEF(-9e10);
    original.setZPositionECEF(1e-10);
    
    auto serialized = original.serialize();
    
    ExtrapTrackData deserialized;
    deserialized.deserialize(serialized);
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), 9e10);
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), -9e10);
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), 1e-10);
}
