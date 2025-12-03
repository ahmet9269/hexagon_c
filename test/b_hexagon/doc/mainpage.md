# B_Hexagon Architecture Documentation {#architecture}

@page architecture B_Hexagon Architecture

@tableofcontents

## Overview

**B_Hexagon** is a high-performance track data processing application built using **Hexagonal Architecture** (Ports & Adapters) with **Thread-per-Type** pattern for scalable concurrent processing.

@version 2.1.0
@date 2025-12-03
@author b_hexagon Team

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                    THREAD-PER-TYPE ARCHITECTURE                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    ADAPTERS (INFRASTRUCTURE)                 │   │
│  │  ┌───────────────────┐         ┌───────────────────────┐    │   │
│  │  │ ExtrapTrackData   │         │ DelayCalcTrackData    │    │   │
│  │  │ ZeroMQ Incoming   │────────▶│ ZeroMQ Outgoing       │    │   │
│  │  │ (DISH Socket)     │         │ (RADIO Socket)        │    │   │
│  │  └─────────┬─────────┘         └───────────▲───────────┘    │   │
│  │            │                               │                 │   │
│  └────────────┼───────────────────────────────┼─────────────────┘   │
│               │                               │                     │
│  ┌────────────▼───────────────────────────────┼─────────────────┐   │
│  │                    DOMAIN (BUSINESS LOGIC)                    │   │
│  │  ┌───────────────────────────────────────────────────────┐   │   │
│  │  │              ProcessTrackUseCase                       │   │   │
│  │  │  ┌─────────────────────────────────────────────────┐  │   │   │
│  │  │  │           CalculatorService                      │  │   │   │
│  │  │  │  • calculateDelay()                             │  │   │   │
│  │  │  │  • getCurrentTimeMicroseconds()                 │  │   │   │
│  │  │  └─────────────────────────────────────────────────┘  │   │   │
│  │  └───────────────────────────────────────────────────────┘   │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

## Key Features

- **Hexagonal Architecture**: Clean separation between domain logic and infrastructure
- **Thread-per-Type**: Each message type runs in its own isolated thread
- **SOLID Principles**: Full compliance with SOLID design principles
- **Dependency Inversion**: All dependencies are abstracted for testability
- **ZeroMQ Messaging**: High-performance UDP multicast with RADIO/DISH sockets
- **MISRA C++ 2023**: Compliant implementation for safety-critical systems

## Project Structure

```
b_hexagon/
├── src/b_hexagon/
│   ├── main.cpp                    # Application entry point
│   ├── main.hpp                    # Main header
│   ├── adapters/                   # Infrastructure layer
│   │   ├── common/                 # Shared adapter components
│   │   │   ├── IAdapter.hpp        # Base adapter interface
│   │   │   ├── AdapterManager.hpp  # Lifecycle management
│   │   │   ├── MessagePipeline.hpp # Message flow encapsulation
│   │   │   └── messaging/          # Socket abstractions
│   │   ├── incoming/               # Inbound adapters
│   │   └── outgoing/               # Outbound adapters
│   ├── domain/                     # Business logic layer
│   │   ├── logic/                  # Use cases and services
│   │   │   ├── ICalculatorService.hpp
│   │   │   ├── CalculatorService.hpp
│   │   │   └── ProcessTrackUseCase.hpp
│   │   ├── model/                  # Domain models
│   │   └── ports/                  # Port interfaces
│   │       ├── incoming/           # Inbound ports
│   │       └── outgoing/           # Outbound ports
│   └── utils/                      # Utility classes
│       └── Logger.hpp              # spdlog wrapper
└── test/                           # Unit tests
```

## SOLID Compliance

### Dependency Inversion Principle

```
┌─────────────────────────────────────────────────────────────────────┐
│                    DEPENDENCY INVERSION PRINCIPLE                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────┐         ┌─────────────────────────────────┐   │
│  │ProcessTrackUseCase│ ────▶ │    ICalculatorService           │   │
│  │   (High Level)  │         │      (Abstraction)              │   │
│  └─────────────────┘         └─────────────────────────────────┘   │
│                                         ▲                          │
│                              ┌──────────┴──────────┐               │
│                              │                     │               │
│                    ┌─────────────────┐   ┌─────────────────┐       │
│                    │CalculatorService│   │MockCalculatorSvc│       │
│                    │   (Concrete)    │   │    (Test)       │       │
│                    └─────────────────┘   └─────────────────┘       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

## Data Flow

1. **ExtrapTrackData** received via ZeroMQ DISH socket (UDP multicast)
2. Data deserialized and forwarded to **ProcessTrackUseCase**
3. **CalculatorService** calculates timing delays
4. **DelayCalcTrackData** created with computed metrics
5. Result enqueued to background worker (~20ns latency)
6. Background thread sends via ZeroMQ RADIO socket

## Performance Characteristics

### Latency Budget

| Operation | Target | Actual |
|-----------|--------|--------|
| Message Enqueue | <50ns | ~20ns |
| Log Statement | <100ns | ~20ns (async) |
| Delay Calculation | <50µs | ~20µs |
| ZMQ Send | <200µs | ~100µs |

### Thread Configuration

| Thread | Priority | CPU Core | Policy |
|--------|----------|----------|--------|
| Incoming Adapter | 95 | 1 | SCHED_FIFO |
| Outgoing Worker | 80 | 2 | SCHED_FIFO |
| Async Logger | 10 | Any | SCHED_OTHER |

## Building

```bash
cd test/b_hexagon
make clean
make
```

## Running

```bash
./run.sh
# or
./bin/b_hexagon
```

## Dependencies

- **ZeroMQ** (libzmq with Draft API enabled via zmq_config.hpp)
- **spdlog** (Async logging library)
- **C++17** or later

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 2.1.0 | 2025-12-03 | Doxygen documentation update, namespace fixes |
| 2.0.0 | 2025-12 | Background worker thread, shared_mutex, async logger, ILogger interface |
| 1.0.1 | 2025-11 | Initial SOLID compliant implementation |

## Related Pages

- See IAdapter for adapter implementations
- See domain::logic for domain logic
- See utils::Logger for logging utilities

## Author

B_Hexagon Team - 2025

@copyright See LICENSE file for details.
