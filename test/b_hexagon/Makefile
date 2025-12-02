PSUEODAPPNAME = b_hexagon
SRC_DIR = src/$(PSUEODAPPNAME)

default: all

all:
	@$(MAKE) -f ./makefile_white.mk exec --no-print-directory SRC_DIR=$(SRC_DIR) PSUEODAPPNAME=$(PSUEODAPPNAME)

.PHONY: clean
clean:
	@$(MAKE) -f ./makefile_white.mk clean SRC_DIR:=$(SRC_DIR) --no-print-directory 

# Test targets
.PHONY: test test-clean

test:
	@echo "Building and running tests..."
	@$(MAKE) -C test/b_hexagon all
	@./test/b_hexagon/build/b_hexagon_tests

test-clean:
	@$(MAKE) -C test/b_hexagon clean

# Help target
.PHONY: help

help:
	@echo "Available targets:"
	@echo "  all        - Build the application (default)"
	@echo "  clean      - Clean build artifacts"
	@echo "  test       - Build and run unit tests"
	@echo "  test-clean - Clean test build artifacts"
	@echo "  help       - Show this help message"
