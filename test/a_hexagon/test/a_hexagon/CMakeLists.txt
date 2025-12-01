# CMakeLists.txt for a_hexagon unit tests
# GTest based testing framework

cmake_minimum_required(VERSION 3.14)
project(a_hexagon_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GTest
find_package(GTest REQUIRED)
include(GoogleTest)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../../src/a_hexagon
    ${CMAKE_SOURCE_DIR}/../../src
    ${GTEST_INCLUDE_DIRS}
)

# Source files from main project (for testing)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/../../src/a_hexagon)

# Test source files
set(TEST_SOURCES
    domain/model/TrackDataTest.cpp
    domain/model/ExtrapTrackDataTest.cpp
    domain/logic/TrackDataExtrapolatorTest.cpp
    adapters/common/AdapterManagerTest.cpp
    utils/LoggerTest.cpp
    main_test.cpp
)

# Create test executable
add_executable(a_hexagon_tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(a_hexagon_tests
    GTest::GTest
    GTest::Main
    pthread
)

# Enable testing
enable_testing()
gtest_discover_tests(a_hexagon_tests)
