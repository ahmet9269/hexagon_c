/**
 * @file main_test.cpp
 * @brief GTest main entry point for b_hexagon unit tests
 * @details Initializes Google Test framework and runs all registered tests.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
