CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O0 -fPIC -DSPDLOG_FMT_EXTERNAL=0
INCLUDES = -I../../src/a_hexagon -I../../src -I/usr/include -I../../lib/include
LDFLAGS = -L../../lib -Wl,-rpath,../../lib -lspdlog -lgtest -lgtest_main -lgmock -lpthread
TARGET = build/a_hexagon_tests

# Source directories
SRC_DIR = ../../src/a_hexagon

# Domain model source files (need to be linked)
DOMAIN_SOURCES = $(SRC_DIR)/domain/model/TrackData.cpp \
                 $(SRC_DIR)/domain/model/ExtrapTrackData.cpp \
                 $(SRC_DIR)/domain/logic/TrackDataExtrapolator.cpp

# Test source files
TEST_SOURCES = main_test.cpp \
               adapters/common/AdapterManagerTest.cpp \
               utils/LoggerTest.cpp \
               domain/model/TrackDataTest.cpp \
               domain/model/ExtrapTrackDataTest.cpp \
               domain/logic/TrackDataExtrapolatorTest.cpp

# Object files
DOMAIN_OBJS = $(DOMAIN_SOURCES:.cpp=.o)
TEST_OBJS = $(TEST_SOURCES:.cpp=.o)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(TEST_OBJS) $(DOMAIN_OBJS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(TEST_OBJS) $(DOMAIN_OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test: $(TARGET)
	LD_LIBRARY_PATH=../../lib:$(LD_LIBRARY_PATH) ./$(TARGET)

clean:
	rm -f $(TEST_OBJS) $(DOMAIN_OBJS)
	rm -rf build
