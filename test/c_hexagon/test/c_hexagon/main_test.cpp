/**
 * @file main_test.cpp
 * @brief Test runner main entry point
 */

#include <gtest/gtest.h>
#include "utils/Logger.hpp"

int main(int argc, char** argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Initialize logger for tests (optional, some tests do this themselves)
    // utils::Logger::init("c_hexagon_tests");
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    // Cleanup
    // utils::Logger::shutdown();
    
    return result;
}
