# C_Hexagon

High-performance ZeroMQ-based message processing application built with **Thread-per-Type Hexagonal Architecture**.

## 📋 Table of Contents

- [Architecture](#-architecture)
- [Project Structure](#-project-structure)
- [Requirements](#-requirements)
- [Building](#-building)
- [Running](#-running)
- [Testing](#-testing)
- [API Reference](#-api-reference)

## 🏗 Architecture

The project combines **Hexagonal Architecture (Ports & Adapters)** pattern with **Thread-per-Type** isolation strategy:

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

### Layers

| Layer | Responsibility | Example |
|-------|----------------|---------|
| **Adapters (Incoming)** | Receive messages from external world | `TrackDataZeroMQIncomingAdapter` |
| **Ports** | Interface definitions | `IDelayCalcTrackDataIncomingPort` |
| **Domain** | Business logic | `FinalCalculationService` |
| **Adapters (Outgoing)** | Send messages to external world | (Future) |

## 📁 Project Structure

```
src/c_hexagon/
├── main.cpp                          # Application entry point
├── c_hexagon.hpp                     # Main header
├── adapters/
│   ├── common/
│   │   ├── IAdapter.hpp              # Adapter base interface
│   │   ├── MessagePipeline.hpp       # Pipeline class
│   │   └── AdapterManager.hpp        # Pipeline manager
│   └── incoming/
│       └── zeromq/
│           └── TrackDataZeroMQIncomingAdapter.hpp
├── domain/
│   ├── logic/
│   │   ├── FinalCalculationService.hpp
│   │   └── FinalCalculationService.cpp
│   └── model/
│       ├── DelayCalcTrackData.hpp/.cpp
│       └── FinalCalcTrackData.hpp/.cpp
├── ports/
│   └── incoming/
│       └── IDelayCalcTrackDataIncomingPort.hpp
└── utils/
    └── Logger.hpp                    # spdlog async wrapper

test/c_hexagon/
├── adapters/common/
│   └── AdapterManagerTest.cpp
├── domain/
│   ├── logic/
│   │   └── FinalCalculationServiceTest.cpp
│   └── model/
│       ├── DelayCalcTrackDataTest.cpp
│       └── FinalCalcTrackDataTest.cpp
├── utils/
│   └── LoggerTest.cpp
└── main_test.cpp
```

## 📦 Requirements

| Dependency | Version | Description |
|------------|---------|-------------|
| GCC | 8.5+ | C++17 support |
| ZeroMQ | 4.3+ | Draft API support (`ZMQ_BUILD_DRAFT_API=1`) |
| spdlog | 1.12.0 | Async logging (~20ns latency) |
| Google Test | 1.14.0 | Unit testing framework |

## 🔨 Building

### Main Application

```bash
cd /workspaces/hexagon_c/test/c_hexagon
make clean && make
```

Output: `bin/c_hexagon`

### Test Binary

```bash
make -f Makefile.test clean
make -f Makefile.test test
```

Output: `bin/c_hexagon_test`

### Build Options

```bash
# Debug build
make DEBUG=1

# Release build (default)
make

# Verbose output
make V=1
```

## 🚀 Running

```bash
# Start the application
./bin/c_hexagon

# Graceful shutdown: Ctrl+C
```

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `LD_LIBRARY_PATH` | `./lib` | ZeroMQ and spdlog library path |

## 🧪 Testing

### Run All Tests

```bash
make -f Makefile.test test
```

### Test Results

```
[==========] 106 tests from 7 test suites ran. (12 ms total)
[  PASSED  ] 106 tests.
```

### Test Suites

| Suite | Test Count | Description |
|-------|------------|-------------|
| `IAdapterTest` | 5 | Interface contract tests |
| `MessagePipelineTest` | 12 | Pipeline lifecycle tests |
| `AdapterManagerTest` | 12 | Manager orchestration tests |
| `FinalCalculationServiceTest` | 25+ | Domain logic tests |
| `DelayCalcTrackDataTest` | 20+ | Model tests |
| `FinalCalcTrackDataTest` | 20+ | Model tests |
| `LoggerTest` | 10+ | Logging tests |

## 📚 API Reference

### IAdapter Interface

```cpp
namespace adapters {

class IAdapter {
public:
    virtual ~IAdapter() = default;
    
    virtual bool start() = 0;           // Start the adapter
    virtual void stop() = 0;            // Stop the adapter
    virtual bool isRunning() const = 0; // Check if running
    virtual std::string getName() const = 0; // Get adapter name
};

}
```

### MessagePipeline Class

```cpp
namespace adapters {

class MessagePipeline {
public:
    explicit MessagePipeline(std::string name);
    MessagePipeline(std::string name,
                    std::shared_ptr<IAdapter> incoming,
                    std::shared_ptr<IAdapter> outgoing = nullptr);
    
    bool start();
    void stop() noexcept;
    bool isRunning() const noexcept;
    
    const std::string& getName() const noexcept;
    void setIncomingAdapter(std::shared_ptr<IAdapter> adapter);
    void setOutgoingAdapter(std::shared_ptr<IAdapter> adapter);
};

}
```

### AdapterManager Class

```cpp
namespace adapters {

class AdapterManager {
public:
    AdapterManager();
    ~AdapterManager();
    
    void registerPipeline(MessagePipeline pipeline);
    bool startAll();
    void stopAll() noexcept;
    
    std::size_t getPipelineCount() const noexcept;
    bool isRunning() const noexcept;
    MessagePipeline* getPipeline(const std::string& name) noexcept;
};

}
```

### Logger Usage

```cpp
#include "utils/Logger.hpp"

// Initialize once at startup
utils::Logger::init("app_name");

// Log levels
LOG_TRACE("Trace message: {}", value);
LOG_DEBUG("Debug message: {}", value);
LOG_INFO("Info message: {}", value);
LOG_WARN("Warning message: {}", value);
LOG_ERROR("Error message: {}", value);
LOG_CRITICAL("Critical message: {}", value);

// Shutdown gracefully
utils::Logger::shutdown();
```

## 📐 MISRA C++ 2023 Compliance

The project is developed in compliance with MISRA C++ 2023 standards:

- ✅ Rule 3.2: Single Responsibility (one responsibility per file)
- ✅ Rule 6.4: `[[nodiscard]]` attribute usage
- ✅ Rule 11.3: `noexcept` exception specification
- ✅ Rule 15.1: Smart pointer usage (no raw pointers)
- ✅ Rule 21.6: Type-safe logging (no printf)

## 📊 Performance

| Metric | Value |
|--------|-------|
| Logging latency | ~20ns (async) |
| Message rate | 200 Hz |
| Thread priority | SCHED_FIFO 95 |
| CPU affinity | Core 1 |

## 📄 License

HEX License

## 👥 Contributors

- c_hexagon Team

---

**Note**: For Doxygen documentation, run `doxygen Doxyfile`.
