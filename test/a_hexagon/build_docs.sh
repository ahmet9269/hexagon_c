#!/bin/bash
#
# build_docs.sh - Doxygen documentation generator for a_hexagon project
# 
# Usage: ./build_docs.sh [options]
# Options:
#   -c, --clean     Clean documentation before building
#   -o, --open      Open documentation in browser after building
#   -q, --quiet     Suppress output
#   -h, --help      Show this help message
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DOC_DIR="${SCRIPT_DIR}/doc"
DOXYFILE="${SCRIPT_DIR}/Doxyfile"

# Default options
CLEAN=false
OPEN_DOCS=false
QUIET=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -o|--open)
            OPEN_DOCS=true
            shift
            ;;
        -q|--quiet)
            QUIET=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  -c, --clean     Clean documentation before building"
            echo "  -o, --open      Open documentation in browser after building"
            echo "  -q, --quiet     Suppress output"
            echo "  -h, --help      Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

# Function to print messages
print_msg() {
    if [ "$QUIET" = false ]; then
        echo -e "$1"
    fi
}

# Check if doxygen is installed
if ! command -v doxygen &> /dev/null; then
    print_msg "${RED}Error: doxygen is not installed.${NC}"
    print_msg "Install with: sudo apt-get install doxygen doxygen-doc graphviz"
    exit 1
fi

# Check if graphviz is installed (for diagrams)
if ! command -v dot &> /dev/null; then
    print_msg "${YELLOW}Warning: graphviz (dot) is not installed. Diagrams will not be generated.${NC}"
    print_msg "Install with: sudo apt-get install graphviz"
fi

# Check if Doxyfile exists
if [ ! -f "$DOXYFILE" ]; then
    print_msg "${RED}Error: Doxyfile not found at ${DOXYFILE}${NC}"
    exit 1
fi

print_msg "${BLUE}========================================${NC}"
print_msg "${BLUE}  a_hexagon Documentation Generator${NC}"
print_msg "${BLUE}========================================${NC}"

# Clean documentation if requested
if [ "$CLEAN" = true ]; then
    print_msg "${YELLOW}Cleaning existing documentation...${NC}"
    rm -rf "${DOC_DIR}/html"
    rm -rf "${DOC_DIR}/latex"
    rm -f "${DOC_DIR}/doxygen_warnings.log"
    rm -f "${DOC_DIR}/a_hexagon.tag"
    print_msg "${GREEN}Documentation cleaned.${NC}"
fi

# Create doc directories if they don't exist
mkdir -p "${DOC_DIR}/images"

# Generate documentation
print_msg "${BLUE}Generating documentation...${NC}"
cd "$SCRIPT_DIR"

if [ "$QUIET" = true ]; then
    doxygen "$DOXYFILE" > /dev/null 2>&1
else
    doxygen "$DOXYFILE"
fi

# Check if documentation was generated successfully
if [ -f "${DOC_DIR}/html/index.html" ]; then
    print_msg "${GREEN}========================================${NC}"
    print_msg "${GREEN}  Documentation generated successfully!${NC}"
    print_msg "${GREEN}========================================${NC}"
    print_msg ""
    print_msg "Documentation location: ${DOC_DIR}/html/index.html"
    
    # Check for warnings
    if [ -f "${DOC_DIR}/doxygen_warnings.log" ]; then
        WARN_COUNT=$(wc -l < "${DOC_DIR}/doxygen_warnings.log")
        if [ "$WARN_COUNT" -gt 0 ]; then
            print_msg "${YELLOW}Warnings: ${WARN_COUNT} (see ${DOC_DIR}/doxygen_warnings.log)${NC}"
        fi
    fi
    
    # Open in browser if requested
    if [ "$OPEN_DOCS" = true ]; then
        print_msg "${BLUE}Opening documentation in browser...${NC}"
        if command -v xdg-open &> /dev/null; then
            xdg-open "${DOC_DIR}/html/index.html" &
        elif command -v open &> /dev/null; then
            open "${DOC_DIR}/html/index.html"
        elif [ -n "$BROWSER" ]; then
            "$BROWSER" "${DOC_DIR}/html/index.html" &
        else
            print_msg "${YELLOW}Could not detect browser. Please open manually:${NC}"
            print_msg "${DOC_DIR}/html/index.html"
        fi
    fi
else
    print_msg "${RED}Error: Documentation generation failed!${NC}"
    exit 1
fi

print_msg ""
print_msg "To view documentation:"
print_msg "  Open: ${DOC_DIR}/html/index.html"
print_msg ""
