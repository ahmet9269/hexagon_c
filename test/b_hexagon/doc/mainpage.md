# B_Hexagon Architecture Documentation {#architecture}

## Overview

**B_Hexagon** is a high-performance track data processing application built using **Hexagonal Architecture** (Ports & Adapters) with **Thread-per-Type** pattern for scalable concurrent processing.

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
5. Result sent via ZeroMQ RADIO socket (UDP multicast)

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

- **ZeroMQ** (libzmq with Draft API enabled)
- **spdlog** (Logging library)
- **C++17** or later

## Author

B_Hexagon Team - 2025

## License

See LICENSE file for details.
