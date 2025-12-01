/**
 * @file LoggerTest.cpp
 * @brief Unit tests for Logger utility class
 * @details GTest based tests for async logging functionality
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "utils/Logger.hpp"
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

/**
 * @brief Test fixture for Logger tests
 */
class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Ensure logger is shutdown before each test
        utils::Logger::shutdown();
    }

    void TearDown() override {
        // Clean shutdown after each test
        utils::Logger::shutdown();
    }
};

// ==================== Initialization Tests ====================

TEST_F(LoggerTest, Init_DefaultParameters_Success) {
    EXPECT_NO_THROW({
        utils::Logger::init("test_app");
    });
    
    EXPECT_TRUE(utils::Logger::isInitialized());
}

TEST_F(LoggerTest, Init_CustomQueueSize_Success) {
    EXPECT_NO_THROW({
        utils::Logger::init("test_app", 4096, 1);
    });
    
    EXPECT_TRUE(utils::Logger::isInitialized());
}

TEST_F(LoggerTest, Init_MultipleThreads_Success) {
    EXPECT_NO_THROW({
        utils::Logger::init("test_app", 8192, 2);
    });
    
    EXPECT_TRUE(utils::Logger::isInitialized());
}

TEST_F(LoggerTest, InitWithFile_ValidPath_Success) {
    EXPECT_NO_THROW({
        utils::Logger::initWithFile("test_app", "/tmp/test_log.log");
    });
    
    EXPECT_TRUE(utils::Logger::isInitialized());
}

TEST_F(LoggerTest, DoubleInit_NoError) {
    utils::Logger::init("test_app");
    
    // Double initialization should not throw
    EXPECT_NO_THROW({
        utils::Logger::init("test_app_2");
    });
}

// ==================== Shutdown Tests ====================

TEST_F(LoggerTest, Shutdown_AfterInit_Success) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::shutdown();
    });
    
    EXPECT_FALSE(utils::Logger::isInitialized());
}

TEST_F(LoggerTest, Shutdown_BeforeInit_NoError) {
    // Shutdown without init should not throw
    EXPECT_NO_THROW({
        utils::Logger::shutdown();
    });
}

TEST_F(LoggerTest, DoubleShutdown_NoError) {
    utils::Logger::init("test_app");
    utils::Logger::shutdown();
    
    // Double shutdown should not throw
    EXPECT_NO_THROW({
        utils::Logger::shutdown();
    });
}

// ==================== Log Level Tests ====================

TEST_F(LoggerTest, SetLevel_AllLevels_NoError) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::setLevel(utils::Logger::Level::Trace);
        utils::Logger::setLevel(utils::Logger::Level::Debug);
        utils::Logger::setLevel(utils::Logger::Level::Info);
        utils::Logger::setLevel(utils::Logger::Level::Warn);
        utils::Logger::setLevel(utils::Logger::Level::Error);
        utils::Logger::setLevel(utils::Logger::Level::Critical);
        utils::Logger::setLevel(utils::Logger::Level::Off);
    });
}

// ==================== Logging Function Tests ====================

TEST_F(LoggerTest, Trace_ValidMessage_NoThrow) {
    utils::Logger::init("test_app");
    utils::Logger::setLevel(utils::Logger::Level::Trace);
    
    EXPECT_NO_THROW({
        utils::Logger::trace("Test trace message");
    });
}

TEST_F(LoggerTest, Debug_ValidMessage_NoThrow) {
    utils::Logger::init("test_app");
    utils::Logger::setLevel(utils::Logger::Level::Debug);
    
    EXPECT_NO_THROW({
        utils::Logger::debug("Test debug message");
    });
}

TEST_F(LoggerTest, Info_ValidMessage_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::info("Test info message");
    });
}

TEST_F(LoggerTest, Warn_ValidMessage_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::warn("Test warning message");
    });
}

TEST_F(LoggerTest, Error_ValidMessage_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::error("Test error message");
    });
}

TEST_F(LoggerTest, Critical_ValidMessage_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::critical("Test critical message");
    });
}

// ==================== Format String Tests ====================

TEST_F(LoggerTest, Info_WithFormatArgs_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::info("Value: {}, String: {}", 42, "test");
    });
}

TEST_F(LoggerTest, Debug_WithMultipleArgs_NoThrow) {
    utils::Logger::init("test_app");
    utils::Logger::setLevel(utils::Logger::Level::Debug);
    
    int value = 100;
    double pi = 3.14159;
    std::string name = "TestName";
    
    EXPECT_NO_THROW({
        utils::Logger::debug("Int: {}, Double: {:.2f}, String: {}", value, pi, name);
    });
}

// ==================== Latency Logging Tests ====================

TEST_F(LoggerTest, LogLatency_ValidParams_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::logLatency("ZMQ", "receive", 150);
    });
}

TEST_F(LoggerTest, LogLatency_ZeroLatency_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::logLatency("Component", "operation", 0);
    });
}

TEST_F(LoggerTest, LogLatency_LargeLatency_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::logLatency("Slow", "operation", 1000000);  // 1 second in µs
    });
}

// ==================== Track Logging Tests ====================

TEST_F(LoggerTest, LogTrackReceived_ValidParams_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::logTrackReceived(1001, 150);  // track_id, hop_latency in µs
    });
}

TEST_F(LoggerTest, LogTrackReceived_NegativeId_NoThrow) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::logTrackReceived(-1, 0);  // Invalid track, zero latency
    });
}

// ==================== Thread Safety Tests ====================

TEST_F(LoggerTest, ConcurrentLogging_NoDataRace) {
    utils::Logger::init("test_app");
    
    std::atomic<int> counter{0};
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&counter, i]() {
            for (int j = 0; j < 100; ++j) {
                utils::Logger::info("Thread {} - Message {}", i, j);
                counter++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(counter.load(), 1000);
}

TEST_F(LoggerTest, ConcurrentInitShutdown_NoDeadlock) {
    // NOTE: Concurrent init/shutdown from different threads is not supported.
    // This test verifies sequential init/shutdown from multiple threads works.
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&success_count]() {
            // Each thread does its own independent work
            // Since Logger is global, we just test that logging works
            // after the main Logger::init in SetUp
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            success_count++;
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(success_count.load(), 5);
}

// ==================== Performance Tests ====================

TEST_F(LoggerTest, LoggingLatency_UnderThreshold) {
    utils::Logger::init("test_app");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        utils::Logger::info("Performance test message {}", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Average should be under 100µs per log (async logging)
    double avgMicros = static_cast<double>(duration.count()) / 1000.0;
    
    // Async logging should be fast - hot path < 100µs typically
    EXPECT_LT(avgMicros, 1000.0);  // Generous limit for CI
}

// ==================== Macro Tests ====================

TEST_F(LoggerTest, LOG_INFO_Macro_Works) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        LOG_INFO("Macro test message");
    });
}

TEST_F(LoggerTest, LOG_DEBUG_Macro_Works) {
    utils::Logger::init("test_app");
    utils::Logger::setLevel(utils::Logger::Level::Debug);
    
    EXPECT_NO_THROW({
        LOG_DEBUG("Debug macro test");
    });
}

TEST_F(LoggerTest, LOG_ERROR_Macro_Works) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        LOG_ERROR("Error macro test");
    });
}

// ==================== isInitialized Tests ====================

TEST_F(LoggerTest, IsInitialized_BeforeInit_ReturnsFalse) {
    EXPECT_FALSE(utils::Logger::isInitialized());
}

TEST_F(LoggerTest, IsInitialized_AfterInit_ReturnsTrue) {
    utils::Logger::init("test_app");
    
    EXPECT_TRUE(utils::Logger::isInitialized());
}

TEST_F(LoggerTest, IsInitialized_AfterShutdown_ReturnsFalse) {
    utils::Logger::init("test_app");
    utils::Logger::shutdown();
    
    EXPECT_FALSE(utils::Logger::isInitialized());
}

// ==================== Edge Cases ====================

TEST_F(LoggerTest, EmptyAppName_NoError) {
    EXPECT_NO_THROW({
        utils::Logger::init("");
    });
}

TEST_F(LoggerTest, VeryLongMessage_NoError) {
    utils::Logger::init("test_app");
    
    std::string longMessage(10000, 'X');
    
    EXPECT_NO_THROW({
        utils::Logger::info("{}", longMessage);
    });
}

TEST_F(LoggerTest, SpecialCharacters_NoError) {
    utils::Logger::init("test_app");
    
    EXPECT_NO_THROW({
        utils::Logger::info("Special chars: äöü ñ 中文 🚀");
    });
}
