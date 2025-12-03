/**
 * @file ILoggerTest.cpp
 * @brief Unit tests for ILogger interface and MockLogger
 * @details Verifies mock logger captures log messages correctly
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "utils/ILogger.hpp"
#include "test/c_hexagon/utils/MockLogger.hpp"

using namespace utils;
using namespace utils::test;

/**
 * @brief Test fixture for ILogger tests
 */
class ILoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_logger_ = std::make_unique<MockLogger>();
    }

    void TearDown() override {
        mock_logger_->clear();
    }

    std::unique_ptr<MockLogger> mock_logger_;
};

// ============================================
// Basic Logging Tests
// ============================================

TEST_F(ILoggerTest, Trace_CapturesMessage) {
    mock_logger_->trace("Test trace message");
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("Test trace message"));
}

TEST_F(ILoggerTest, Debug_CapturesMessage) {
    mock_logger_->debug("Test debug message");
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("Test debug message"));
}

TEST_F(ILoggerTest, Info_CapturesMessage) {
    mock_logger_->info("Test info message");
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("Test info message"));
}

TEST_F(ILoggerTest, Warn_CapturesMessage) {
    mock_logger_->warn("Test warning message");
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("Test warning message"));
}

TEST_F(ILoggerTest, Error_CapturesMessage) {
    mock_logger_->error("Test error message");
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("Test error message"));
}

TEST_F(ILoggerTest, Critical_CapturesMessage) {
    mock_logger_->critical("Test critical message");
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("Test critical message"));
}

// ============================================
// Level Filtering Tests
// ============================================

TEST_F(ILoggerTest, SetLevel_FiltersLowerLevels) {
    mock_logger_->setLevel(ILogger::Level::Warn);
    
    mock_logger_->trace("trace");
    mock_logger_->debug("debug");
    mock_logger_->info("info");
    mock_logger_->warn("warning");
    mock_logger_->error("error");
    
    // Only warn and above should be captured
    EXPECT_EQ(mock_logger_->getEntryCount(), 2);
    EXPECT_FALSE(mock_logger_->containsMessage("trace"));
    EXPECT_FALSE(mock_logger_->containsMessage("debug"));
    EXPECT_FALSE(mock_logger_->containsMessage("info"));
    EXPECT_TRUE(mock_logger_->containsMessage("warning"));
    EXPECT_TRUE(mock_logger_->containsMessage("error"));
}

TEST_F(ILoggerTest, GetEntriesByLevel_ReturnsCorrectEntries) {
    mock_logger_->info("info1");
    mock_logger_->warn("warn1");
    mock_logger_->info("info2");
    mock_logger_->error("error1");
    
    auto info_entries = mock_logger_->getEntriesByLevel(ILogger::Level::Info);
    auto warn_entries = mock_logger_->getEntriesByLevel(ILogger::Level::Warn);
    
    EXPECT_EQ(info_entries.size(), 2);
    EXPECT_EQ(warn_entries.size(), 1);
}

// ============================================
// Special Logging Methods Tests
// ============================================

TEST_F(ILoggerTest, LogLatency_FormatsCorrectly) {
    mock_logger_->logLatency("ZMQ", "receive", 150);
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("ZMQ"));
    EXPECT_TRUE(mock_logger_->containsMessage("receive"));
    EXPECT_TRUE(mock_logger_->containsMessage("150"));
}

TEST_F(ILoggerTest, LogTrackReceived_FormatsCorrectly) {
    mock_logger_->logTrackReceived(1001, 50, 75);
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 1);
    EXPECT_TRUE(mock_logger_->containsMessage("1001"));
    EXPECT_TRUE(mock_logger_->containsMessage("50"));
    EXPECT_TRUE(mock_logger_->containsMessage("75"));
}

// ============================================
// Helper Methods Tests
// ============================================

TEST_F(ILoggerTest, Clear_RemovesAllEntries) {
    mock_logger_->info("message1");
    mock_logger_->info("message2");
    mock_logger_->info("message3");
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 3);
    
    mock_logger_->clear();
    
    EXPECT_EQ(mock_logger_->getEntryCount(), 0);
}

TEST_F(ILoggerTest, GetLastMessage_ReturnsLastEntry) {
    mock_logger_->info("first");
    mock_logger_->info("second");
    mock_logger_->info("third");
    
    EXPECT_EQ(mock_logger_->getLastMessage(), "third");
}

TEST_F(ILoggerTest, GetLastMessage_ReturnsEmptyIfNoEntries) {
    EXPECT_TRUE(mock_logger_->getLastMessage().empty());
}

TEST_F(ILoggerTest, ContainsMessage_FindsSubstring) {
    mock_logger_->info("This is a test message with Track ID 1001");
    
    EXPECT_TRUE(mock_logger_->containsMessage("Track ID"));
    EXPECT_TRUE(mock_logger_->containsMessage("1001"));
    EXPECT_FALSE(mock_logger_->containsMessage("not found"));
}

// ============================================
// Multiple Messages Tests
// ============================================

TEST_F(ILoggerTest, MultipleLogs_CapturedInOrder) {
    mock_logger_->info("first");
    mock_logger_->warn("second");
    mock_logger_->error("third");
    
    auto entries = mock_logger_->getEntries();
    
    ASSERT_EQ(entries.size(), 3);
    EXPECT_EQ(entries[0].message, "first");
    EXPECT_EQ(entries[1].message, "second");
    EXPECT_EQ(entries[2].message, "third");
    
    EXPECT_EQ(entries[0].level, ILogger::Level::Info);
    EXPECT_EQ(entries[1].level, ILogger::Level::Warn);
    EXPECT_EQ(entries[2].level, ILogger::Level::Error);
}
