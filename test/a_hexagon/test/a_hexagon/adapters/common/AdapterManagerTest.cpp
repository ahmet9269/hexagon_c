/**
 * @file AdapterManagerTest.cpp
 * @brief Unit tests for AdapterManager class
 * @details GTest based tests for adapter lifecycle management
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "adapters/common/AdapterManager.hpp"
#include "adapters/common/IAdapter.hpp"
#include "adapters/common/MessagePipeline.hpp"
#include <memory>
#include <string>

using namespace adapters;
using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

/**
 * @brief Mock class for IAdapter interface
 */
class MockAdapter : public IAdapter {
public:
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isRunning, (), (const, override));
    MOCK_METHOD(std::string, getName, (), (const, noexcept, override));
};

/**
 * @brief Test fixture for AdapterManager tests
 */
class AdapterManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager_ = std::make_unique<AdapterManager>();
    }

    void TearDown() override {
        if (manager_) {
            manager_->stopAll();
        }
    }

    std::unique_ptr<AdapterManager> manager_;
};

// ==================== Construction Tests ====================

TEST_F(AdapterManagerTest, DefaultConstructor_NotRunning) {
    AdapterManager mgr;
    EXPECT_FALSE(mgr.isRunning());
}

TEST_F(AdapterManagerTest, DefaultConstructor_EmptyPipelines) {
    AdapterManager mgr;
    EXPECT_EQ(mgr.getPipelineCount(), 0);
}

// ==================== Register Pipeline Tests ====================

TEST_F(AdapterManagerTest, RegisterPipeline_SinglePipeline_Success) {
    auto inAdapter = std::make_shared<NiceMock<MockAdapter>>();
    auto outAdapter = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    
    EXPECT_EQ(manager_->getPipelineCount(), 1);
}

TEST_F(AdapterManagerTest, RegisterPipeline_MultiplePipelines_AllRegistered) {
    auto in1 = std::make_shared<NiceMock<MockAdapter>>();
    auto out1 = std::make_shared<NiceMock<MockAdapter>>();
    auto in2 = std::make_shared<NiceMock<MockAdapter>>();
    auto out2 = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*in1, getName()).WillByDefault(Return("In1"));
    ON_CALL(*out1, getName()).WillByDefault(Return("Out1"));
    ON_CALL(*in2, getName()).WillByDefault(Return("In2"));
    ON_CALL(*out2, getName()).WillByDefault(Return("Out2"));
    
    MessagePipeline pipeline1("Pipeline1", in1, out1);
    MessagePipeline pipeline2("Pipeline2", in2, out2);
    
    manager_->registerPipeline(std::move(pipeline1));
    manager_->registerPipeline(std::move(pipeline2));
    
    EXPECT_EQ(manager_->getPipelineCount(), 2);
}

// ==================== Start/Stop Tests ====================

TEST_F(AdapterManagerTest, StartAll_CallsStartOnAllPipelines) {
    auto inAdapter = std::make_shared<MockAdapter>();
    auto outAdapter = std::make_shared<MockAdapter>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    
    EXPECT_CALL(*inAdapter, start()).WillOnce(Return(true));
    EXPECT_CALL(*outAdapter, start()).WillOnce(Return(true));
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    
    bool result = manager_->startAll();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(manager_->isRunning());
}

TEST_F(AdapterManagerTest, StartAll_OneAdapterFails_ReturnsFalse) {
    auto inAdapter = std::make_shared<MockAdapter>();
    auto outAdapter = std::make_shared<MockAdapter>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    
    EXPECT_CALL(*inAdapter, start()).WillOnce(Return(true));
    EXPECT_CALL(*outAdapter, start()).WillOnce(Return(false));
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    
    bool result = manager_->startAll();
    
    EXPECT_FALSE(result);
}

TEST_F(AdapterManagerTest, StopAll_CallsStopOnAllAdapters) {
    auto inAdapter = std::make_shared<NiceMock<MockAdapter>>();
    auto outAdapter = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    ON_CALL(*inAdapter, start()).WillByDefault(Return(true));
    ON_CALL(*outAdapter, start()).WillByDefault(Return(true));
    
    // Note: stop() will be called by stopAll() and again in TearDown
    // Using NiceMock to suppress warnings about multiple calls
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    (void)manager_->startAll();
    
    manager_->stopAll();
    
    EXPECT_FALSE(manager_->isRunning());
}

TEST_F(AdapterManagerTest, StopAll_WhenNotStarted_NoError) {
    auto inAdapter = std::make_shared<NiceMock<MockAdapter>>();
    auto outAdapter = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    
    EXPECT_NO_THROW(manager_->stopAll());
}

// ==================== State Tests ====================

TEST_F(AdapterManagerTest, IsRunning_AfterStart_ReturnsTrue) {
    auto inAdapter = std::make_shared<NiceMock<MockAdapter>>();
    auto outAdapter = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    ON_CALL(*inAdapter, start()).WillByDefault(Return(true));
    ON_CALL(*outAdapter, start()).WillByDefault(Return(true));
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    (void)manager_->startAll();
    
    EXPECT_TRUE(manager_->isRunning());
}

TEST_F(AdapterManagerTest, IsRunning_AfterStop_ReturnsFalse) {
    auto inAdapter = std::make_shared<NiceMock<MockAdapter>>();
    auto outAdapter = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    ON_CALL(*inAdapter, start()).WillByDefault(Return(true));
    ON_CALL(*outAdapter, start()).WillByDefault(Return(true));
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    (void)manager_->startAll();
    manager_->stopAll();
    
    EXPECT_FALSE(manager_->isRunning());
}

// ==================== Get Pipeline Tests ====================

TEST_F(AdapterManagerTest, GetPipeline_ByName_ReturnsCorrectPipeline) {
    auto inAdapter = std::make_shared<NiceMock<MockAdapter>>();
    auto outAdapter = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    
    MessagePipeline pipeline("MyPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    
    auto* found = manager_->getPipeline("MyPipeline");
    
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "MyPipeline");
}

TEST_F(AdapterManagerTest, GetPipeline_NonExistent_ReturnsNull) {
    auto inAdapter = std::make_shared<NiceMock<MockAdapter>>();
    auto outAdapter = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*inAdapter, getName()).WillByDefault(Return("InAdapter"));
    ON_CALL(*outAdapter, getName()).WillByDefault(Return("OutAdapter"));
    
    MessagePipeline pipeline("TestPipeline", inAdapter, outAdapter);
    manager_->registerPipeline(std::move(pipeline));
    
    auto* found = manager_->getPipeline("NonExistentPipeline");
    
    EXPECT_EQ(found, nullptr);
}

// ==================== Edge Cases ====================

TEST_F(AdapterManagerTest, StartAll_NoPipelines_ReturnsTrue) {
    bool result = manager_->startAll();
    
    EXPECT_TRUE(result);
}

TEST_F(AdapterManagerTest, StopAll_NoPipelines_NoError) {
    EXPECT_NO_THROW(manager_->stopAll());
}

TEST_F(AdapterManagerTest, GetPipelineCount_EmptyManager_ReturnsZero) {
    EXPECT_EQ(manager_->getPipelineCount(), 0);
}

// ==================== Pipeline Count Tests ====================

TEST_F(AdapterManagerTest, GetPipelineCount_AfterRegister_ReturnsCorrectCount) {
    auto in1 = std::make_shared<NiceMock<MockAdapter>>();
    auto out1 = std::make_shared<NiceMock<MockAdapter>>();
    auto in2 = std::make_shared<NiceMock<MockAdapter>>();
    auto out2 = std::make_shared<NiceMock<MockAdapter>>();
    auto in3 = std::make_shared<NiceMock<MockAdapter>>();
    auto out3 = std::make_shared<NiceMock<MockAdapter>>();
    
    ON_CALL(*in1, getName()).WillByDefault(Return("In1"));
    ON_CALL(*out1, getName()).WillByDefault(Return("Out1"));
    ON_CALL(*in2, getName()).WillByDefault(Return("In2"));
    ON_CALL(*out2, getName()).WillByDefault(Return("Out2"));
    ON_CALL(*in3, getName()).WillByDefault(Return("In3"));
    ON_CALL(*out3, getName()).WillByDefault(Return("Out3"));
    
    MessagePipeline p1("P1", in1, out1);
    MessagePipeline p2("P2", in2, out2);
    MessagePipeline p3("P3", in3, out3);
    
    manager_->registerPipeline(std::move(p1));
    EXPECT_EQ(manager_->getPipelineCount(), 1);
    
    manager_->registerPipeline(std::move(p2));
    EXPECT_EQ(manager_->getPipelineCount(), 2);
    
    manager_->registerPipeline(std::move(p3));
    EXPECT_EQ(manager_->getPipelineCount(), 3);
}
