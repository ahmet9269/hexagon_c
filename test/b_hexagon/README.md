# B_Hexagon - Track Data Processor

## Overview

B_Hexagon is a high-performance **Track Data Processor** built with **Thread-per-Type Hexagonal Architecture**. It receives `ExtrapTrackData` via ZeroMQ DISH socket, calculates timing delays, and broadcasts `DelayCalcTrackData` via ZeroMQ RADIO socket.

## Architecture

### Hexagonal Architecture (Ports & Adapters)

```
┌─────────────────────────────────────────────────────────────────────┐
│                    B_HEXAGON ARCHITECTURE                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌────────────────────────────────────────────────────────────┐    │
│  │              ADAPTERS (Infrastructure Layer)               │    │
│  │  ┌─────────────────────┐   ┌─────────────────────────┐    │    │
│  │  │   Incoming Adapter  │   │   Outgoing Adapter      │    │    │
│  │  │   (ZeroMQ DISH)     │   │   (ZeroMQ RADIO)        │    │    │
│  │  │   UDP: 239.1.1.2    │   │   TCP: localhost:15002  │    │    │
│  │  └──────────┬──────────┘   └──────────▲──────────────┘    │    │
│  └─────────────┼─────────────────────────┼───────────────────┘    │
│                │                         │                         │
│  ┌─────────────▼─────────────────────────┼───────────────────┐    │
│  │              DOMAIN (Business Logic Layer)                 │    │
│  │  ┌─────────────────────────────────────────────────────┐  │    │
│  │  │          ProcessTrackUseCase                        │  │    │
│  │  │  ┌───────────────────────────────────────────────┐  │  │    │
│  │  │  │         CalculatorService                     │  │  │    │
│  │  │  │  • calculateDelay(ExtrapTrackData)            │  │  │    │
│  │  │  │  • getCurrentTimeMicroseconds()               │  │  │    │
│  │  │  │  • calculateTimeDelta()                       │  │  │    │
│  │  │  └───────────────────────────────────────────────┘  │  │    │
│  │  └─────────────────────────────────────────────────────┘  │    │
│  └────────────────────────────────────────────────────────────┘    │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### Thread-per-Type Pattern

Each message type has its own isolated processing pipeline:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ ExtrapTrackData │    │ ProcessedData   │    │ SensorData      │
├─────────────────┤    ├─────────────────┤    ├─────────────────┤
│ InAdapter (T1)  │    │ InAdapter (T2)  │    │ InAdapter (T3)  │
│       ↓         │    │       ↓         │    │       ↓         │
│ DomainService   │    │ DomainService   │    │ DomainService   │
│       ↓         │    │       ↓         │    │       ↓         │
│ OutAdapter (T1) │    │ OutAdapter (T2) │    │ OutAdapter (T3) │
└─────────────────┘    └─────────────────┘    └─────────────────┘
     ISOLATED              ISOLATED              ISOLATED
```

## Features

- **SOLID Principles**: Full compliance with SOLID design principles
- **Dependency Inversion**: All dependencies are abstracted for testability
- **Thread Safety**: Each adapter runs in a dedicated thread
- **High Performance**: ZeroMQ for high-throughput messaging
- **MISRA C++ 2023**: Compliant implementation
- **Scalable**: Supports 20+ concurrent adapters

## Directory Structure

```
b_hexagon/
├── src/b_hexagon/
│   ├── main.cpp                  # Application entry point
│   ├── adapters/                 # Infrastructure layer
│   │   ├── common/
│   │   │   ├── IAdapter.hpp      # Base interface
│   │   │   ├── AdapterManager.hpp
│   │   │   ├── MessagePipeline.hpp
│   │   │   └── messaging/
│   │   │       ├── IMessageSocket.hpp
│   │   │       └── ZeroMQSocket.hpp
│   │   ├── incoming/zeromq/
│   │   │   └── ExtrapTrackDataZeroMQIncomingAdapter.hpp
│   │   └── outgoing/zeromq/
│   │       └── DelayCalcTrackDataZeroMQOutgoingAdapter.hpp
│   ├── domain/
│   │   ├── logic/
│   │   │   ├── ICalculatorService.hpp
│   │   │   ├── CalculatorService.hpp
│   │   │   └── ProcessTrackUseCase.hpp
│   │   └── ports/
│   │       ├── ExtrapTrackData.hpp
│   │       ├── DelayCalcTrackData.hpp
│   │       ├── incoming/
│   │       │   └── IExtrapTrackDataIncomingPort.hpp
│   │       └── outgoing/
│   │           └── IDelayCalcTrackDataOutgoingPort.hpp
│   └── utils/
│       └── Logger.hpp            # spdlog wrapper
├── test/                         # Unit tests
├── doc/                          # Documentation
│   └── html/                     # Generated Doxygen docs
├── Doxyfile                      # Doxygen configuration
├── Makefile                      # Build configuration
└── README.md                     # This file
```

## Building

### Prerequisites

- C++17 compatible compiler (GCC 9+ / Clang 10+)
- ZeroMQ library with Draft API enabled
- spdlog library

### Compile

```bash
cd test/b_hexagon
make clean
make
```

### Build Documentation

```bash
doxygen Doxyfile
```

Documentation will be generated in `docs/html/`.

## Running

```bash
./run.sh
# or directly
./bin/b_hexagon
```

### Expected Output

```
[2025-12-03 10:00:00.000] [info] === B_Hexagon Track Processing System Starting ===
[2025-12-03 10:00:00.001] [info] Architecture: Thread-per-Type (Scalable to 20+ adapters)
[2025-12-03 10:00:00.002] [info] SOLID: Dependency Inversion enabled for high test coverage
[2025-12-03 10:00:00.003] [info] Starting all adapters in dedicated threads...
[2025-12-03 10:00:00.004] [info] All adapters running. Entering main loop...
```

## Data Models

### ExtrapTrackData (Input)

| Field | Type | Description |
|-------|------|-------------|
| trackId | int32_t | Unique track identifier |
| xPositionECEF | double | X position in ECEF (meters) |
| yPositionECEF | double | Y position in ECEF (meters) |
| zPositionECEF | double | Z position in ECEF (meters) |
| xVelocityECEF | double | X velocity in ECEF (m/s) |
| yVelocityECEF | double | Y velocity in ECEF (m/s) |
| zVelocityECEF | double | Z velocity in ECEF (m/s) |
| updateTime | int64_t | Update time (microseconds) |
| originalUpdateTime | int64_t | Original update time (microseconds) |
| firstHopSentTime | int64_t | First hop sent time (microseconds) |

### DelayCalcTrackData (Output)

All fields from ExtrapTrackData plus:

| Field | Type | Description |
|-------|------|-------------|
| firstHopDelayTime | int64_t | Calculated first hop delay (microseconds) |
| secondHopSentTime | int64_t | Second hop sent time (microseconds) |

## Network Configuration

| Component | Protocol | Address | Port |
|-----------|----------|---------|------|
| Incoming (DISH) | UDP Multicast | 239.1.1.2 | 9001 |
| Outgoing (RADIO) | TCP | localhost | 15002 |

## Testing

```bash
cd test/b_hexagon
make test
./bin/run_tests
```

## Signal Handling

The application handles SIGINT (Ctrl+C) and SIGTERM for graceful shutdown:

```cpp
std::signal(SIGINT, signalHandler);
std::signal(SIGTERM, signalHandler);
```

## Dependencies

- **ZeroMQ** (>= 4.3.4 with Draft API)
- **spdlog** (>= 1.12.0)
- **CMake** (>= 3.16) or **Make**

## Author

B_Hexagon Team - 2025

## License

See LICENSE file for details.
