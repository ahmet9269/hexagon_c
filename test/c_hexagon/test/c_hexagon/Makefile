CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O0 -fPIC -DSPDLOG_FMT_EXTERNAL=0 -DZMQ_BUILD_DRAFT_API

# Library directories
APP_LIB_DIR = ../../lib
TEST_LIB_DIR = lib

# Include paths - project headers + test framework headers
INCLUDES = -I../../src/c_hexagon -I../../src -I$(APP_LIB_DIR)/include -I$(TEST_LIB_DIR)/include -I. -I../..

# Link flags - application libs from ../../lib, test libs from ./lib
LDFLAGS = -L$(APP_LIB_DIR) -L$(TEST_LIB_DIR) \
          -Wl,-rpath,'$$ORIGIN/../../../lib' -Wl,-rpath,'$$ORIGIN/../lib' \
          -lspdlog -lzmq \
          -lgtest -lgtest_main -lgmock \
          -lpthread

TARGET = build/c_hexagon_tests

# Source directories
SRC_DIR = ../../src/c_hexagon

# Domain model source files (need to be linked)
DOMAIN_SOURCES = $(SRC_DIR)/domain/model/DelayCalcTrackData.cpp \
                 $(SRC_DIR)/domain/model/FinalCalcTrackData.cpp \
                 $(SRC_DIR)/domain/logic/FinalCalculationService.cpp \
                 $(SRC_DIR)/adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.cpp \
                 $(SRC_DIR)/adapters/outgoing/zeromq/FinalCalcTrackDataZeroMQOutgoingAdapter.cpp

# Test source files
TEST_SOURCES = main_test.cpp \
               domain/logic/FinalCalculationServiceTest.cpp \
               domain/model/FinalCalcTrackDataTest.cpp \
               domain/model/DelayCalcTrackDataTest.cpp \
               domain/ports/MockPortsTest.cpp \
               adapters/common/AdapterManagerTest.cpp \
               adapters/incoming/TrackDataZeroMQIncomingAdapterTest.cpp \
               adapters/outgoing/FinalCalcTrackDataZeroMQOutgoingAdapterTest.cpp \
               utils/LoggerTest.cpp \
               utils/ILoggerTest.cpp

# Object files
DOMAIN_OBJS = $(DOMAIN_SOURCES:.cpp=.o)
TEST_OBJS = $(TEST_SOURCES:.cpp=.o)

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): $(TEST_OBJS) $(DOMAIN_OBJS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_OBJS) $(DOMAIN_OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test: $(TARGET)
	LD_LIBRARY_PATH=$(APP_LIB_DIR):$(TEST_LIB_DIR):$(LD_LIBRARY_PATH) ./$(TARGET)

run: test

clean:
	rm -f $(TEST_OBJS) $(DOMAIN_OBJS)
	rm -rf build
	find . -name "*.o" -type f -delete

help:
	@echo "Available targets:"
	@echo "  all    - Build all tests (default)"
	@echo "  test   - Build and run all tests"
	@echo "  run    - Alias for test"
	@echo "  clean  - Remove all build artifacts"
	@echo "  help   - Show this help message"
