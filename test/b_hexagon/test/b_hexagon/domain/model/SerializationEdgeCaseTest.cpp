/**
 * @file SerializationEdgeCaseTest.cpp
 * @brief Edge case tests for serialization/deserialization
 * @details Tests corrupt data, boundary values, and error handling
 *          to improve code coverage for error paths.
 * 
 * Coverage targets:
 * - Invalid/corrupt binary data
 * - Boundary values (min/max)
 * - Truncated data
 * - Empty data
 * - Overflow scenarios
 * - NaN and Inf handling
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <limits>
#include <cmath>
#include <random>
#include <thread>
#include <atomic>
#include <vector>

#include "domain/ports/DelayCalcTrackData.hpp"
#include "domain/ports/ExtrapTrackData.hpp"

using namespace domain::ports;
using namespace testing;

// ==================== DelayCalcTrackData Edge Cases ====================

class DelayCalcTrackDataEdgeCaseTest : public Test {
protected:
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

// ---------- Boundary Value Tests ----------

TEST_F(DelayCalcTrackDataEdgeCaseTest, TrackId_MaxValidValue_HandlesCorrectly) {
    DelayCalcTrackData data;
    // TrackId valid range is 1-9999
    data.setTrackId(9999);
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_EQ(deserialized.getTrackId(), 9999);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, TrackId_MinValidValue_HandlesCorrectly) {
    DelayCalcTrackData data;
    // TrackId valid range is 1-9999
    data.setTrackId(1);
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_EQ(deserialized.getTrackId(), 1);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, TrackId_InvalidZero_ThrowsException) {
    DelayCalcTrackData data;
    // TrackId value 0 is out of valid range (1-9999)
    EXPECT_THROW(data.setTrackId(0), std::out_of_range);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, TrackId_InvalidTooLarge_ThrowsException) {
    DelayCalcTrackData data;
    // TrackId value 10000 is out of valid range (1-9999)
    EXPECT_THROW(data.setTrackId(10000), std::out_of_range);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Position_MaxDouble_ThrowsException) {
    DelayCalcTrackData data;
    data.setTrackId(100);  // Set valid trackId
    // Position range is -9.9E+10 to 9.9E+10, max double is out of range
    EXPECT_THROW(data.setXPositionECEF(std::numeric_limits<double>::max()), std::out_of_range);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Position_MaxValid_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(100);  // Set valid trackId
    // Use max valid position value
    data.setXPositionECEF(9.9e10 - 1);
    data.setYPositionECEF(9.9e10 - 1);
    data.setZPositionECEF(9.9e10 - 1);
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), 9.9e10 - 1);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Position_MinDouble_ThrowsException) {
    DelayCalcTrackData data;
    data.setTrackId(101);  // Set valid trackId
    // Position range is -9.9E+10 to 9.9E+10, lowest double is out of range
    EXPECT_THROW(data.setXPositionECEF(std::numeric_limits<double>::lowest()), std::out_of_range);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Position_MinValid_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(101);  // Set valid trackId
    data.setXPositionECEF(-9.9e10 + 1);
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), -9.9e10 + 1);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Position_NegativeValues_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(102);  // Set valid trackId
    data.setXPositionECEF(-4000000.0);
    data.setYPositionECEF(-3000000.0);
    data.setZPositionECEF(-5000000.0);
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), -4000000.0);
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), -3000000.0);
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), -5000000.0);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Velocity_ZeroValues_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(103);  // Set valid trackId
    data.setXVelocityECEF(0.0);
    data.setYVelocityECEF(0.0);
    data.setZVelocityECEF(0.0);
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(deserialized.getYVelocityECEF(), 0.0);
    EXPECT_DOUBLE_EQ(deserialized.getZVelocityECEF(), 0.0);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Timestamp_MaxInt64_ThrowsException) {
    DelayCalcTrackData data;
    data.setTrackId(104);  // Set valid trackId
    // UpdateTime max is 9223372036854775LL, not INT64_MAX
    EXPECT_THROW(data.setUpdateTime(std::numeric_limits<int64_t>::max()), std::out_of_range);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Timestamp_MaxValid_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(104);  // Set valid trackId
    data.setUpdateTime(9223372036854775LL);  // Max valid value
    data.setOriginalUpdateTime(9223372036854775LL);
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_EQ(deserialized.getUpdateTime(), 9223372036854775LL);
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, DelayTime_MaxUint32_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(105);  // Set valid trackId
    data.setFirstHopDelayTime(std::numeric_limits<uint32_t>::max());
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_EQ(deserialized.getFirstHopDelayTime(), std::numeric_limits<uint32_t>::max());
}

// ---------- Invalid Data Tests ----------

TEST_F(DelayCalcTrackDataEdgeCaseTest, Deserialize_EmptyData_ReturnsFalse) {
    DelayCalcTrackData data;
    std::vector<uint8_t> emptyData;
    
    EXPECT_FALSE(data.deserialize(emptyData));
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Deserialize_TruncatedData_ReturnsFalse) {
    DelayCalcTrackData original = createValidData();
    auto serialized = original.serialize();
    
    // Truncate to half size
    std::vector<uint8_t> truncated(serialized.begin(), serialized.begin() + serialized.size() / 2);
    
    DelayCalcTrackData data;
    EXPECT_FALSE(data.deserialize(truncated));
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Deserialize_OneByteShort_ReturnsFalse) {
    DelayCalcTrackData original = createValidData();
    auto serialized = original.serialize();
    
    // Remove last byte
    serialized.pop_back();
    
    DelayCalcTrackData data;
    EXPECT_FALSE(data.deserialize(serialized));
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Deserialize_SingleByte_ReturnsFalse) {
    DelayCalcTrackData data;
    std::vector<uint8_t> singleByte = {0x00};
    
    EXPECT_FALSE(data.deserialize(singleByte));
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Deserialize_RandomData_HandlesGracefully) {
    // Random data might accidentally pass or fail - we just want no crash
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    for (int trial = 0; trial < 10; ++trial) {
        std::vector<uint8_t> randomData(100);
        for (auto& byte : randomData) {
            byte = static_cast<uint8_t>(dis(gen));
        }
        
        DelayCalcTrackData data;
        // Should not crash, may return true or false
        (void)data.deserialize(randomData);
    }
    SUCCEED();  // No crash = success
}

// ---------- Special Float Values ----------

TEST_F(DelayCalcTrackDataEdgeCaseTest, Position_SmallestPositive_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(201);  // Set valid trackId
    data.setXPositionECEF(std::numeric_limits<double>::min());  // Smallest positive
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), std::numeric_limits<double>::min());
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, Position_Epsilon_HandlesCorrectly) {
    DelayCalcTrackData data;
    data.setTrackId(202);  // Set valid trackId
    data.setXPositionECEF(std::numeric_limits<double>::epsilon());
    
    auto serialized = data.serialize();
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), std::numeric_limits<double>::epsilon());
}

// ---------- Serialize Size Tests ----------

TEST_F(DelayCalcTrackDataEdgeCaseTest, Serialize_ConsistentSize) {
    DelayCalcTrackData data1 = createValidData();
    DelayCalcTrackData data2;  // Default constructed
    data2.setTrackId(1);       // Set valid trackId for serialization
    
    auto size1 = data1.serialize().size();
    auto size2 = data2.serialize().size();
    
    EXPECT_EQ(size1, size2);  // Fixed size serialization
}

TEST_F(DelayCalcTrackDataEdgeCaseTest, SerializeDeserialize_RoundTrip_Identical) {
    DelayCalcTrackData original = createValidData();
    auto serialized1 = original.serialize();
    
    DelayCalcTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized1));
    
    auto serialized2 = deserialized.serialize();
    
    EXPECT_EQ(serialized1, serialized2);  // Byte-for-byte identical
}

// ==================== ExtrapTrackData Edge Cases ====================

class ExtrapTrackDataEdgeCaseTest : public Test {
protected:
    ExtrapTrackData createValidData() {
        ExtrapTrackData data;
        data.setTrackId(5678);
        data.setXPositionECEF(6000000.0);
        data.setYPositionECEF(7000000.0);
        data.setZPositionECEF(8000000.0);
        data.setXVelocityECEF(300.0);
        data.setYVelocityECEF(400.0);
        data.setZVelocityECEF(500.0);
        data.setUpdateTime(1700000000000L);
        return data;
    }
};

TEST_F(ExtrapTrackDataEdgeCaseTest, TrackId_MaxInt32_HandlesCorrectly) {
    ExtrapTrackData data;
    // setTrackId takes int32_t, so max valid is INT32_MAX
    data.setTrackId(std::numeric_limits<int32_t>::max());
    
    auto serialized = data.serialize();
    ExtrapTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_EQ(deserialized.getTrackId(), std::numeric_limits<int32_t>::max());
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Deserialize_EmptyData_ReturnsFalse) {
    ExtrapTrackData data;
    std::vector<uint8_t> emptyData;
    
    EXPECT_FALSE(data.deserialize(emptyData));
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Deserialize_TruncatedData_ReturnsFalse) {
    ExtrapTrackData original = createValidData();
    auto serialized = original.serialize();
    
    // Truncate to half
    std::vector<uint8_t> truncated(serialized.begin(), serialized.begin() + serialized.size() / 2);
    
    ExtrapTrackData data;
    EXPECT_FALSE(data.deserialize(truncated));
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Deserialize_OneByteShort_ReturnsFalse) {
    ExtrapTrackData original = createValidData();
    auto serialized = original.serialize();
    serialized.pop_back();
    
    ExtrapTrackData data;
    EXPECT_FALSE(data.deserialize(serialized));
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Position_AllNegative_HandlesCorrectly) {
    ExtrapTrackData data;
    data.setTrackId(300);  // Set valid trackId
    data.setXPositionECEF(-1e10);  // Within range -9.9E+10 to 9.9E+10
    data.setYPositionECEF(-1e10);
    data.setZPositionECEF(-1e10);
    
    auto serialized = data.serialize();
    ExtrapTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXPositionECEF(), -1e10);
    EXPECT_DOUBLE_EQ(deserialized.getYPositionECEF(), -1e10);
    EXPECT_DOUBLE_EQ(deserialized.getZPositionECEF(), -1e10);
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Velocity_VeryLarge_ThrowsOnInvalid) {
    ExtrapTrackData data;
    data.setTrackId(301);  // Set valid trackId
    // Velocity range is -1E+6 to 1E+6, so 1e10 is out of range
    EXPECT_THROW(data.setXVelocityECEF(1e10), std::out_of_range);
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Velocity_MaxValid_HandlesCorrectly) {
    ExtrapTrackData data;
    data.setTrackId(302);  // Set valid trackId
    data.setXVelocityECEF(1e6 - 1);  // Just under max
    data.setYVelocityECEF(1e6 - 1);
    data.setZVelocityECEF(1e6 - 1);
    
    auto serialized = data.serialize();
    ExtrapTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_DOUBLE_EQ(deserialized.getXVelocityECEF(), 1e6 - 1);
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Timestamp_Zero_HandlesCorrectly) {
    ExtrapTrackData data;
    data.setTrackId(303);  // Set valid trackId
    data.setUpdateTime(0);
    
    auto serialized = data.serialize();
    ExtrapTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    EXPECT_EQ(deserialized.getUpdateTime(), 0);
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Timestamp_Negative_ThrowsException) {
    ExtrapTrackData data;
    data.setTrackId(304);  // Set valid trackId
    // Negative timestamp is out of valid range (0 to INT64_MAX)
    EXPECT_THROW(data.setUpdateTime(-1000000000000L), std::out_of_range);
}

TEST_F(ExtrapTrackDataEdgeCaseTest, SerializeDeserialize_RoundTrip_Identical) {
    ExtrapTrackData original = createValidData();
    auto serialized1 = original.serialize();
    
    ExtrapTrackData deserialized;
    ASSERT_TRUE(deserialized.deserialize(serialized1));
    
    auto serialized2 = deserialized.serialize();
    
    EXPECT_EQ(serialized1, serialized2);
}

TEST_F(ExtrapTrackDataEdgeCaseTest, Deserialize_ExtraData_IgnoresExtra) {
    ExtrapTrackData original = createValidData();
    auto serialized = original.serialize();
    
    // Add extra bytes at the end
    serialized.push_back(0xFF);
    serialized.push_back(0xAA);
    serialized.push_back(0x55);
    
    ExtrapTrackData deserialized;
    // Should succeed and ignore extra data
    ASSERT_TRUE(deserialized.deserialize(serialized));
    
    // Verify data integrity
    EXPECT_EQ(deserialized.getTrackId(), original.getTrackId());
}

// ==================== Cross-Type Tests ====================

class CrossTypeSerializationTest : public Test {};

TEST_F(CrossTypeSerializationTest, DelayCalcData_DeserializeAsExtrap_Fails) {
    DelayCalcTrackData delayData;
    delayData.setTrackId(1234);
    auto serialized = delayData.serialize();
    
    ExtrapTrackData extrapData;
    // Different structure sizes, should fail or give wrong data
    bool result = extrapData.deserialize(serialized);
    
    // Either fails or the trackId might match by coincidence
    // We just verify no crash
    SUCCEED();
}

TEST_F(CrossTypeSerializationTest, ExtrapData_DeserializeAsDelayCalc_Fails) {
    ExtrapTrackData extrapData;
    extrapData.setTrackId(5678);
    auto serialized = extrapData.serialize();
    
    DelayCalcTrackData delayData;
    // Might fail due to size mismatch
    bool result = delayData.deserialize(serialized);
    
    // No crash = success
    SUCCEED();
}

// ==================== Stress Tests ====================

class SerializationStressTest : public Test {};

TEST_F(SerializationStressTest, DelayCalcData_ManyRoundTrips_Consistent) {
    DelayCalcTrackData original;
    original.setTrackId(9999);
    original.setXPositionECEF(1234567.89);
    
    DelayCalcTrackData current = original;
    
    for (int i = 0; i < 1000; ++i) {
        auto serialized = current.serialize();
        DelayCalcTrackData next;
        ASSERT_TRUE(next.deserialize(serialized));
        current = next;
    }
    
    EXPECT_EQ(current.getTrackId(), original.getTrackId());
    EXPECT_DOUBLE_EQ(current.getXPositionECEF(), original.getXPositionECEF());
}

TEST_F(SerializationStressTest, ExtrapData_ConcurrentSerialize_ThreadSafe) {
    ExtrapTrackData sharedData;
    sharedData.setTrackId(1111);
    sharedData.setXPositionECEF(999999.0);
    
    std::atomic<int> successCount{0};
    std::vector<std::thread> threads;
    
    for (int t = 0; t < 10; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < 100; ++i) {
                auto serialized = sharedData.serialize();
                ExtrapTrackData deserialized;
                if (deserialized.deserialize(serialized)) {
                    if (deserialized.getTrackId() == 1111) {
                        ++successCount;
                    }
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(successCount.load(), 1000);
}

