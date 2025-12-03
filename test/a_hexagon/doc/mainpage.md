# a_hexagon - Track Data Extrapolation System {#mainpage}

\tableofcontents

## Overview

**a_hexagon** is a high-performance track data extrapolation system built using Hexagonal Architecture (Ports and Adapters pattern). It receives track data at 8Hz via ZeroMQ DISH sockets, performs linear extrapolation, and outputs at 100Hz via RADIO sockets.

### Key Features

| Feature | Description |
|---------|-------------|
| **Thread-per-Type** | Each message type processed in dedicated thread with CPU affinity |
| **Non-blocking I/O** | Background worker threads with ~20ns enqueue latency |
| **Async Logging** | spdlog async logger with overrun_oldest policy |
| **DIP Compliant** | All adapters depend on IMessageSocket abstraction |
| **MISRA C++ 2023** | Safety-critical coding standards |

---

## Architecture

### Hexagonal Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        a_hexagon Application                             │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│   ┌───────────────────┐                        ┌───────────────────┐    │
│   │   PRIMARY ADAPTER │                        │ SECONDARY ADAPTER │    │
│   │    (Incoming)     │                        │    (Outgoing)     │    │
│   ├───────────────────┤                        ├───────────────────┤    │
│   │ TrackDataZeroMQ   │                        │ ExtrapTrackData   │    │
│   │ IncomingAdapter   │    ┌──────────────┐    │ ZeroMQOutgoing    │    │
│   │                   │───▶│   DOMAIN     │───▶│ Adapter           │    │
│   │ - DISH Socket     │    │              │    │                   │    │
│   │ - Real-time Thread│    │ TrackData    │    │ - RADIO Socket    │    │
│   │ - CPU Affinity    │    │ Extrapolator │    │ - Worker Thread   │    │
│   └───────────────────┘    │              │    │ - Message Queue   │    │
│           │                │ 8Hz → 100Hz  │    └───────────────────┘    │
│           │                └──────────────┘            │                │
│           │                       │                    │                │
│   ┌───────▼───────┐       ┌───────▼───────┐   ┌───────▼───────┐        │
│   │ ITrackData    │       │ ExtrapTrack   │   │ IMessageSocket│        │
│   │ IncomingPort  │       │ Data Model    │   │ (Abstraction) │        │
│   └───────────────┘       └───────────────┘   └───────────────┘        │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

### Thread-per-Type Pattern

```
┌─────────────────────────────────────────────────────────────────────┐
│                    THREAD-PER-TYPE ARCHITECTURE                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                  TrackData Pipeline                          │    │
│  ├─────────────────────────────────────────────────────────────┤    │
│  │  TrackDataZeroMQIncomingAdapter (Thread T1, CPU 1)          │    │
│  │           │ SCHED_FIFO priority 95                          │    │
│  │           ▼                                                  │    │
│  │  TrackDataExtrapolator (Domain Service)                     │    │
│  │           │                                                  │    │
│  │           ▼                                                  │    │
│  │  ExtrapTrackDataZeroMQOutgoingAdapter                       │    │
│  │           │ Worker Thread (CPU 2, priority 80)              │    │
│  │           ▼                                                  │    │
│  │  Background Publisher (~20ns enqueue)                       │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                      │
│  Each pipeline operates in COMPLETE ISOLATION - zero contention!    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Core Components

### Adapters

| Class | Type | Description |
|-------|------|-------------|
| adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter | Primary | ZeroMQ DISH receiver with real-time thread |
| adapters::outgoing::zeromq::ExtrapTrackDataZeroMQOutgoingAdapter | Secondary | ZeroMQ RADIO sender with background worker |
| adapters::AdapterManager | Infrastructure | Pipeline lifecycle management with shared_mutex |
| adapters::MessagePipeline | Infrastructure | Groups incoming/outgoing adapters |

### Domain

| Class | Description |
|-------|-------------|
| domain::logic::TrackDataExtrapolator | Core extrapolation logic (8Hz → 100Hz) |
| domain::model::TrackData | Input track data model with binary serialization |
| domain::model::ExtrapTrackData | Output extrapolated data model |

### Ports

| Interface | Description |
|-----------|-------------|
| domain::ports::incoming::ITrackDataIncomingPort | Input port for track data processing |
| domain::ports::outgoing::IExtrapTrackDataOutgoingPort | Output port for extrapolated data |
| adapters::common::messaging::IMessageSocket | Socket abstraction (DIP) |

### Utilities

| Class | Description |
|-------|-------------|
| utils::Logger | Async spdlog wrapper with ~20ns latency |

---

## Performance Characteristics

### Latency Budget

| Operation | Target | Actual |
|-----------|--------|--------|
| Message Enqueue | <50ns | ~20ns |
| Log Statement | <100ns | ~20ns (async) |
| Extrapolation | <100µs | ~50µs |
| ZMQ Send | <200µs | ~100µs |

### Thread Configuration

| Thread | Priority | CPU Core | Policy |
|--------|----------|----------|--------|
| Incoming Adapter | 95 | 1 | SCHED_FIFO |
| Outgoing Worker | 80 | 2 | SCHED_FIFO |
| Async Logger | 10 | Any | SCHED_OTHER |

---

## Build & Run

### Prerequisites

- GCC 8.5+ with C++17 support
- ZeroMQ 4.3+ (with DRAFT API enabled via zmq_config.hpp)
- spdlog 1.12+
- Google Test (for unit tests)
- Doxygen + Graphviz (for documentation)

### Build

```bash
cd test/a_hexagon
make clean && make
```

### Run

```bash
cd test/a_hexagon
./run.sh
```

### Test

```bash
cd test/a_hexagon/test/a_hexagon
make clean && make test
```

---

## SOLID Compliance

| Principle | Implementation |
|-----------|----------------|
| **S**ingle Responsibility | Each adapter handles one message type |
| **O**pen/Closed | IMessageSocket allows new socket implementations |
| **L**iskov Substitution | MockMessageSocket substitutes ZeroMQSocket |
| **I**nterface Segregation | Focused port interfaces |
| **D**ependency Inversion | Adapters depend on IMessageSocket abstraction |

---

## Related Projects

- **b_hexagon** - Delay calculation processor (receives from a_hexagon)
- **c_hexagon** - Final calculation processor (receives from b_hexagon)

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 2.1.0 | 2025-12 | Improved Doxygen documentation, DIP socket abstraction |
| 2.0.0 | 2025-12 | Background worker thread, shared_mutex, async logger |
| 1.0.0 | 2025-11 | Initial hexagonal architecture |

---

## Documentation

### Doxygen Comments

This project uses comprehensive Doxygen comments following these conventions:

| Tag | Usage |
|-----|-------|
| `@file` | File-level documentation |
| `@class` | Class-level documentation |
| `@brief` | Short description |
| `@details` | Extended description |
| `@param` | Parameter documentation |
| `@return` | Return value documentation |
| `@pre` | Preconditions |
| `@post` | Postconditions |
| `@invariant` | Class invariants |
| `@note` | Important notes |
| `@see` | Cross-references |
| `@thread_safe` | Thread safety information |
| `@latency` | Latency characteristics |

### Generate Documentation

```bash
cd test/a_hexagon
doxygen Doxyfile
# Open doc/html/index.html in browser
```

