# Root Makefile for all hexagon projects
.PHONY: all clean rebuild a_hexagon b_hexagon hexagon_c help

all: a_hexagon b_hexagon hexagon_c

a_hexagon:
	@echo "🔨 Building a_hexagon..."
	$(MAKE) -C a_hexagon/a_hexagon

b_hexagon:
	@echo "🔨 Building b_hexagon..."
	$(MAKE) -C b_hexagon/b_hexagon

hexagon_c:
	@echo "🔨 Building hexagon_c..."
	$(MAKE) -C hexagon_c/hexagon_c

clean:
	@echo "🧹 Cleaning all projects..."
	$(MAKE) -C a_hexagon/a_hexagon clean
	$(MAKE) -C b_hexagon/b_hexagon clean
	$(MAKE) -C hexagon_c/hexagon_c clean
	@echo "✅ All projects cleaned"

rebuild: clean all

help:
	@echo "🎯 Hexagon Projects Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build all projects (default)"
	@echo "  a_hexagon  - Build only a_hexagon"
	@echo "  b_hexagon  - Build only b_hexagon" 
	@echo "  hexagon_c  - Build only hexagon_c"
	@echo "  clean      - Clean all projects"
	@echo "  rebuild    - Clean and build all projects"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "📁 Project Structure & Executables:"
	@echo "  a_hexagon  - 8Hz→200Hz ExtrapTrackData generator     → executable: a_hexagon"
	@echo "  b_hexagon  - DelayCalcTrackData processor            → executable: b_hexagon" 
	@echo "  hexagon_c  - FinalCalcTrackData analyzer (Hexagonal) → executable: c_hexagon"