# C_Hexagon Project Documentation {#mainpage}

## Overview

C_Hexagon is a high-performance ZeroMQ-based message processing application built with **Thread-per-Type Hexagonal Architecture**.

## Key Features

- **Hexagonal Architecture (Ports & Adapters)**: Clean separation of concerns
- **Thread-per-Type Pattern**: Zero contention between message types
- **ZeroMQ RADIO/DISH Pattern**: UDP multicast for low-latency messaging
- **MISRA C++ 2023 Compliant**: Industry-standard safety and reliability
- **Async Logging**: ~20ns latency with spdlog

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                    THREAD-PER-TYPE ARCHITECTURE                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐ │
│  │ MessageType A   │    │ MessageType B   │    │ MessageType C   │ │
│  ├─────────────────┤    ├─────────────────┤    ├─────────────────┤ │
│  │ InAdapter (T1)  │    │ InAdapter (T2)  │    │ InAdapter (T3)  │ │
│  │       ↓         │    │       ↓         │    │       ↓         │ │
│  │ DomainService   │    │ DomainService   │    │ DomainService   │ │
│  │       ↓         │    │       ↓         │    │       ↓         │ │
│  │ OutAdapter (T1) │    │ OutAdapter (T2) │    │ OutAdapter (T3) │ │
│  └─────────────────┘    └─────────────────┘    └─────────────────┘ │
│                                                                     │
│  Each vertical stack is COMPLETELY ISOLATED - zero contention!     │
└─────────────────────────────────────────────────────────────────────┘
```

## Layer Structure

| Layer | Responsibility | Examples |
|-------|----------------|----------|
| **Adapters (Incoming)** | Receive messages from external world | TrackDataZeroMQIncomingAdapter |
| **Ports (Primary)** | Interface definitions for domain entry | IDelayCalcTrackDataIncomingPort |
| **Domain** | Business logic | FinalCalculationService |
| **Ports (Secondary)** | Interface definitions for external output | ITrackDataStatisticOutgoingPort |
| **Adapters (Outgoing)** | Send messages to external world | FinalCalcTrackDataZeroMQOutgoingAdapter |

## Namespace Structure

- **adapters**: All incoming and outgoing adapters
  - **adapters::common**: Common adapter interfaces and utilities
  - **adapters::incoming::zeromq**: ZeroMQ incoming adapters
  - **adapters::outgoing::zeromq**: ZeroMQ outgoing adapters
- **domain**: Business logic layer
  - **domain::logic**: Domain services
  - **domain::ports**: Port interfaces and data models
- **utils**: Utility classes (logging, etc.)

## Data Flow

1. **B_hexagon** sends `DelayCalcTrackData` via ZeroMQ RADIO socket
2. **TrackDataZeroMQIncomingAdapter** (DISH) receives the message
3. **FinalCalculationService** processes the data
4. **FinalCalcTrackDataZeroMQOutgoingAdapter** (RADIO) sends `FinalCalcTrackData`
5. **D_hexagon/Listener** receives the final data

## Key Classes

### Adapters
- **AdapterManager**: Manages all message pipelines
- **MessagePipeline**: Encapsulates a complete message flow
- **IAdapter**: Base interface for all adapters

### Domain
- **FinalCalculationService**: Main business logic
- **DelayCalcTrackData**: Input data model
- **FinalCalcTrackData**: Output data model

### Utilities
- **Logger**: High-performance async logging wrapper

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.1.0 | 2025-12 | Updated documentation, added mainpage |
| 1.0.0 | 2025 | Initial release |

## Author

c_hexagon Team

## License

Proprietary - All rights reserved
