/**
 * @file AdapterManagerTest.cpp
 * @brief Unit tests for AdapterManager and IAdapter interface
 */

#include <gtest/gtest.h>
#include "adapters/common/AdapterManager.hpp"
#include <memory>
#include <thread>
#include <chrono>

using namespace adapters;

/**
 * @brief Mock adapter for testing
 */
class MockAdapter : public IAdapter {
public:
    explicit MockAdapter(const std::string& name) 
        : name_(name), running_(false), startCount_(0), stopCount_(0) {}
    
    bool start() override {
        startCount_++;
        if (shouldFailStart_) {
            return false;
        }
        running_ = true;
        return true;
    }
    
    void stop() override {
        stopCount_++;
        running_ = false;
    }
    
    bool isRunning() const override {
        return running_;
    }
    
    std::string getName() const override {
        return name_;
    }
    
    // Test helpers
    void setShouldFailStart(bool fail) { shouldFailStart_ = fail; }
    int getStartCount() const { return startCount_; }
    int getStopCount() const { return stopCount_; }
    
private:
    std::string name_;
    bool running_;
    bool shouldFailStart_ = false;
    int startCount_;
    int stopCount_;
};

/**
 * @brief Test fixture for IAdapter tests
 */
class IAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        adapter_ = std::make_shared<MockAdapter>("TestAdapter");
    }
    
    std::shared_ptr<MockAdapter> adapter_;
};

/**
 * @brief Test fixture for MessagePipeline tests
 */
class MessagePipelineTest : public ::testing::Test {
protected:
    void SetUp() override {
        incomingAdapter_ = std::make_shared<MockAdapter>("IncomingAdapter");
        outgoingAdapter_ = std::make_shared<MockAdapter>("OutgoingAdapter");
    }
    
    std::shared_ptr<MockAdapter> incomingAdapter_;
    std::shared_ptr<MockAdapter> outgoingAdapter_;
};

/**
 * @brief Test fixture for AdapterManager tests
 */
class AdapterManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create adapters for pipelines
        adapter1_ = std::make_shared<MockAdapter>("Adapter1");
        adapter2_ = std::make_shared<MockAdapter>("Adapter2");
        adapter3_ = std::make_shared<MockAdapter>("Adapter3");
    }
    
    std::shared_ptr<MockAdapter> adapter1_;
    std::shared_ptr<MockAdapter> adapter2_;
    std::shared_ptr<MockAdapter> adapter3_;
    AdapterManager manager_;
};

// ============================================
// IAdapter Interface Tests
// ============================================

TEST_F(IAdapterTest, Start_SetsRunningState) {
    EXPECT_FALSE(adapter_->isRunning());
    
    bool result = adapter_->start();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(adapter_->isRunning());
}

TEST_F(IAdapterTest, Stop_ClearsRunningState) {
    adapter_->start();
    EXPECT_TRUE(adapter_->isRunning());
    
    adapter_->stop();
    
    EXPECT_FALSE(adapter_->isRunning());
}

TEST_F(IAdapterTest, GetName_ReturnsAdapterName) {
    EXPECT_EQ(adapter_->getName(), "TestAdapter");
}

TEST_F(IAdapterTest, Start_CanBeCalledMultipleTimes) {
    adapter_->start();
    adapter_->start();
    adapter_->start();
    
    EXPECT_EQ(adapter_->getStartCount(), 3);
    EXPECT_TRUE(adapter_->isRunning());
}

TEST_F(IAdapterTest, Stop_CanBeCalledMultipleTimes) {
    adapter_->start();
    adapter_->stop();
    adapter_->stop();
    adapter_->stop();
    
    EXPECT_EQ(adapter_->getStopCount(), 3);
    EXPECT_FALSE(adapter_->isRunning());
}

// ============================================
// MessagePipeline Tests
// ============================================

TEST_F(MessagePipelineTest, Start_StartsAllAdapters) {
    MessagePipeline pipeline("TestPipeline", incomingAdapter_, outgoingAdapter_);
    
    bool result = pipeline.start();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(incomingAdapter_->isRunning());
    EXPECT_TRUE(outgoingAdapter_->isRunning());
}

TEST_F(MessagePipelineTest, Stop_StopsAllAdapters) {
    MessagePipeline pipeline("TestPipeline", incomingAdapter_, outgoingAdapter_);
    
    pipeline.start();
    pipeline.stop();
    
    EXPECT_FALSE(incomingAdapter_->isRunning());
    EXPECT_FALSE(outgoingAdapter_->isRunning());
}

TEST_F(MessagePipelineTest, IsRunning_ReturnsCorrectState) {
    MessagePipeline pipeline("TestPipeline", incomingAdapter_, outgoingAdapter_);
    
    EXPECT_FALSE(pipeline.isRunning());
    
    pipeline.start();
    EXPECT_TRUE(pipeline.isRunning());
    
    pipeline.stop();
    EXPECT_FALSE(pipeline.isRunning());
}

TEST_F(MessagePipelineTest, Start_WorksWithOnlyIncomingAdapter) {
    MessagePipeline pipeline("IncomingOnly", incomingAdapter_);
    // outgoingAdapter is nullptr by default
    
    bool result = pipeline.start();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(incomingAdapter_->isRunning());
}

TEST_F(MessagePipelineTest, Start_WorksWithOnlyOutgoingAdapter) {
    MessagePipeline pipeline("OutgoingOnly");
    pipeline.setOutgoingAdapter(outgoingAdapter_);
    // incomingAdapter is nullptr
    
    bool result = pipeline.start();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(outgoingAdapter_->isRunning());
}

TEST_F(MessagePipelineTest, Start_ReturnsFailsIfIncomingFails) {
    incomingAdapter_->setShouldFailStart(true);
    
    MessagePipeline pipeline("FailingPipeline", incomingAdapter_, outgoingAdapter_);
    
    bool result = pipeline.start();
    
    EXPECT_FALSE(result);
    // Outgoing should not be started if incoming fails
    EXPECT_FALSE(outgoingAdapter_->isRunning());
}

TEST_F(MessagePipelineTest, Start_ReturnsFailsIfOutgoingFails) {
    outgoingAdapter_->setShouldFailStart(true);
    
    MessagePipeline pipeline("FailingPipeline", incomingAdapter_, outgoingAdapter_);
    
    bool result = pipeline.start();
    
    EXPECT_FALSE(result);
}

TEST_F(MessagePipelineTest, GetName_ReturnsCorrectName) {
    MessagePipeline pipeline("MyPipeline");
    
    EXPECT_EQ(pipeline.getName(), "MyPipeline");
}

TEST_F(MessagePipelineTest, SetName_ChangesName) {
    MessagePipeline pipeline("OldName");
    pipeline.setName("NewName");
    
    EXPECT_EQ(pipeline.getName(), "NewName");
}

TEST_F(MessagePipelineTest, HasAdapters_ReturnsCorrectState) {
    MessagePipeline pipeline("Test");
    
    EXPECT_FALSE(pipeline.hasIncomingAdapter());
    EXPECT_FALSE(pipeline.hasOutgoingAdapter());
    
    pipeline.setIncomingAdapter(incomingAdapter_);
    EXPECT_TRUE(pipeline.hasIncomingAdapter());
    
    pipeline.setOutgoingAdapter(outgoingAdapter_);
    EXPECT_TRUE(pipeline.hasOutgoingAdapter());
}

TEST_F(MessagePipelineTest, GetAdapters_ReturnsCorrectPointers) {
    MessagePipeline pipeline("Test", incomingAdapter_, outgoingAdapter_);
    
    EXPECT_EQ(pipeline.getIncomingAdapter(), incomingAdapter_);
    EXPECT_EQ(pipeline.getOutgoingAdapter(), outgoingAdapter_);
}

// ============================================
// AdapterManager Tests
// ============================================

TEST_F(AdapterManagerTest, Constructor_InitializesEmptyManager) {
    EXPECT_EQ(manager_.getPipelineCount(), 0U);
    EXPECT_FALSE(manager_.isRunning());
}

TEST_F(AdapterManagerTest, RegisterPipeline_IncreasesCount) {
    MessagePipeline pipeline("Pipeline1", adapter1_);
    
    manager_.registerPipeline(std::move(pipeline));
    
    EXPECT_EQ(manager_.getPipelineCount(), 1U);
}

TEST_F(AdapterManagerTest, RegisterMultiplePipelines_IncreasesCount) {
    MessagePipeline pipeline1("Pipeline1", adapter1_);
    MessagePipeline pipeline2("Pipeline2", adapter2_);
    MessagePipeline pipeline3("Pipeline3", adapter3_);
    
    manager_.registerPipeline(std::move(pipeline1));
    manager_.registerPipeline(std::move(pipeline2));
    manager_.registerPipeline(std::move(pipeline3));
    
    EXPECT_EQ(manager_.getPipelineCount(), 3U);
}

TEST_F(AdapterManagerTest, StartAll_StartsAllPipelines) {
    MessagePipeline pipeline1("Pipeline1", adapter1_);
    MessagePipeline pipeline2("Pipeline2", adapter2_);
    
    manager_.registerPipeline(std::move(pipeline1));
    manager_.registerPipeline(std::move(pipeline2));
    
    bool result = manager_.startAll();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(manager_.isRunning());
    EXPECT_TRUE(adapter1_->isRunning());
    EXPECT_TRUE(adapter2_->isRunning());
}

TEST_F(AdapterManagerTest, StopAll_StopsAllPipelines) {
    MessagePipeline pipeline1("Pipeline1", adapter1_);
    MessagePipeline pipeline2("Pipeline2", adapter2_);
    
    manager_.registerPipeline(std::move(pipeline1));
    manager_.registerPipeline(std::move(pipeline2));
    manager_.startAll();
    
    manager_.stopAll();
    
    EXPECT_FALSE(manager_.isRunning());
    EXPECT_FALSE(adapter1_->isRunning());
    EXPECT_FALSE(adapter2_->isRunning());
}

TEST_F(AdapterManagerTest, StartAll_ReturnsFalseIfAnyFails) {
    adapter2_->setShouldFailStart(true);
    
    MessagePipeline pipeline1("Pipeline1", adapter1_);
    MessagePipeline pipeline2("Pipeline2", adapter2_);
    
    manager_.registerPipeline(std::move(pipeline1));
    manager_.registerPipeline(std::move(pipeline2));
    
    bool result = manager_.startAll();
    
    EXPECT_FALSE(result);
}

TEST_F(AdapterManagerTest, Destructor_StopsAllPipelines) {
    {
        AdapterManager tempManager;
        MessagePipeline pipeline("Pipeline", adapter1_);
        
        tempManager.registerPipeline(std::move(pipeline));
        tempManager.startAll();
        
        EXPECT_TRUE(adapter1_->isRunning());
    } // tempManager destructor called here
    
    EXPECT_FALSE(adapter1_->isRunning());
}

TEST_F(AdapterManagerTest, StartAll_WithNoPipelines_ReturnsTrue) {
    bool result = manager_.startAll();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(manager_.isRunning());
}

TEST_F(AdapterManagerTest, StopAll_WithNoPipelines_DoesNotCrash) {
    manager_.stopAll();
    
    EXPECT_FALSE(manager_.isRunning());
}

TEST_F(AdapterManagerTest, GetPipeline_ReturnsCorrectPipeline) {
    MessagePipeline pipeline1("Pipeline1", adapter1_);
    MessagePipeline pipeline2("Pipeline2", adapter2_);
    
    manager_.registerPipeline(std::move(pipeline1));
    manager_.registerPipeline(std::move(pipeline2));
    
    MessagePipeline* found = manager_.getPipeline("Pipeline1");
    
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "Pipeline1");
}

TEST_F(AdapterManagerTest, GetPipeline_ReturnsNullptrIfNotFound) {
    MessagePipeline pipeline("Pipeline1", adapter1_);
    
    manager_.registerPipeline(std::move(pipeline));
    
    MessagePipeline* found = manager_.getPipeline("NonExistent");
    
    EXPECT_EQ(found, nullptr);
}

// ============================================
// Thread Safety Tests (Basic)
// ============================================

TEST_F(AdapterManagerTest, MultipleStartStop_IsStable) {
    MessagePipeline pipeline("Pipeline", adapter1_);
    
    manager_.registerPipeline(std::move(pipeline));
    
    for (int i = 0; i < 10; ++i) {
        manager_.startAll();
        EXPECT_TRUE(manager_.isRunning());
        manager_.stopAll();
        EXPECT_FALSE(manager_.isRunning());
    }
}
