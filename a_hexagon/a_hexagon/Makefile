# Makefile for a_hexagon project
CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -mtune=native -flto -ffast-math -DNDEBUG -pthread
INCLUDES = -Isrc -I../../hexagon_c/libzmq/include -I../../hexagon_c/include -DZMQ_BUILD_DRAFT_API

# Source files
SRCDIR = src
SOURCES = $(SRCDIR)/application/main.cpp \
          $(SRCDIR)/domain/logic/TrackDataExtrapolator.cpp \
          $(SRCDIR)/domain/model/ExtrapTrackData.cpp \
          $(SRCDIR)/domain/model/TrackData.cpp \
          $(SRCDIR)/utilities/JsonConfigParser.cpp \
          $(SRCDIR)/adapters/outgoing/ZeroMQExtrapTrackDataAdapter.cpp

# Libraries
LIBZMQ = ../../hexagon_c/libzmq/build/lib/libzmq.a
LIBS = $(LIBZMQ) -lsodium -lrt -lgnutls

# Target executable
TARGET = a_hexagon

.PHONY: all clean rebuild help

all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo "🔨 Building a_hexagon..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) $(LIBS) -o $(TARGET)
	@echo "✅ a_hexagon built successfully: $(TARGET)"

clean:
	@echo "🧹 Cleaning a_hexagon..."
	rm -f $(TARGET) a_hexagon_app* a_hexagon_optimized* a_hexagon_updated*
	@echo "✅ Clean completed"

rebuild: clean all

help:
	@echo "🎯 A_Hexagon Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build the application (default)"
	@echo "  clean   - Remove executable"
	@echo "  rebuild - Clean and build"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "📁 Function: 8Hz TrackData → 200Hz ExtrapTrackData generator"
