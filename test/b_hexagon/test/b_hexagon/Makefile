CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O0 -fPIC -DSPDLOG_FMT_EXTERNAL=0 -DZMQ_BUILD_DRAFT_API

# Library directories
APP_LIB_DIR = ../../lib
TEST_LIB_DIR = lib

# Include paths - project headers + test framework headers
INCLUDES = -I../../src/b_hexagon -I../../src -I$(APP_LIB_DIR)/include -I$(TEST_LIB_DIR)/include -I. -I../..

# Link flags - application libs from ../../lib, test libs from ./lib
LDFLAGS = -L$(APP_LIB_DIR) -L$(TEST_LIB_DIR) \
          -Wl,-rpath,'$$ORIGIN/../../../lib' -Wl,-rpath,'$$ORIGIN/../lib' \
          -lspdlog -lzmq \
          -lgtest -lgtest_main -lgmock \
          -lpthread

TARGET = build/b_hexagon_tests

# Source directories
SRC_DIR = ../../src/b_hexagon

# Domain model source files (need to be linked)
DOMAIN_SOURCES = $(SRC_DIR)/domain/model/ExtrapTrackData.cpp \
                 $(SRC_DIR)/domain/model/DelayCalcTrackData.cpp \
                 $(SRC_DIR)/domain/logic/ProcessTrackUseCase.cpp \
                 $(SRC_DIR)/domain/logic/CalculatorService.cpp \
                 $(SRC_DIR)/adapters/common/messaging/ZeroMQSocket.cpp \
                 $(SRC_DIR)/adapters/incoming/zeromq/ExtrapTrackDataZeroMQIncomingAdapter.cpp \
                 $(SRC_DIR)/adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.cpp

# Test source files
TEST_SOURCES = main_test.cpp \
               domain/logic/ProcessTrackUseCaseTest.cpp \
               domain/logic/CalculatorServiceTest.cpp \
               domain/model/ExtrapTrackDataTest.cpp \
               domain/model/DelayCalcTrackDataTest.cpp \
               domain/model/SerializationEdgeCaseTest.cpp \
               adapters/common/AdapterManagerTest.cpp \
               adapters/common/ZeroMQSocketTest.cpp \
               adapters/incoming/ExtrapTrackDataZeroMQIncomingAdapterTest.cpp \
               adapters/outgoing/DelayCalcTrackDataZeroMQOutgoingAdapterTest.cpp \
               adapters/ErrorPathTest.cpp \
               utils/LoggerTest.cpp

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
