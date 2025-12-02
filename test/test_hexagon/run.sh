#!/bin/bash
# Run test_hexagon publisher

cd "$(dirname "$0")"

# Build if needed
if [ ! -f bin/test_hexagon ]; then
    echo "Building test_hexagon..."
    make
fi

# Default: send 100 messages
COUNT=${1:-100}

echo "Starting test_hexagon publisher (count=$COUNT)..."
LD_LIBRARY_PATH=lib:$LD_LIBRARY_PATH ./bin/test_hexagon $COUNT
