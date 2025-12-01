# C_Hexagon Makefile - Final Track Data Processing System
# Architecture: Hexagonal (Ports & Adapters)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG -pthread
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -DNDEBUG

# ZeroMQ configuration
ZMQ_CFLAGS = -DZMQ_BUILD_DRAFT_API
ZMQ_LIB_PATH = ../libzmq/build/lib/libzmq.a

# Project structure
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Target executables
TARGET_RELEASE = $(BUILD_DIR)/hexagon_c_app_optimized
TARGET_SIMPLE = c_hexagon

# Dynamically find all source files
SOURCES = $(shell find $(SRC_DIR) -name "*.cpp" -type f)

# Generate object file paths dynamically (maintain directory structure)
OBJECTS_RELEASE = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Include paths
INCLUDES = -I$(SRC_DIR) \
          -I../libzmq/include \
          -I../include

# Libraries
LIBS = $(ZMQ_LIB_PATH) -lsodium -lrt -lgnutls

# Default target
all: release

# Simple build (like old version)
simple: $(TARGET_SIMPLE)

# Release build with organized structure
release: $(TARGET_RELEASE)

# Simple executable (backward compatibility)
$(TARGET_SIMPLE): $(SOURCES)
	@echo "🔨 Building c_hexagon (simple)..."
	$(CXX) $(CXXFLAGS) $(ZMQ_CFLAGS) $(INCLUDES) $(SOURCES) $(LIBS) -o $(TARGET_SIMPLE)
	@echo "✅ c_hexagon built successfully: $(TARGET_SIMPLE)"

# Release executable with organized build
$(TARGET_RELEASE): $(OBJECTS_RELEASE) | $(BUILD_DIR)
	@echo "🔗 Linking release executable: $@"
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $^ -o $@ $(LIBS)
	@echo "✅ Release build completed: $@"

# Release object files (maintain directory structure)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "🔨 Compiling: $< → $@"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(ZMQ_CFLAGS) $(INCLUDES) -c $< -o $@

# Create build directories
$(BUILD_DIR):
	@echo "📁 Creating build directory: $@"
	@mkdir -p $@

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(TARGET_SIMPLE) hexagon_c_app* c_hexagon_* hexagon_c_optimized* hexagon_c_updated* hexagon_c_binary*
	@echo "✅ Clean completed"

# Clean and rebuild
rebuild: clean all

# Show project info
info:
	@echo "=== C_Hexagon Project Information ==="
	@echo "Architecture: Hexagonal (Ports & Adapters)"
	@echo "Language: C++17"
	@echo "Build System: Make (Dynamic Source Discovery)"
	@echo "ZeroMQ: DRAFT API enabled (DISH/RADIO)"
	@echo ""
	@echo "Found source files:"
	@$(foreach src,$(SOURCES),echo "  📄 $(src)";)

# List all source files found
list-sources:
	@echo "=== Dynamically Found Source Files ==="
	@find $(SRC_DIR) -name "*.cpp" -type f | sort

# Show build status
status:
	@echo "=== Build Status ==="
	@if [ -f "$(TARGET_SIMPLE)" ]; then \
		echo "✅ Simple executable: $(TARGET_SIMPLE)"; \
		ls -la $(TARGET_SIMPLE); \
	else \
		echo "❌ Simple executable not found"; \
	fi
	@if [ -f "$(TARGET_RELEASE)" ]; then \
		echo "✅ Release executable: $(TARGET_RELEASE)"; \
		ls -la $(TARGET_RELEASE); \
	else \
		echo "❌ Release executable not found"; \
	fi

# Help target
help:
	@echo "=== C_Hexagon Makefile Help ==="
	@echo "Available targets:"
	@echo "  all           - Build release version (default)"
	@echo "  simple        - Build simple version (backward compatibility)"
	@echo "  release       - Build optimized release with organized structure"
	@echo "  clean         - Remove all build artifacts"
	@echo "  rebuild       - Clean and rebuild"
	@echo "  info          - Show project information"
	@echo "  list-sources  - List all found .cpp files"
	@echo "  status        - Show build status"
	@echo "  help          - Show this help"

# Phony targets
.PHONY: all simple release clean rebuild info list-sources status help