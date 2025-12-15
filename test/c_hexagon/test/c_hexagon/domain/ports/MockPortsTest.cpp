/**
 * @file MockPortsTest.cpp
 * @brief Unit tests for Mock Port implementations
 * @details Verifies mock ports capture data correctly for testing
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "test/c_hexagon/domain/ports/MockOutgoingPort.hpp"
#include "test/c_hexagon/domain/ports/MockIncomingPort.hpp"
#include "domain/ports/incoming/DelayCalcTrackData.hpp"
#include "domain/ports/FinalCalcTrackData.hpp"

using namespace domain::ports;
using namespace domain::ports::outgoing::test;
using namespace domain::ports::incoming::test;

// ============================================
// MockOutgoingPort Tests
// ============================================

class MockOutgoingPortTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_port_ = std::make_unique<MockOutgoingPort>();
    }

    std::unique_ptr<MockOutgoingPort> mock_port_;
    
    FinalCalcTrackData createTestData(int32_t trackId) {
        FinalCalcTrackData data;
        data.setTrackId(trackId);
        data.setFirstHopDelayTime(100);
        data.setSecondHopDelayTime(200);
        data.setTotalDelayTime(300);
        return data;
    }
};

TEST_F(MockOutgoingPortTest, IsReady_ReturnsTrueByDefault) {
    EXPECT_TRUE(mock_port_->isReady());
}

TEST_F(MockOutgoingPortTest, SetReady_ChangesReadyState) {
    mock_port_->setReady(false);
    EXPECT_FALSE(mock_port_->isReady());
    
    mock_port_->setReady(true);
    EXPECT_TRUE(mock_port_->isReady());
}

TEST_F(MockOutgoingPortTest, SendFinalTrackData_CapturesData) {
    auto data = createTestData(1001);
    
    mock_port_->sendFinalTrackData(data);
    
    EXPECT_EQ(mock_port_->getSendCount(), 1);
    EXPECT_TRUE(mock_port_->hasSentData());
    
    auto last = mock_port_->getLastSentData();
    EXPECT_EQ(last.getTrackId(), 1001);
}

TEST_F(MockOutgoingPortTest, SendMultipleData_CapturesAll) {
    mock_port_->sendFinalTrackData(createTestData(1));
    mock_port_->sendFinalTrackData(createTestData(2));
    mock_port_->sendFinalTrackData(createTestData(3));
    
    EXPECT_EQ(mock_port_->getSendCount(), 3);
    
    auto all_data = mock_port_->getSentData();
    ASSERT_EQ(all_data.size(), 3);
    EXPECT_EQ(all_data[0].getTrackId(), 1);
    EXPECT_EQ(all_data[1].getTrackId(), 2);
    EXPECT_EQ(all_data[2].getTrackId(), 3);
}

TEST_F(MockOutgoingPortTest, Clear_RemovesAllCapturedData) {
    mock_port_->sendFinalTrackData(createTestData(1));
    mock_port_->sendFinalTrackData(createTestData(2));
    
    EXPECT_EQ(mock_port_->getSendCount(), 2);
    
    mock_port_->clear();
    
    EXPECT_EQ(mock_port_->getSendCount(), 0);
    EXPECT_FALSE(mock_port_->hasSentData());
}

TEST_F(MockOutgoingPortTest, ShouldThrow_ThrowsOnSend) {
    mock_port_->setShouldThrow(true);
    
    EXPECT_THROW(mock_port_->sendFinalTrackData(createTestData(1)), std::runtime_error);
}

// ============================================
// MockIncomingPort Tests
// ============================================

class MockIncomingPortTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_port_ = std::make_unique<MockIncomingPort>();
    }

    std::unique_ptr<MockIncomingPort> mock_port_;
    
    DelayCalcTrackData createTestData(int32_t trackId) {
        DelayCalcTrackData data;
        data.setTrackId(trackId);
        data.setXPositionECEF(1000.0);
        data.setYPositionECEF(2000.0);
        data.setZPositionECEF(3000.0);
        return data;
    }
};

TEST_F(MockIncomingPortTest, SubmitDelayCalcTrackData_CapturesData) {
    auto data = createTestData(1001);
    
    mock_port_->submitDelayCalcTrackData(data);
    
    EXPECT_EQ(mock_port_->getSubmitCount(), 1);
    EXPECT_TRUE(mock_port_->hasSubmittedData());
    
    auto last = mock_port_->getLastSubmittedData();
    EXPECT_EQ(last.getTrackId(), 1001);
}

TEST_F(MockIncomingPortTest, SubmitMultipleData_CapturesAll) {
    mock_port_->submitDelayCalcTrackData(createTestData(1));
    mock_port_->submitDelayCalcTrackData(createTestData(2));
    mock_port_->submitDelayCalcTrackData(createTestData(3));
    
    EXPECT_EQ(mock_port_->getSubmitCount(), 3);
    
    auto all_data = mock_port_->getSubmittedData();
    ASSERT_EQ(all_data.size(), 3);
    EXPECT_EQ(all_data[0].getTrackId(), 1);
    EXPECT_EQ(all_data[1].getTrackId(), 2);
    EXPECT_EQ(all_data[2].getTrackId(), 3);
}

TEST_F(MockIncomingPortTest, Clear_RemovesAllCapturedData) {
    mock_port_->submitDelayCalcTrackData(createTestData(1));
    mock_port_->submitDelayCalcTrackData(createTestData(2));
    
    EXPECT_EQ(mock_port_->getSubmitCount(), 2);
    
    mock_port_->clear();
    
    EXPECT_EQ(mock_port_->getSubmitCount(), 0);
    EXPECT_FALSE(mock_port_->hasSubmittedData());
}

TEST_F(MockIncomingPortTest, ShouldThrow_ThrowsOnSubmit) {
    mock_port_->setShouldThrow(true);
    
    EXPECT_THROW(mock_port_->submitDelayCalcTrackData(createTestData(1)), std::runtime_error);
}

TEST_F(MockIncomingPortTest, StartsEmpty) {
    EXPECT_FALSE(mock_port_->hasSubmittedData());
    EXPECT_EQ(mock_port_->getSubmitCount(), 0);
}
