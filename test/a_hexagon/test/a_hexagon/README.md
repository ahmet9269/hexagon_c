# a_hexagon Unit Tests

GTest based unit test suite for a_hexagon project.

## Directory Structure

```
test/a_hexagon/
├── CMakeLists.txt              # CMake build configuration
├── Makefile                    # Make build configuration
├── main_test.cpp               # GTest entry point
├── adapters/
│   └── common/
│       └── AdapterManagerTest.cpp
├── domain/
│   ├── logic/
│   │   └── TrackDataExtrapolatorTest.cpp
│   └── model/
│       ├── TrackDataTest.cpp
│       └── ExtrapTrackDataTest.cpp
└── utils/
    └── LoggerTest.cpp
```

## Prerequisites

```bash
# Install GTest and GMock
sudo apt-get update
sudo apt-get install -y libgtest-dev libgmock-dev cmake

# Build GTest libraries (if needed)
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib/
```

## Building Tests

### Using Make
```bash
cd test/a_hexagon
make all
```

### Using CMake
```bash
cd test/a_hexagon
mkdir build && cd build
cmake ..
make
```

## Running Tests

### Run All Tests
```bash
make test
# or
./build/a_hexagon_tests
```

### Run Specific Test Suite
```bash
./build/a_hexagon_tests --gtest_filter=TrackDataTest.*
./build/a_hexagon_tests --gtest_filter=LoggerTest.*
./build/a_hexagon_tests --gtest_filter=AdapterManagerTest.*
```

### Run with Verbose Output
```bash
./build/a_hexagon_tests --gtest_output=xml:results.xml
```

## Test Categories

### Domain Model Tests
- **TrackDataTest**: Tests for TrackData model class
  - Constructor tests
  - Getter/Setter tests
  - Serialization/Deserialization tests
  - Validation tests
  - Boundary value tests

- **ExtrapTrackDataTest**: Tests for ExtrapTrackData model class
  - Timing field tests
  - Position/Velocity tests
  - Serialization round-trip tests

### Domain Logic Tests
- **TrackDataExtrapolatorTest**: Tests for extrapolation service
  - Constructor tests (smart pointer, move semantics)
  - Extrapolation calculation tests
  - Frequency calculation tests
  - Mock integration tests

### Adapter Tests
- **AdapterManagerTest**: Tests for adapter lifecycle management
  - Add/Remove adapter tests
  - Start/Stop tests
  - Thread safety tests
  - State management tests

### Utility Tests
- **LoggerTest**: Tests for async logging utility
  - Initialization tests
  - Log level tests
  - Thread safety tests
  - Performance tests
  - Macro tests

## MISRA C++ 2023 Compliance

All tests follow MISRA C++ 2023 guidelines:
- Fixed-width integers (`int32_t`, `int64_t`)
- `[[nodiscard]]` attributes
- `noexcept` specifications
- Brace initialization
- Smart pointer usage

## Coverage

To generate coverage report:
```bash
# Build with coverage flags
make CXXFLAGS="-std=c++17 -g -O0 --coverage"

# Run tests
make test

# Generate report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## Author
a_hexagon Team - 2025
