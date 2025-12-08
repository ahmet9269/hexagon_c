# 🏗️ A_Hexagon - Yeni Incoming Adapter Ekleme Rehberi

Bu dokümantasyon, a_hexagon projesine **teknoloji bağımsız** yeni bir **Incoming Adapter** ekleme sürecini adım adım açıklar.

---

## 📋 İçindekiler

1. [Genel Bakış](#genel-bakış)
2. [Dosya Yapısı](#dosya-yapısı)
3. [Adım 1: Data Model Oluşturma](#adım-1-data-model-oluşturma)
4. [Adım 2: Incoming Port Interface Oluşturma](#adım-2-incoming-port-interface-oluşturma)
5. [Adım 3: Domain Service Oluşturma](#adım-3-domain-service-oluşturma)
6. [Adım 4: Incoming Adapter Header](#adım-4-incoming-adapter-header)
7. [Adım 5: Incoming Adapter Implementation](#adım-5-incoming-adapter-implementation)
8. [Adım 6: main.cpp Entegrasyonu](#adım-6-maincpp-entegrasyonu)
9. [Adım 7: Makefile Güncellemesi](#adım-7-makefile-güncellemesi)
10. [Checklist](#checklist)

---

## Genel Bakış

### Hexagonal Architecture - Incoming Adapter Akışı

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         EXTERNAL SYSTEM                                 │
│   ┌──────────────────────────────────────────────────────────────────┐ │
│   │  Kafka / RabbitMQ / gRPC / File / TCP / Custom Protocol          │ │
│   └─────────────────────────────┬────────────────────────────────────┘ │
└─────────────────────────────────┼───────────────────────────────────────┘
                                  │ Raw Data
                                  ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    INCOMING ADAPTER (Primary Adapter)                   │
│   ┌───────────────────────────────────────────────────────────────────┐ │
│   │  CustomIncomingAdapter                                            │ │
│   │  ┌─────────────────────────────────────────────────────────────┐  │ │
│   │  │  1. receive() → Raw binary data                             │  │ │
│   │  │  2. deserialize() → CustomData model                        │  │ │
│   │  │  3. incomingPort_->processCustomData(data) → Domain'e aktar │  │ │
│   │  └─────────────────────────────────────────────────────────────┘  │ │
│   └───────────────────────────────────────────────────────────────────┘ │
│                                    │                                    │
│                     implements IAdapter (lifecycle)                     │
└────────────────────────────────────┼────────────────────────────────────┘
                                     │ calls
                                     ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                         DOMAIN LAYER                                    │
│   ┌───────────────────────────────────────────────────────────────────┐ │
│   │  ICustomDataIncomingPort (Primary Port Interface)                 │ │
│   │  - processCustomData(data): void                                  │ │
│   └───────────────────────────────────────────────────────────────────┘ │
│                                    ▲                                    │
│                                    │ implements                         │
│   ┌───────────────────────────────────────────────────────────────────┐ │
│   │  CustomDataService : ICustomDataIncomingPort                      │ │
│   │  - İş mantığı burada                                              │ │
│   │  - Veri manipülasyonu                                             │ │
│   │  - outgoingPort_->send() ile çıkış                                │ │
│   └───────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────┘
```

### Incoming Adapter'ın Görevi

| Görev | Açıklama |
|-------|----------|
| **Veri Alma** | Dış sistemden ham veri alır |
| **Deserialize** | Binary → Domain Model dönüşümü |
| **Port Çağrısı** | Domain service'e veri aktarır |
| **Lifecycle** | start/stop ile yaşam döngüsü yönetimi |
| **Thread Yönetimi** | Kendi thread'inde çalışır |

---

## Dosya Yapısı

Yeni bir Incoming Adapter eklemek için oluşturulması gereken dosyalar:

```
src/a_hexagon/
├── adapters/
│   ├── common/
│   │   ├── IAdapter.hpp              ← MEVCUT (lifecycle interface)
│   │   ├── AdapterManager.hpp        ← MEVCUT (pipeline yönetimi)
│   │   └── MessagePipeline.hpp       ← MEVCUT (adapter grubu)
│   └── incoming/
│       ├── zeromq/                   ← MEVCUT (ZeroMQ implementasyonu)
│       │   └── TrackDataZeroMQIncomingAdapter.hpp/cpp
│       └── custom/                   ← 📁 YENİ KLASÖR
│           ├── CustomIncomingAdapter.hpp   ← 📄 YENİ (ADIM 4)
│           └── CustomIncomingAdapter.cpp   ← 📄 YENİ (ADIM 5)
├── domain/
│   ├── model/
│   │   └── CustomData.hpp            ← 📄 YENİ (ADIM 1)
│   ├── ports/
│   │   └── incoming/
│   │       ├── ITrackDataIncomingPort.hpp  ← MEVCUT
│   │       └── ICustomDataIncomingPort.hpp ← 📄 YENİ (ADIM 2)
│   └── logic/
│       └── CustomDataService.hpp/cpp  ← 📄 YENİ (ADIM 3)
└── main.cpp                          ← 🔧 GÜNCELLE (ADIM 6)
```

---

## Adım 1: Data Model Oluşturma

**Dosya:** `src/a_hexagon/domain/model/CustomData.hpp`

Data model, adapter ile domain arasındaki veri kontratını tanımlar.

```cpp
/**
 * @file CustomData.hpp
 * @brief Domain model for custom incoming data
 * @details Technology-independent data representation
 */

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

namespace domain {
namespace model {

/**
 * @class CustomData
 * @brief Immutable value object for custom data
 * @details MISRA C++ 2023 compliant implementation
 * 
 * @invariant id_ > 0 for valid data
 * @invariant timestamp_ > 0 for valid data
 */
class CustomData final {
public:
    // ═══════════════════════════════════════════════════════════════════
    // CONSTRUCTORS & DESTRUCTOR (Rule of Five)
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Default constructor
     */
    explicit CustomData() noexcept = default;
    
    /**
     * @brief Parameterized constructor
     */
    CustomData(int32_t id, double value, int64_t timestamp)
        : id_(id), value_(value), timestamp_(timestamp) {}
    
    // Rule of Five - default implementations
    CustomData(const CustomData& other) = default;
    CustomData(CustomData&& other) noexcept = default;
    CustomData& operator=(const CustomData& other) = default;
    CustomData& operator=(CustomData&& other) noexcept = default;
    ~CustomData() = default;

    // ═══════════════════════════════════════════════════════════════════
    // GETTERS
    // ═══════════════════════════════════════════════════════════════════
    
    [[nodiscard]] int32_t getId() const noexcept { return id_; }
    [[nodiscard]] double getValue() const noexcept { return value_; }
    [[nodiscard]] int64_t getTimestamp() const noexcept { return timestamp_; }
    [[nodiscard]] const std::string& getName() const noexcept { return name_; }

    // ═══════════════════════════════════════════════════════════════════
    // SETTERS
    // ═══════════════════════════════════════════════════════════════════
    
    void setId(int32_t id) noexcept { id_ = id; }
    void setValue(double value) noexcept { value_ = value; }
    void setTimestamp(int64_t timestamp) noexcept { timestamp_ = timestamp; }
    void setName(const std::string& name) { name_ = name; }

    // ═══════════════════════════════════════════════════════════════════
    // VALIDATION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Check if data is valid
     * @return true if all required fields are properly set
     */
    [[nodiscard]] bool isValid() const noexcept {
        return id_ > 0 && timestamp_ > 0;
    }

    // ═══════════════════════════════════════════════════════════════════
    // SERIALIZATION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Get serialized size in bytes
     */
    [[nodiscard]] static constexpr size_t getSerializedSize() noexcept {
        return sizeof(int32_t)   // id_
             + sizeof(double)    // value_
             + sizeof(int64_t);  // timestamp_
    }

    /**
     * @brief Deserialize from binary data
     * @param data Raw binary data
     * @return true if deserialization successful
     */
    [[nodiscard]] bool deserialize(const std::vector<uint8_t>& data) {
        if (data.size() < getSerializedSize()) {
            return false;
        }

        size_t offset = 0;

        std::memcpy(&id_, data.data() + offset, sizeof(id_));
        offset += sizeof(id_);

        std::memcpy(&value_, data.data() + offset, sizeof(value_));
        offset += sizeof(value_);

        std::memcpy(&timestamp_, data.data() + offset, sizeof(timestamp_));
        offset += sizeof(timestamp_);

        return true;
    }

    /**
     * @brief Serialize to binary data
     * @return Serialized binary representation
     */
    [[nodiscard]] std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer(getSerializedSize());
        size_t offset = 0;

        std::memcpy(buffer.data() + offset, &id_, sizeof(id_));
        offset += sizeof(id_);

        std::memcpy(buffer.data() + offset, &value_, sizeof(value_));
        offset += sizeof(value_);

        std::memcpy(buffer.data() + offset, &timestamp_, sizeof(timestamp_));
        offset += sizeof(timestamp_);

        return buffer;
    }

private:
    int32_t id_{0};
    double value_{0.0};
    int64_t timestamp_{0};
    std::string name_;
};

} // namespace model
} // namespace domain
```

---

## Adım 2: Incoming Port Interface Oluşturma

**Dosya:** `src/a_hexagon/domain/ports/incoming/ICustomDataIncomingPort.hpp`

Bu interface, adapter'ın domain'e veri aktarması için kontrat tanımlar.

```cpp
/**
 * @file ICustomDataIncomingPort.hpp
 * @brief Input port interface for CustomData processing
 * @details Primary port in hexagonal architecture - driven by incoming adapters
 */

#ifndef CUSTOM_DATA_INCOMING_PORT_HPP
#define CUSTOM_DATA_INCOMING_PORT_HPP

#include "domain/model/CustomData.hpp"

namespace domain {
namespace ports {
namespace incoming {

/**
 * @class ICustomDataIncomingPort
 * @brief Abstract interface for custom data input port (primary port)
 * @details Defines the contract between incoming adapters and domain services.
 * 
 * Usage:
 * @code
 * class CustomDataService : public ICustomDataIncomingPort {
 *     void processCustomData(const CustomData& data) override {
 *         // Process incoming data
 *     }
 * };
 * @endcode
 * 
 * @note All implementations must be thread-safe
 */
class ICustomDataIncomingPort {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~ICustomDataIncomingPort() = default;

    /**
     * @brief Process received custom data
     * @param data Incoming data to process
     * @pre data.isValid() returns true
     * @thread_safe Must be thread-safe for concurrent adapter calls
     */
    virtual void processCustomData(const model::CustomData& data) = 0;

protected:
    // Prevent direct instantiation
    ICustomDataIncomingPort() = default;
    ICustomDataIncomingPort(const ICustomDataIncomingPort&) = default;
    ICustomDataIncomingPort(ICustomDataIncomingPort&&) = default;
    ICustomDataIncomingPort& operator=(const ICustomDataIncomingPort&) = default;
    ICustomDataIncomingPort& operator=(ICustomDataIncomingPort&&) = default;
};

} // namespace incoming
} // namespace ports
} // namespace domain

#endif // CUSTOM_DATA_INCOMING_PORT_HPP
```

---

## Adım 3: Domain Service Oluşturma

**Dosya:** `src/a_hexagon/domain/logic/CustomDataService.hpp`

Domain service, incoming port'u implement eder ve iş mantığını içerir.

```cpp
/**
 * @file CustomDataService.hpp
 * @brief Domain service for processing CustomData
 * @details Implements business logic and coordinates between ports
 */

#pragma once

#include "domain/ports/incoming/ICustomDataIncomingPort.hpp"
#include "domain/ports/outgoing/ICustomDataOutgoingPort.hpp"  // Eğer çıkış varsa
#include "domain/model/CustomData.hpp"
#include "utils/Logger.hpp"

#include <memory>
#include <atomic>

namespace domain {
namespace logic {

/**
 * @class CustomDataService
 * @brief Processes incoming CustomData and forwards to output
 * @details Core domain logic implementation
 */
class CustomDataService : public ports::incoming::ICustomDataIncomingPort {
public:
    // ═══════════════════════════════════════════════════════════════════
    // CONSTRUCTORS
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Constructor without output (sadece loglama)
     */
    CustomDataService() 
        : processedCount_(0) {
        LOG_INFO("CustomDataService initialized (logging only mode)");
    }

    /**
     * @brief Constructor with outgoing port (çıkış ile)
     * @param outgoingPort Port for sending processed data
     */
    explicit CustomDataService(
        std::shared_ptr<ports::outgoing::ICustomDataOutgoingPort> outgoingPort)
        : outgoingPort_(std::move(outgoingPort))
        , processedCount_(0) {
        LOG_INFO("CustomDataService initialized with outgoing port");
    }

    // ═══════════════════════════════════════════════════════════════════
    // ICustomDataIncomingPort IMPLEMENTATION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Process received custom data
     * @param data Incoming data to process
     * 
     * ═══════════════════════════════════════════════════════════════════
     * 🔧 VERİ MANİPÜLASYONU BURADA YAPILIR
     * ═══════════════════════════════════════════════════════════════════
     */
    void processCustomData(const model::CustomData& data) override {
        // 1. Validasyon
        if (!data.isValid()) {
            LOG_WARN("Invalid CustomData received, skipping");
            return;
        }

        // 2. İş mantığı / Manipülasyon
        ++processedCount_;
        
        LOG_INFO("Processing CustomData - ID: {}, Value: {}, Count: {}",
                 data.getId(), data.getValue(), processedCount_.load());

        // 3. Hesaplama örneği
        double processedValue = data.getValue() * scaleFactor_;

        // 4. Çıkış portu varsa, işlenmiş veriyi gönder
        if (outgoingPort_) {
            // model::ProcessedCustomData outputData;
            // outputData.setOriginalId(data.getId());
            // outputData.setProcessedValue(processedValue);
            // outgoingPort_->sendProcessedData(outputData);
        }
    }

    // ═══════════════════════════════════════════════════════════════════
    // STATISTICS
    // ═══════════════════════════════════════════════════════════════════
    
    [[nodiscard]] uint64_t getProcessedCount() const noexcept {
        return processedCount_.load();
    }

private:
    std::shared_ptr<ports::outgoing::ICustomDataOutgoingPort> outgoingPort_;
    std::atomic<uint64_t> processedCount_;
    double scaleFactor_{1.0};
};

} // namespace logic
} // namespace domain
```

---

## Adım 4: Incoming Adapter Header

**Dosya:** `src/a_hexagon/adapters/incoming/custom/CustomIncomingAdapter.hpp`

Adapter, `IAdapter` interface'ini implement eder ve lifecycle yönetimi sağlar.

```cpp
/**
 * @file CustomIncomingAdapter.hpp
 * @brief Technology-independent incoming adapter template
 * @details Can be implemented with any messaging technology
 *          (Kafka, RabbitMQ, gRPC, TCP, File, etc.)
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/incoming/ICustomDataIncomingPort.hpp"
#include "domain/model/CustomData.hpp"

#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace adapters {
namespace incoming {
namespace custom {

/**
 * @class CustomIncomingAdapter
 * @brief Base adapter for receiving CustomData from external systems
 * @details Implements IAdapter for lifecycle management.
 *          Technology-specific implementation should override receiveData().
 * 
 * Architecture:
 * ┌─────────────────────────────────────────────────────────────┐
 * │              CustomIncomingAdapter                          │
 * ├─────────────────────────────────────────────────────────────┤
 * │  [External System] → receiveData() → deserialize()         │
 * │         ↓                                                   │
 * │  incomingPort_->processCustomData(data)                     │
 * └─────────────────────────────────────────────────────────────┘
 * 
 * @invariant When running_, receiveThread_ is active
 */
class CustomIncomingAdapter : public ::adapters::IAdapter {
public:
    // ═══════════════════════════════════════════════════════════════════
    // CONSTRUCTORS & DESTRUCTOR
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Constructor with domain port injection
     * @param incomingPort Domain service implementing ICustomDataIncomingPort
     * @param adapterName Unique identifier for this adapter
     * @pre incomingPort is not null
     */
    explicit CustomIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ICustomDataIncomingPort> incomingPort,
        const std::string& adapterName = "CustomIncomingAdapter");

    /**
     * @brief Constructor with configuration
     * @param incomingPort Domain service port
     * @param endpoint Connection endpoint (format depends on technology)
     * @param adapterName Unique identifier
     */
    CustomIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ICustomDataIncomingPort> incomingPort,
        const std::string& endpoint,
        const std::string& adapterName);

    /**
     * @brief Destructor - ensures graceful shutdown
     */
    ~CustomIncomingAdapter() override;

    // ═══════════════════════════════════════════════════════════════════
    // IAdapter INTERFACE IMPLEMENTATION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Start the adapter's receive loop
     * @return true if started successfully
     * @post isRunning() returns true
     * @post Receive thread is active
     */
    [[nodiscard]] bool start() override;

    /**
     * @brief Stop the adapter gracefully
     * @post isRunning() returns false
     * @post Receive thread has terminated
     */
    void stop() override;

    /**
     * @brief Check if adapter is running
     * @return true if receive loop is active
     */
    [[nodiscard]] bool isRunning() const override;

    /**
     * @brief Get adapter name
     * @return Adapter identifier string
     */
    [[nodiscard]] std::string getName() const noexcept override;

    // ═══════════════════════════════════════════════════════════════════
    // DELETE COPY/MOVE (Thread Safety)
    // ═══════════════════════════════════════════════════════════════════
    
    CustomIncomingAdapter(const CustomIncomingAdapter&) = delete;
    CustomIncomingAdapter& operator=(const CustomIncomingAdapter&) = delete;
    CustomIncomingAdapter(CustomIncomingAdapter&&) = delete;
    CustomIncomingAdapter& operator=(CustomIncomingAdapter&&) = delete;

protected:
    // ═══════════════════════════════════════════════════════════════════
    // VIRTUAL METHODS - Override for specific technology
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Initialize connection to external system
     * @return true if connection established
     * @note Override for specific technology (Kafka, RabbitMQ, etc.)
     */
    virtual bool initializeConnection();

    /**
     * @brief Receive raw data from external system
     * @param buffer Output buffer for received data
     * @return true if data received, false if timeout or no data
     * @note Override for specific technology
     */
    virtual bool receiveData(std::vector<uint8_t>& buffer);

    /**
     * @brief Close connection to external system
     * @note Override for specific technology
     */
    virtual void closeConnection();

private:
    // ═══════════════════════════════════════════════════════════════════
    // PRIVATE METHODS
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Main receive loop (runs in dedicated thread)
     */
    void receiveLoop();

    /**
     * @brief Deserialize binary data to CustomData
     * @param data Raw binary message
     * @param customData Output object
     * @return true if deserialization successful
     */
    bool deserializeData(const std::vector<uint8_t>& data,
                         domain::model::CustomData& customData);

    // ═══════════════════════════════════════════════════════════════════
    // MEMBER VARIABLES
    // ═══════════════════════════════════════════════════════════════════
    
    // Domain port (where data is sent)
    std::shared_ptr<domain::ports::incoming::ICustomDataIncomingPort> incomingPort_;

    // Configuration
    std::string endpoint_;
    std::string adapterName_;

    // Thread management
    std::thread receiveThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopRequested_{false};

    // Statistics
    std::atomic<uint64_t> receivedCount_{0};
    std::atomic<uint64_t> errorCount_{0};

    // ═══════════════════════════════════════════════════════════════════
    // CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════
    
    static constexpr int32_t RECEIVE_TIMEOUT_MS{100};
    static constexpr int32_t REALTIME_THREAD_PRIORITY{95};
    static constexpr int32_t DEDICATED_CPU_CORE{1};
};

} // namespace custom
} // namespace incoming
} // namespace adapters
```

---

## Adım 5: Incoming Adapter Implementation

**Dosya:** `src/a_hexagon/adapters/incoming/custom/CustomIncomingAdapter.cpp`

```cpp
/**
 * @file CustomIncomingAdapter.cpp
 * @brief Implementation of CustomIncomingAdapter
 */

#include "CustomIncomingAdapter.hpp"
#include "utils/Logger.hpp"

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

namespace adapters {
namespace incoming {
namespace custom {

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTORS & DESTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════

CustomIncomingAdapter::CustomIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::ICustomDataIncomingPort> incomingPort,
    const std::string& adapterName)
    : incomingPort_(std::move(incomingPort))
    , adapterName_(adapterName) {
    
    LOG_INFO("[{}] Adapter created", adapterName_);
}

CustomIncomingAdapter::CustomIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::ICustomDataIncomingPort> incomingPort,
    const std::string& endpoint,
    const std::string& adapterName)
    : incomingPort_(std::move(incomingPort))
    , endpoint_(endpoint)
    , adapterName_(adapterName) {
    
    LOG_INFO("[{}] Adapter created with endpoint: {}", adapterName_, endpoint_);
}

CustomIncomingAdapter::~CustomIncomingAdapter() {
    stop();
    LOG_INFO("[{}] Adapter destroyed", adapterName_);
}

// ═══════════════════════════════════════════════════════════════════════════
// IAdapter INTERFACE IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

bool CustomIncomingAdapter::start() {
    if (running_.load()) {
        LOG_WARN("[{}] Already running", adapterName_);
        return false;
    }

    // Initialize connection to external system
    if (!initializeConnection()) {
        LOG_ERROR("[{}] Failed to initialize connection", adapterName_);
        return false;
    }

    stopRequested_.store(false);
    running_.store(true);

    // Start receive thread
    receiveThread_ = std::thread([this]() {
        // Configure real-time thread priority (Linux)
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = REALTIME_THREAD_PRIORITY;
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

        // Set CPU affinity
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif

        receiveLoop();
    });

    LOG_INFO("[{}] Started successfully", adapterName_);
    return true;
}

void CustomIncomingAdapter::stop() {
    if (!running_.load()) {
        return;
    }

    stopRequested_.store(true);
    running_.store(false);

    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }

    closeConnection();

    LOG_INFO("[{}] Stopped - Received: {}, Errors: {}",
             adapterName_, receivedCount_.load(), errorCount_.load());
}

bool CustomIncomingAdapter::isRunning() const {
    return running_.load();
}

std::string CustomIncomingAdapter::getName() const noexcept {
    return adapterName_;
}

// ═══════════════════════════════════════════════════════════════════════════
// VIRTUAL METHODS (Default implementation - override for specific tech)
// ═══════════════════════════════════════════════════════════════════════════

bool CustomIncomingAdapter::initializeConnection() {
    // Override this method for specific technology
    // Example: Kafka consumer initialization, TCP socket connection, etc.
    LOG_DEBUG("[{}] Default initializeConnection() called", adapterName_);
    return true;
}

bool CustomIncomingAdapter::receiveData(std::vector<uint8_t>& buffer) {
    // Override this method for specific technology
    // Example: Kafka consume, TCP recv, file read, etc.
    
    // Simulated data for testing
    std::this_thread::sleep_for(std::chrono::milliseconds(RECEIVE_TIMEOUT_MS));
    return false;  // No data by default
}

void CustomIncomingAdapter::closeConnection() {
    // Override this method for specific technology
    LOG_DEBUG("[{}] Default closeConnection() called", adapterName_);
}

// ═══════════════════════════════════════════════════════════════════════════
// PRIVATE METHODS
// ═══════════════════════════════════════════════════════════════════════════

void CustomIncomingAdapter::receiveLoop() {
    LOG_DEBUG("[{}] Receive loop started", adapterName_);

    std::vector<uint8_t> buffer;
    buffer.reserve(4096);  // Pre-allocate buffer

    while (!stopRequested_.load()) {
        try {
            buffer.clear();

            // Receive data from external system
            if (receiveData(buffer)) {
                if (buffer.empty()) {
                    continue;
                }

                // Deserialize to domain model
                domain::model::CustomData customData;
                if (deserializeData(buffer, customData)) {
                    // Validate and forward to domain
                    if (customData.isValid() && incomingPort_) {
                        ++receivedCount_;
                        
                        // ═══════════════════════════════════════════════════
                        // 🔥 VERİ DOMAIN'E BURADA AKTARILIR
                        // ═══════════════════════════════════════════════════
                        incomingPort_->processCustomData(customData);
                        
                        LOG_DEBUG("[{}] Data forwarded to domain - ID: {}",
                                  adapterName_, customData.getId());
                    } else {
                        LOG_WARN("[{}] Invalid data received", adapterName_);
                        ++errorCount_;
                    }
                } else {
                    LOG_ERROR("[{}] Deserialization failed - Size: {} bytes",
                              adapterName_, buffer.size());
                    ++errorCount_;
                }
            }
            // No data received, continue loop

        } catch (const std::exception& e) {
            LOG_ERROR("[{}] Error in receive loop: {}", adapterName_, e.what());
            ++errorCount_;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    LOG_DEBUG("[{}] Receive loop ended", adapterName_);
}

bool CustomIncomingAdapter::deserializeData(
    const std::vector<uint8_t>& data,
    domain::model::CustomData& customData) {
    
    try {
        return customData.deserialize(data);
    } catch (const std::exception& e) {
        LOG_ERROR("[{}] Deserialize exception: {}", adapterName_, e.what());
        return false;
    }
}

} // namespace custom
} // namespace incoming
} // namespace adapters
```

---

## Adım 6: main.cpp Entegrasyonu

**Dosya:** `src/a_hexagon/main.cpp` içine eklenecekler:

### 6.1 Include Ekle

```cpp
// Existing includes...
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"

// ═══════════════════════════════════════════════════════════════════════
// YENİ INCLUDE'LAR
// ═══════════════════════════════════════════════════════════════════════
#include "adapters/incoming/custom/CustomIncomingAdapter.hpp"
#include "domain/logic/CustomDataService.hpp"
#include "domain/ports/incoming/ICustomDataIncomingPort.hpp"
```

### 6.2 Pipeline Oluştur ve Kaydet

```cpp
int main(int argc, char* argv[]) {
    // ... existing code ...

    try {
        // AdapterManager oluştur
        adapters::AdapterManager adapter_manager;

        // ═══════════════════════════════════════════════════════════════
        // MEVCUT PIPELINE (TrackData)
        // ═══════════════════════════════════════════════════════════════
        // ... existing TrackData pipeline code ...

        // ═══════════════════════════════════════════════════════════════
        // YENİ PIPELINE (CustomData)
        // ═══════════════════════════════════════════════════════════════
        LOG_INFO("Creating CustomData processing pipeline...");

        // 1. Domain service oluştur
        auto custom_service = std::make_shared<domain::logic::CustomDataService>();
        
        // 2. Incoming adapter oluştur (service'i inject et)
        auto custom_adapter = std::make_shared<
            adapters::incoming::custom::CustomIncomingAdapter>(
            custom_service,                    // Domain port interface
            "tcp://127.0.0.1:15010",          // Endpoint (teknolojiye göre değişir)
            "CustomDataIncomingAdapter"        // Adapter adı
        );

        // 3. Pipeline oluştur
        adapters::MessagePipeline custom_pipeline(
            "CustomData",        // Pipeline adı
            custom_adapter,      // Incoming adapter
            nullptr              // Outgoing adapter (opsiyonel)
        );

        // 4. AdapterManager'a kaydet
        adapter_manager.registerPipeline(std::move(custom_pipeline));

        // ═══════════════════════════════════════════════════════════════
        // TÜM PIPELINE'LARI BAŞLAT
        // ═══════════════════════════════════════════════════════════════
        LOG_INFO("Starting all pipelines...");
        if (!adapter_manager.startAll()) {
            LOG_ERROR("Failed to start all pipelines");
            return 1;
        }

        LOG_INFO("All pipelines started successfully");
        LOG_INFO("Registered pipelines: {}", adapter_manager.getPipelineCount());

        // Main loop
        while (g_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Graceful shutdown
        adapter_manager.stopAll();

    } catch (const std::exception& e) {
        LOG_CRITICAL("Application error: {}", e.what());
        return 1;
    }

    return 0;
}
```

---

## Adım 7: Makefile Güncellemesi

**Dosya:** `Makefile` içine yeni .cpp dosyalarını ekle:

```makefile
# ═══════════════════════════════════════════════════════════════════════════
# SOURCE FILES
# ═══════════════════════════════════════════════════════════════════════════

# Mevcut dosyalar
SOURCES = \
    src/a_hexagon/main.cpp \
    src/a_hexagon/adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.cpp \
    src/a_hexagon/adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.cpp \
    src/a_hexagon/domain/logic/TrackDataExtrapolator.cpp \
    # ... diğer mevcut dosyalar ...

# ═══════════════════════════════════════════════════════════════════════════
# YENİ DOSYALAR
# ═══════════════════════════════════════════════════════════════════════════
SOURCES += \
    src/a_hexagon/adapters/incoming/custom/CustomIncomingAdapter.cpp \
    src/a_hexagon/domain/logic/CustomDataService.cpp

# ═══════════════════════════════════════════════════════════════════════════
# INCLUDE PATHS
# ═══════════════════════════════════════════════════════════════════════════
INCLUDES = -I$(SRC_DIR)

# Object files
OBJECTS = $(SOURCES:.cpp=.o)
```

---

## Checklist

### ✅ Dosya Oluşturma

- [ ] `domain/model/CustomData.hpp` - Data model
- [ ] `domain/ports/incoming/ICustomDataIncomingPort.hpp` - Port interface
- [ ] `domain/logic/CustomDataService.hpp` - Domain service
- [ ] `domain/logic/CustomDataService.cpp` - Service implementation (opsiyonel)
- [ ] `adapters/incoming/custom/CustomIncomingAdapter.hpp` - Adapter header
- [ ] `adapters/incoming/custom/CustomIncomingAdapter.cpp` - Adapter implementation

### ✅ Entegrasyon

- [ ] `main.cpp` - Include'lar ekle
- [ ] `main.cpp` - Service oluştur
- [ ] `main.cpp` - Adapter oluştur (service'i inject et)
- [ ] `main.cpp` - Pipeline oluştur ve kaydet
- [ ] `Makefile` - Yeni .cpp dosyalarını ekle

### ✅ Test

- [ ] Derleme başarılı
- [ ] start() çağrılıyor (log kontrolü)
- [ ] Veri alınıyor ve domain'e aktarılıyor
- [ ] stop() düzgün çalışıyor

---

## 📊 Veri Akışı Özeti

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        STARTUP SEQUENCE                                 │
│                                                                         │
│  1. main.cpp: auto service = make_shared<CustomDataService>()           │
│  2. main.cpp: auto adapter = make_shared<CustomIncomingAdapter>(service)│
│  3. main.cpp: pipeline.registerPipeline(adapter)                        │
│  4. main.cpp: adapter_manager.startAll()                                │
│       └──> pipeline.start()                                             │
│             └──> adapter->start()                                       │
│                   └──> Thread başlatılır: receiveLoop()                 │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│                        RUNTIME DATA FLOW                                │
│                                                                         │
│  [External System]                                                      │
│         │                                                               │
│         ▼ raw data                                                      │
│  CustomIncomingAdapter::receiveData()                                   │
│         │                                                               │
│         ▼ vector<uint8_t>                                               │
│  CustomIncomingAdapter::deserializeData()                               │
│         │                                                               │
│         ▼ CustomData object                                             │
│  incomingPort_->processCustomData(data)                                 │
│         │                                                               │
│         ▼                                                               │
│  CustomDataService::processCustomData()                                 │
│         │                                                               │
│         ▼ (iş mantığı uygulanır)                                        │
│  [İşlenmiş veri / Loglama / Outgoing port]                              │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Teknoloji Spesifik Uyarlama

Bu template'i farklı teknolojilere uyarlamak için sadece 3 metodu override edin:

| Metod | Açıklama | Örnek |
|-------|----------|-------|
| `initializeConnection()` | Bağlantı kurulumu | Kafka consumer, TCP socket |
| `receiveData()` | Veri alımı | consume(), recv(), read() |
| `closeConnection()` | Bağlantı kapatma | close(), disconnect() |

**Örnek: Kafka Consumer**

```cpp
class KafkaIncomingAdapter : public CustomIncomingAdapter {
protected:
    bool initializeConnection() override {
        consumer_ = std::make_unique<KafkaConsumer>(brokers_, topic_);
        return consumer_->connect();
    }

    bool receiveData(std::vector<uint8_t>& buffer) override {
        auto message = consumer_->poll(RECEIVE_TIMEOUT_MS);
        if (message) {
            buffer.assign(message->begin(), message->end());
            return true;
        }
        return false;
    }

    void closeConnection() override {
        consumer_->close();
    }
};
```