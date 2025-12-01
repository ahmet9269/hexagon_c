/**
 * @file FinalCalculationServiceTest.cpp
 * @brief Unit tests for FinalCalculationService domain logic
 */

#include <gtest/gtest.h>
#include "domain/logic/FinalCalculationService.hpp"
#include "domain/ports/outgoing/ITrackDataStatisticOutgoingPort.hpp"
#include "utils/Logger.hpp"
#include <memory>
#include <vector>

using namespace domain::logic;
using namespace domain::ports;

/**
 * @brief Mock outgoing port for testing
 */
class MockOutgoingPort : public outgoing::ITrackDataStatisticOutgoingPort {
public:
    MockOutgoingPort() : ready_(true), sendCount_(0) {}
    
    bool isReady() const override { return ready_; }
    
    void sendFinalTrackData(const FinalCalcTrackData& data) override {
        lastSentData_ = data;
        sendCount_++;
    }
    
    // Test helpers
    void setReady(bool ready) { ready_ = ready; }
    const FinalCalcTrackData& getLastSentData() const { return lastSentData_; }
    int getSendCount() const { return sendCount_; }
    
private:
    bool ready_;
    FinalCalcTrackData lastSentData_;
    int sendCount_;
};

/**
 * @brief Test fixture for FinalCalculationService tests
 */
class FinalCalculationServiceTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // Initialize logger once for all tests
        utils::Logger::init("test_service");
    }
    
    static void TearDownTestSuite() {
        utils::Logger::shutdown();
    }
    
    void SetUp() override {
        // Create input data
        inputData_.setTrackId(1001);
        inputData_.setXPositionECEF(1000000.0);
        inputData_.setYPositionECEF(2000000.0);
        inputData_.setZPositionECEF(3000000.0);
        inputData_.setXVelocityECEF(100.0);
        inputData_.setYVelocityECEF(200.0);
        inputData_.setZVelocityECEF(300.0);
        inputData_.setOriginalUpdateTime(1000000);
        inputData_.setUpdateTime(1000100);
        inputData_.setFirstHopSentTime(1000050);
        inputData_.setFirstHopDelayTime(50);
        inputData_.setSecondHopSentTime(1000150);
    }

    DelayCalcTrackData inputData_;
};

// ============================================
// Constructor Tests
// ============================================

TEST_F(FinalCalculationServiceTest, DefaultConstructor_CreatesServiceWithoutPort) {
    FinalCalculationService service;
    // Should not throw
    SUCCEED();
}

TEST_F(FinalCalculationServiceTest, ConstructorWithPort_AcceptsOutgoingPort) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    FinalCalculationService service(std::move(mockPort));
    // Should not throw
    SUCCEED();
}

// ============================================
// Interface Implementation Tests
// ============================================

TEST_F(FinalCalculationServiceTest, SubmitDelayCalcTrackData_ProcessesData) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    EXPECT_EQ(mockPortPtr->getSendCount(), 1);
}

TEST_F(FinalCalculationServiceTest, SubmitDelayCalcTrackData_PreservesTrackId) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    EXPECT_EQ(mockPortPtr->getLastSentData().getTrackId(), inputData_.getTrackId());
}

TEST_F(FinalCalculationServiceTest, SubmitDelayCalcTrackData_CopiesPositions) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    const auto& output = mockPortPtr->getLastSentData();
    EXPECT_DOUBLE_EQ(output.getXPositionECEF(), inputData_.getXPositionECEF());
    EXPECT_DOUBLE_EQ(output.getYPositionECEF(), inputData_.getYPositionECEF());
    EXPECT_DOUBLE_EQ(output.getZPositionECEF(), inputData_.getZPositionECEF());
}

TEST_F(FinalCalculationServiceTest, SubmitDelayCalcTrackData_CopiesVelocities) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    const auto& output = mockPortPtr->getLastSentData();
    EXPECT_DOUBLE_EQ(output.getXVelocityECEF(), inputData_.getXVelocityECEF());
    EXPECT_DOUBLE_EQ(output.getYVelocityECEF(), inputData_.getYVelocityECEF());
    EXPECT_DOUBLE_EQ(output.getZVelocityECEF(), inputData_.getZVelocityECEF());
}

// ============================================
// Delay Calculation Tests
// ============================================

TEST_F(FinalCalculationServiceTest, ProcessData_CopiesFirstHopDelayTime) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    EXPECT_EQ(mockPortPtr->getLastSentData().getFirstHopDelayTime(), 
              inputData_.getFirstHopDelayTime());
}

TEST_F(FinalCalculationServiceTest, ProcessData_CalculatesSecondHopDelay) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    // SecondHopDelayTime should be calculated (currentTime - secondHopSentTime)
    // Since we can't predict exact currentTime, just verify it's set
    EXPECT_GE(mockPortPtr->getLastSentData().getSecondHopDelayTime(), 0);
}

TEST_F(FinalCalculationServiceTest, ProcessData_SetsThirdHopSentTime) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    // ThirdHopSentTime should be set to current time
    EXPECT_GT(mockPortPtr->getLastSentData().getThirdHopSentTime(), 0);
}

TEST_F(FinalCalculationServiceTest, ProcessData_CalculatesTotalDelay) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    const auto& output = mockPortPtr->getLastSentData();
    // Total should equal first + second hop delays
    EXPECT_EQ(output.getTotalDelayTime(), 
              output.getFirstHopDelayTime() + output.getSecondHopDelayTime());
}

// ============================================
// Edge Case Tests
// ============================================

TEST_F(FinalCalculationServiceTest, ProcessData_WithNullPort_DoesNotCrash) {
    FinalCalculationService service;  // No outgoing port
    
    // Should not crash
    service.submitDelayCalcTrackData(inputData_);
    SUCCEED();
}

TEST_F(FinalCalculationServiceTest, ProcessData_WithNotReadyPort_DoesNotSend) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    mockPortPtr->setReady(false);
    FinalCalculationService service(std::move(mockPort));
    
    service.submitDelayCalcTrackData(inputData_);
    
    EXPECT_EQ(mockPortPtr->getSendCount(), 0);
}

TEST_F(FinalCalculationServiceTest, ProcessMultipleData_AllAreSent) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    DelayCalcTrackData data1, data2, data3;
    data1.setTrackId(1);
    data2.setTrackId(2);
    data3.setTrackId(3);
    
    service.submitDelayCalcTrackData(data1);
    service.submitDelayCalcTrackData(data2);
    service.submitDelayCalcTrackData(data3);
    
    EXPECT_EQ(mockPortPtr->getSendCount(), 3);
    EXPECT_EQ(mockPortPtr->getLastSentData().getTrackId(), 3);
}

TEST_F(FinalCalculationServiceTest, ProcessData_WithZeroDelays_Works) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    DelayCalcTrackData zeroDelayData;
    zeroDelayData.setTrackId(999);
    zeroDelayData.setFirstHopDelayTime(0);
    zeroDelayData.setSecondHopSentTime(0);
    
    service.submitDelayCalcTrackData(zeroDelayData);
    
    EXPECT_EQ(mockPortPtr->getSendCount(), 1);
}

// ============================================
// ProcessDelayCalcData Direct Call Tests
// ============================================

TEST_F(FinalCalculationServiceTest, ProcessDelayCalcData_WorksSameAsSubmit) {
    auto mockPort = std::make_unique<MockOutgoingPort>();
    MockOutgoingPort* mockPortPtr = mockPort.get();
    FinalCalculationService service(std::move(mockPort));
    
    service.processDelayCalcData(inputData_);
    
    EXPECT_EQ(mockPortPtr->getSendCount(), 1);
    EXPECT_EQ(mockPortPtr->getLastSentData().getTrackId(), inputData_.getTrackId());
}
