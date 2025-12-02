#!/bin/bash
# Run script for b_hexagon
# Sets LD_LIBRARY_PATH to include local lib directory

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"

echo "Starting b_hexagon..."
echo "LD_LIBRARY_PATH: ${LD_LIBRARY_PATH}"

exec "${SCRIPT_DIR}/bin/b_hexagon" "$@"
