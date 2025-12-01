PSUEODAPPNAME = a_hexagon
SRC_DIR = src/$(PSUEODAPPNAME)

default: all

all:
	@$(MAKE) -f ./makefile_white.mk exec --no-print-directory SRC_DIR=$(SRC_DIR) PSUEODAPPNAME=$(PSUEODAPPNAME)

.PHONY: clean
clean:
	@$(MAKE) -f ./makefile_white.mk clean SRC_DIR:=$(SRC_DIR) --no-print-directory 

# Documentation targets
.PHONY: docs docs-clean docs-open

docs:
	@echo "Generating Doxygen documentation..."
	@chmod +x ./build_docs.sh
	@./build_docs.sh

docs-clean:
	@echo "Cleaning documentation..."
	@./build_docs.sh --clean

docs-open:
	@echo "Generating and opening documentation..."
	@chmod +x ./build_docs.sh
	@./build_docs.sh --open

# Test targets
.PHONY: test test-clean

test:
	@echo "Building and running tests..."
	@$(MAKE) -C test/a_hexagon all
	@./test/a_hexagon/build/a_hexagon_tests

test-clean:
	@$(MAKE) -C test/a_hexagon clean

# Help target
.PHONY: help

help:
	@echo "Available targets:"
	@echo "  all        - Build the application (default)"
	@echo "  clean      - Clean build artifacts"
	@echo "  docs       - Generate Doxygen documentation"
	@echo "  docs-clean - Clean and regenerate documentation"
	@echo "  docs-open  - Generate docs and open in browser"
	@echo "  test       - Build and run unit tests"
	@echo "  test-clean - Clean test build artifacts"
	@echo "  help       - Show this help message"
