/**
 * @file LoggerTest.cpp
 * @brief Unit tests for Logger utility class
 */

#include <gtest/gtest.h>
#include "utils/Logger.hpp"
#include <thread>
#include <chrono>
#include <fstream>

using namespace utils;

/**
 * @brief Test fixture for Logger tests
 */
class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logger before each test
        Logger::init("test_logger");
    }
    
    void TearDown() override {
        // Shutdown logger after each test
        Logger::shutdown();
    }
};

// ============================================
// Initialization Tests
// ============================================

TEST_F(LoggerTest, Init_DoesNotThrow) {
    // Already initialized in SetUp
    SUCCEED();
}

TEST_F(LoggerTest, Shutdown_DoesNotThrow) {
    // Will be called in TearDown
    Logger::shutdown();
    SUCCEED();
}

TEST_F(LoggerTest, DoubleInit_DoesNotThrow) {
    Logger::init("test_logger_2");
    SUCCEED();
}

TEST_F(LoggerTest, DoubleShutdown_DoesNotThrow) {
    Logger::shutdown();
    Logger::shutdown();
    SUCCEED();
}

// ============================================
// Logging Macro Tests
// ============================================

TEST_F(LoggerTest, LOG_INFO_DoesNotThrow) {
    LOG_INFO("Test info message");
    SUCCEED();
}

TEST_F(LoggerTest, LOG_DEBUG_DoesNotThrow) {
    LOG_DEBUG("Test debug message");
    SUCCEED();
}

TEST_F(LoggerTest, LOG_WARN_DoesNotThrow) {
    LOG_WARN("Test warning message");
    SUCCEED();
}

TEST_F(LoggerTest, LOG_ERROR_DoesNotThrow) {
    LOG_ERROR("Test error message");
    SUCCEED();
}

TEST_F(LoggerTest, LOG_CRITICAL_DoesNotThrow) {
    LOG_CRITICAL("Test critical message");
    SUCCEED();
}

// ============================================
// Formatted Logging Tests
// ============================================

TEST_F(LoggerTest, LOG_INFO_WithFormat_Works) {
    LOG_INFO("Test with int: {}", 42);
    LOG_INFO("Test with string: {}", "hello");
    LOG_INFO("Test with multiple: {} {} {}", 1, 2.5, "three");
    SUCCEED();
}

TEST_F(LoggerTest, LOG_DEBUG_WithFormat_Works) {
    LOG_DEBUG("Debug value: {}", 123);
    SUCCEED();
}

TEST_F(LoggerTest, LOG_WARN_WithFormat_Works) {
    LOG_WARN("Warning code: {}", -1);
    SUCCEED();
}

TEST_F(LoggerTest, LOG_ERROR_WithFormat_Works) {
    LOG_ERROR("Error occurred: {} at line {}", "test error", 100);
    SUCCEED();
}

// ============================================
// logTrackReceived Tests
// ============================================

TEST_F(LoggerTest, LogTrackReceived_DoesNotThrow) {
    Logger::logTrackReceived(1001, 50, 75);
    SUCCEED();
}

TEST_F(LoggerTest, LogTrackReceived_WithZeroValues_Works) {
    Logger::logTrackReceived(0, 0, 0);
    SUCCEED();
}

TEST_F(LoggerTest, LogTrackReceived_WithLargeValues_Works) {
    Logger::logTrackReceived(INT32_MAX, 1000000, 1000000);
    SUCCEED();
}

TEST_F(LoggerTest, LogTrackReceived_WithNegativeDelays_Works) {
    // Negative delays shouldn't crash (might indicate clock issues)
    Logger::logTrackReceived(1, -100, -200);
    SUCCEED();
}

// ============================================
// Stress Tests
// ============================================

TEST_F(LoggerTest, HighVolume_Logging) {
    for (int i = 0; i < 1000; ++i) {
        LOG_INFO("Message number: {}", i);
    }
    SUCCEED();
}

TEST_F(LoggerTest, RapidTrackLogging) {
    for (int i = 0; i < 1000; ++i) {
        Logger::logTrackReceived(i, i * 10, i * 20);
    }
    SUCCEED();
}

// ============================================
// Edge Cases
// ============================================

TEST_F(LoggerTest, EmptyMessage_DoesNotThrow) {
    LOG_INFO("");
    SUCCEED();
}

TEST_F(LoggerTest, LongMessage_DoesNotThrow) {
    std::string longMessage(10000, 'X');
    LOG_INFO("{}", longMessage);
    SUCCEED();
}

TEST_F(LoggerTest, SpecialCharacters_DoesNotThrow) {
    LOG_INFO("Special chars: \t\n\r{}[]");
    SUCCEED();
}

TEST_F(LoggerTest, UnicodeMessage_DoesNotThrow) {
    LOG_INFO("Unicode: αβγδε日本語中文");
    SUCCEED();
}
