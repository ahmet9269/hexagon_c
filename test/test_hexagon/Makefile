# test_hexagon - Simple TrackData Publisher
# Sends TrackData to a_hexagon via ZeroMQ RADIO socket

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Directories
SRC_DIR = src
BIN_DIR = bin
LIB_DIR = lib

# Include paths (local lib headers first)
INCLUDES = -I$(LIB_DIR)/include -I$(SRC_DIR)

# Library paths and libraries
LDFLAGS = -L$(LIB_DIR) -Wl,-rpath,'$$ORIGIN/../lib'
LIBS = -lzmq -lpthread

# Target
TARGET = $(BIN_DIR)/test_hexagon

# Source files
SRCS = $(SRC_DIR)/main.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(SRCS) $(LDFLAGS) $(LIBS)
	@echo "Build complete: $(TARGET)"

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

run: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ./$(TARGET)

# Run with custom message count
run100: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ./$(TARGET) 100

run500: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ./$(TARGET) 500

# Infinite mode (Ctrl+C to stop)
infinite: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ./$(TARGET) 0
