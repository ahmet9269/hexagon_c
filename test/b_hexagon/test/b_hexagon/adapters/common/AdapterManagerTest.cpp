/**
 * @file AdapterManagerTest.cpp
 * @brief Unit tests for AdapterManager with Thread-per-Type pattern
 * @details Tests pipeline lifecycle, threading, and management
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>

#include "adapters/common/IAdapter.hpp"
#include "adapters/common/AdapterManager.hpp"
#include "adapters/common/MessagePipeline.hpp"

using namespace adapters;
using ::testing::_;
using ::testing::Return;

/**
 * @brief Mock adapter for testing pipelines
 */
class MockAdapter : public IAdapter {
public:
    explicit MockAdapter(const std::string& name) 
        : m_name(name), m_running(false), m_startCount(0), m_stopCount(0) {}
    
    ~MockAdapter() override = default;

    bool start() override {
        m_running = true;
        ++m_startCount;
        return true;
    }

    void stop() override {
        m_running = false;
        ++m_stopCount;
    }

    [[nodiscard]] bool isRunning() const override {
        return m_running;
    }

    [[nodiscard]] std::string getName() const noexcept override {
        return m_name;
    }

    int getStartCount() const { return m_startCount; }
    int getStopCount() const { return m_stopCount; }

private:
    std::string m_name;
    std::atomic<bool> m_running;
    std::atomic<int> m_startCount;
    std::atomic<int> m_stopCount;
};

/**
 * @brief Test fixture for AdapterManager
 */
class AdapterManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_adapterManager = std::make_unique<AdapterManager>();
    }

    void TearDown() override {
        if (m_adapterManager) {
            m_adapterManager->stopAll();
        }
    }

    /**
     * @brief Create a test pipeline with mock adapters
     */
    MessagePipeline createTestPipeline(const std::string& name) {
        auto inAdapter = std::make_shared<MockAdapter>(name + "_in");
        auto outAdapter = std::make_shared<MockAdapter>(name + "_out");
        return MessagePipeline(name, inAdapter, outAdapter);
    }

    std::unique_ptr<AdapterManager> m_adapterManager;
};

// ==================== Registration Tests ====================

TEST_F(AdapterManagerTest, RegisterPipeline_SinglePipeline_IncreasesCount) {
    m_adapterManager->registerPipeline(createTestPipeline("TestPipeline"));
    
    EXPECT_EQ(m_adapterManager->getPipelineCount(), 1);
}

TEST_F(AdapterManagerTest, RegisterPipeline_MultiplePipelines_AllRegistered) {
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline1"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline2"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline3"));
    
    EXPECT_EQ(m_adapterManager->getPipelineCount(), 3);
}

TEST_F(AdapterManagerTest, RegisterPipeline_TwentyPipelines_ScalesCorrectly) {
    for (int i = 0; i < 20; ++i) {
        m_adapterManager->registerPipeline(
            createTestPipeline("Pipeline" + std::to_string(i))
        );
    }
    
    EXPECT_EQ(m_adapterManager->getPipelineCount(), 20);
}

// ==================== Lifecycle Tests ====================

TEST_F(AdapterManagerTest, StartAll_AllPipelinesStarted_ReturnsTrue) {
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline1"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline2"));
    
    bool result = m_adapterManager->startAll();
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(m_adapterManager->isRunning());
}

TEST_F(AdapterManagerTest, StopAll_AllPipelinesStopped) {
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline1"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline2"));
    m_adapterManager->startAll();
    
    m_adapterManager->stopAll();
    
    EXPECT_FALSE(m_adapterManager->isRunning());
}

TEST_F(AdapterManagerTest, StartAll_NoPipelines_NoException) {
    EXPECT_NO_THROW(m_adapterManager->startAll());
}

TEST_F(AdapterManagerTest, StopAll_NoPipelines_NoException) {
    EXPECT_NO_THROW(m_adapterManager->stopAll());
}

TEST_F(AdapterManagerTest, IsRunning_BeforeStart_ReturnsFalse) {
    m_adapterManager->registerPipeline(createTestPipeline("TestPipeline"));
    
    EXPECT_FALSE(m_adapterManager->isRunning());
}

TEST_F(AdapterManagerTest, IsRunning_AfterStart_ReturnsTrue) {
    m_adapterManager->registerPipeline(createTestPipeline("TestPipeline"));
    
    m_adapterManager->startAll();
    
    EXPECT_TRUE(m_adapterManager->isRunning());
}

TEST_F(AdapterManagerTest, IsRunning_AfterStop_ReturnsFalse) {
    m_adapterManager->registerPipeline(createTestPipeline("TestPipeline"));
    m_adapterManager->startAll();
    
    m_adapterManager->stopAll();
    
    EXPECT_FALSE(m_adapterManager->isRunning());
}

// ==================== Query Tests ====================

TEST_F(AdapterManagerTest, GetPipeline_ExistingName_ReturnsPipeline) {
    m_adapterManager->registerPipeline(createTestPipeline("TestPipeline"));
    
    auto* pipeline = m_adapterManager->getPipeline("TestPipeline");
    
    ASSERT_NE(pipeline, nullptr);
    EXPECT_EQ(pipeline->getName(), "TestPipeline");
}

TEST_F(AdapterManagerTest, GetPipeline_NonExistingName_ReturnsNull) {
    auto* pipeline = m_adapterManager->getPipeline("NonExisting");
    
    EXPECT_EQ(pipeline, nullptr);
}

TEST_F(AdapterManagerTest, GetPipelineNames_ReturnsAllNames) {
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline1"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline2"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline3"));
    
    auto names = m_adapterManager->getPipelineNames();
    
    EXPECT_EQ(names.size(), 3);
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Pipeline1") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Pipeline2") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Pipeline3") != names.end());
}

// ==================== Thread Safety Tests ====================

TEST_F(AdapterManagerTest, ConcurrentRegistration_ThreadSafe) {
    std::vector<std::thread> threads;
    const int numThreads = 10;
    std::atomic<int> successCount{0};
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([this, i]() {
            m_adapterManager->registerPipeline(
                createTestPipeline("Pipeline" + std::to_string(i))
            );
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(m_adapterManager->getPipelineCount(), numThreads);
}

TEST_F(AdapterManagerTest, ConcurrentStartStop_ThreadSafe) {
    m_adapterManager->registerPipeline(createTestPipeline("TestPipeline"));
    
    std::thread startThread([this]() {
        for (int i = 0; i < 100; ++i) {
            m_adapterManager->startAll();
        }
    });
    
    std::thread stopThread([this]() {
        for (int i = 0; i < 100; ++i) {
            m_adapterManager->stopAll();
        }
    });
    
    startThread.join();
    stopThread.join();
    
    // No crash or deadlock = success
    SUCCEED();
}

// ==================== Pipeline Interaction Tests ====================

TEST_F(AdapterManagerTest, StartAll_PipelinesStartInOrder) {
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline1"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline2"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline3"));
    
    m_adapterManager->startAll();
    
    auto* p1 = m_adapterManager->getPipeline("Pipeline1");
    auto* p2 = m_adapterManager->getPipeline("Pipeline2");
    auto* p3 = m_adapterManager->getPipeline("Pipeline3");
    
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    ASSERT_NE(p3, nullptr);
    
    EXPECT_TRUE(p1->isRunning());
    EXPECT_TRUE(p2->isRunning());
    EXPECT_TRUE(p3->isRunning());
}

TEST_F(AdapterManagerTest, StopAll_PipelinesStopGracefully) {
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline1"));
    m_adapterManager->registerPipeline(createTestPipeline("Pipeline2"));
    m_adapterManager->startAll();
    
    // Give some time for startup
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    m_adapterManager->stopAll();
    
    auto* p1 = m_adapterManager->getPipeline("Pipeline1");
    auto* p2 = m_adapterManager->getPipeline("Pipeline2");
    
    EXPECT_FALSE(p1->isRunning());
    EXPECT_FALSE(p2->isRunning());
}
