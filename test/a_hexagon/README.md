# a_hexagon - Hexagonal Architecture Track Data Processing

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/std/the-standard)
[![MISRA C++ 2023](https://img.shields.io/badge/MISRA%20C%2B%2B-2023-green.svg)](https://www.misra.org.uk/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## Overview

**a_hexagon** is a high-performance track data processing application built using Hexagonal Architecture (Ports and Adapters pattern). It receives track data at 8Hz via ZeroMQ DISH sockets, performs linear extrapolation, and outputs at 100Hz via RADIO sockets.

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        a_hexagon Application                     │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐     ┌──────────────────┐     ┌─────────────┐   │
│  │   Incoming  │     │      Domain      │     │  Outgoing   │   │
│  │   Adapter   │────▶│      Logic       │────▶│   Adapter   │   │
│  │  (ZeroMQ)   │     │  (Extrapolator)  │     │  (ZeroMQ)   │   │
│  └─────────────┘     └──────────────────┘     └─────────────┘   │
│        │                      │                      │          │
│        ▼                      ▼                      ▼          │
│  ┌─────────────┐     ┌──────────────────┐     ┌─────────────┐   │
│  │    Port     │     │     Model        │     │    Port     │   │
│  │ (TrackData) │     │ (ExtrapTrackData)│     │(ExtrapTrack)│   │
│  └─────────────┘     └──────────────────┘     └─────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

## Features

- **Thread-per-Type Architecture**: Each message type processed in dedicated thread
- **8Hz → 100Hz Extrapolation**: Linear interpolation for position prediction
- **ZeroMQ DISH/RADIO**: UDP multicast for real-time communication
- **MISRA C++ 2023 Compliant**: Safety-critical coding standards
- **Binary Serialization**: Efficient network data transfer
- **Async Logging**: spdlog-based non-blocking logging

## Directory Structure

```
test/a_hexagon/
├── Doxyfile                    # Doxygen configuration
├── Makefile                    # Main build file
├── makefile_white.mk           # Build rules
├── build_docs.sh               # Documentation generator script
├── README.md                   # This file
├── doc/                        # Generated documentation
│   └── html/                   # HTML documentation
├── lib/                        # External libraries
│   ├── libzmq.so              # ZeroMQ library
│   ├── libspdlog.so           # spdlog library
│   └── include/               # Header files
├── src/
│   └── a_hexagon/
│       ├── main.cpp           # Application entry point
│       ├── adapters/          # Hexagonal adapters
│       │   ├── common/        # Common adapter utilities
│       │   ├── incoming/      # Input adapters (DISH)
│       │   └── outgoing/      # Output adapters (RADIO)
│       ├── domain/            # Business logic
│       │   ├── logic/         # TrackDataExtrapolator
│       │   ├── model/         # TrackData, ExtrapTrackData
│       │   └── ports/         # Port interfaces
│       └── utils/             # Utilities (Logger)
└── test/
    └── a_hexagon/             # Unit tests (GTest)
        ├── Makefile
        ├── main_test.cpp
        ├── adapters/
        ├── domain/
        └── utils/
```

## Building

### Prerequisites

- GCC 8.5+ with C++17 support
- ZeroMQ 4.3+ (with DRAFT API enabled)
- spdlog 1.12+
- Google Test (for unit tests)
- Doxygen + Graphviz (for documentation)

### Build Application

```bash
cd test/a_hexagon
make clean
make all
```

### Build Tests

```bash
cd test/a_hexagon/test/a_hexagon
make clean
make all
./build/a_hexagon_tests
```

### Build Documentation

```bash
cd test/a_hexagon
./build_docs.sh          # Generate docs
./build_docs.sh -o       # Generate and open in browser
./build_docs.sh -c       # Clean and regenerate
```

## Running

```bash
cd test/a_hexagon/bin
LD_LIBRARY_PATH=../lib ./a_hexagon
```

## Configuration

### Socket Configuration

| Parameter | TrackData (Input) | ExtrapTrackData (Output) |
|-----------|-------------------|--------------------------|
| Socket Type | DISH (Subscriber) | RADIO (Publisher) |
| Multicast Address | 239.1.1.1 | 239.1.1.2 |
| Port | 9000 | 9001 |
| Protocol | UDP | UDP |

### Extrapolation Parameters

| Parameter | Value |
|-----------|-------|
| Input Frequency | 8 Hz |
| Output Frequency | 100 Hz |
| Extrapolation Method | Linear |
| Samples per Input | 12-13 |

## Testing

### Unit Tests (115 tests)

| Test Suite | Tests | Description |
|------------|-------|-------------|
| AdapterManagerTest | 16 | Adapter lifecycle, registration |
| LoggerTest | 34 | spdlog integration, thread safety |
| TrackDataTest | 24 | Model validation, serialization |
| ExtrapTrackDataTest | 22 | Timing fields, validation |
| TrackDataExtrapolatorTest | 19 | Extrapolation logic |

### Run Specific Test

```bash
./build/a_hexagon_tests --gtest_filter=LoggerTest.*
./build/a_hexagon_tests --gtest_filter=*Serialize*
```

## API Documentation

Generate and view documentation:

```bash
./build_docs.sh -o
```

Or view directly: `doc/html/index.html`

## MISRA C++ 2023 Compliance

This project follows MISRA C++ 2023 guidelines:

- ✅ Strong typing with `int32_t`, `int64_t`
- ✅ `noexcept` specifications
- ✅ `[[nodiscard]]` attributes
- ✅ Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- ✅ RAII resource management
- ✅ Explicit constructors
- ✅ No raw `new`/`delete`

## License

MIT License - See [LICENSE](LICENSE) file.

## Authors

- a_hexagon Development Team

## See Also

- [b_hexagon](../../b_hexagon/) - Related hexagonal architecture project
- [c_hexagon](../../hexagon_c/) - C implementation reference
