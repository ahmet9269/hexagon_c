# 🏗️ C_Hexagon - Yeni Outgoing Adapter Ekleme Rehberi

Bu dokümantasyon, c_hexagon projesine yeni bir **Outgoing Adapter** ekleme sürecini detaylı olarak açıklar.

---

## 📋 İçindekiler

1. [Hexagonal Architecture Genel Bakış](#hexagonal-architecture-genel-bakış)
2. [Outgoing Adapter Yapısı](#outgoing-adapter-yapısı)
3. [Oluşturulması Gereken Dosyalar](#oluşturulması-gereken-dosyalar)
4. [Adım Adım Uygulama](#adım-adım-uygulama)
5. [Entegrasyon (main.cpp)](#entegrasyon-maincpp)
6. [Makefile Güncellemesi](#makefile-güncellemesi)
7. [Örnek Kullanım Senaryoları](#örnek-kullanım-senaryoları)
8. [Checklist](#checklist)

---

## Hexagonal Architecture Genel Bakış

Hexagonal Architecture (Ports & Adapters), domain logic'i dış bağımlılıklardan izole eder:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         EXTERNAL SYSTEMS                                │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐              │
│   │  ZeroMQ  │  │   HTTP   │  │   File   │  │  gRPC    │              │
│   └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘              │
└────────┼─────────────┼─────────────┼─────────────┼──────────────────────┘
         │             │             │             │
         ▼             ▼             ▼             ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                      ADAPTERS LAYER (Outgoing)                          │
│   ┌──────────────┐ ┌──────────────┐ ┌──────────────┐ ┌──────────────┐  │
│   │ZMQOutAdapter │ │HTTPOutAdapter│ │FileOutAdapter│ │gRPCOutAdapter│  │
│   └──────┬───────┘ └──────┬───────┘ └──────┬───────┘ └──────┬───────┘  │
│          │                │                │                │          │
│          └────────────────┴────────────────┴────────────────┘          │
│                                    │                                    │
│                                    ▼                                    │
│                        implements IOutgoingPort                         │
└────────────────────────────────────┼────────────────────────────────────┘
                                     │
┌────────────────────────────────────┼────────────────────────────────────┐
│                         DOMAIN LAYER                                    │
│                                    ▼                                    │
│   ┌────────────────────────────────────────────────────────────────┐   │
│   │              IOutgoingPort (Secondary Port Interface)           │   │
│   │   - sendData(data): void                                        │   │
│   │   - isReady(): bool                                             │   │
│   └────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                    │
│                                    │ uses                               │
│   ┌────────────────────────────────────────────────────────────────┐   │
│   │                      Domain Service                             │   │
│   │   - Dependency Injection ile port alır                          │   │
│   │   - Concrete adapter'ı bilmez                                   │   │
│   └────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────┘
```

### Temel Prensipler

| Prensip | Açıklama |
|---------|----------|
| **Dependency Inversion** | Domain, adapter'a değil interface'e bağımlıdır |
| **Port** | Domain'in dış dünya ile iletişim kontratı |
| **Adapter** | Port'u implement eden concrete sınıf |
| **Secondary Port** | Outgoing (driven) portlar - domain'den dışarıya |

---

## Outgoing Adapter Yapısı

Bir Outgoing Adapter iki interface'i implement etmelidir:

### 1. IAdapter (Lifecycle Management)

```cpp
class IAdapter {
public:
    virtual ~IAdapter() = default;
    virtual bool start() = 0;           // Adapter'ı başlat
    virtual void stop() = 0;            // Adapter'ı durdur
    virtual bool isRunning() const = 0; // Çalışıyor mu?
    virtual std::string getName() const = 0; // Adapter adı
};
```

### 2. IOutgoingPort (Domain Contract)

```cpp
class ICustomOutgoingPort {
public:
    virtual ~ICustomOutgoingPort() = default;
    virtual void sendData(const DataType& data) = 0; // Veri gönder
    virtual bool isReady() const = 0;                // Hazır mı?
};
```

### Sınıf Diyagramı

```
┌─────────────────────────────────────────────────────────────┐
│                    CustomOutgoingAdapter                     │
├─────────────────────────────────────────────────────────────┤
│ - endpoint_: string                                          │
│ - channel_: string                                           │
│ - adapter_name_: string                                      │
│ - worker_thread_: thread                                     │
│ - running_: atomic<bool>                                     │
│ - ready_: atomic<bool>                                       │
│ - queue_mutex_: mutex                                        │
│ - queue_cv_: condition_variable                              │
│ - message_queue_: queue<DataType>                            │
├─────────────────────────────────────────────────────────────┤
│ + CustomOutgoingAdapter()                                    │
│ + CustomOutgoingAdapter(endpoint, channel)                   │
│ + ~CustomOutgoingAdapter()                                   │
│ ─────────────────────────────────────────────────────────── │
│ «IAdapter»                                                   │
│ + start(): bool                                              │
│ + stop(): void                                               │
│ + isRunning(): bool                                          │
│ + getName(): string                                          │
│ ─────────────────────────────────────────────────────────── │
│ «ICustomOutgoingPort»                                        │
│ + sendCustomData(data): void                                 │
│ + isReady(): bool                                            │
│ ─────────────────────────────────────────────────────────── │
│ - initializeConnection(): void                               │
│ - workerThread(): void                                       │
│ - enqueueMessage(data): void                                 │
└─────────────────────────────────────────────────────────────┘
                    △                     △
                    │                     │
                    │ implements          │ implements
                    │                     │
        ┌───────────┴───────┐   ┌────────┴────────────┐
        │     IAdapter      │   │ ICustomOutgoingPort │
        └───────────────────┘   └─────────────────────┘
```

---

## Oluşturulması Gereken Dosyalar

### Dosya Yapısı

```
src/c_hexagon/
├── adapters/
│   ├── common/
│   │   ├── IAdapter.hpp              ← Base interface (MEVCUT)
│   │   └── AdapterManager.hpp        ← Pipeline yönetimi (MEVCUT)
│   └── outgoing/
│       ├── zeromq/                   ← Mevcut ZMQ adapter (MEVCUT)
│       │   ├── FinalCalcTrackDataZeroMQOutgoingAdapter.hpp
│       │   └── FinalCalcTrackDataZeroMQOutgoingAdapter.cpp
│       └── custom/                   ← YENİ KLASÖR
│           ├── CustomOutgoingAdapter.hpp   ← YENİ
│           └── CustomOutgoingAdapter.cpp   ← YENİ
├── domain/
│   └── ports/
│       ├── CustomData.hpp            ← Data model (YENİ - opsiyonel)
│       └── outgoing/
│           ├── ITrackDataStatisticOutgoingPort.hpp  (MEVCUT)
│           └── ICustomOutgoingPort.hpp              ← YENİ
└── main.cpp                          ← Entegrasyon noktası
```

---

## Adım Adım Uygulama

### ADIM 1: Port Interface Oluşturma

**Dosya:** `src/c_hexagon/domain/ports/outgoing/ICustomOutgoingPort.hpp`

```cpp
/**
 * @file ICustomOutgoingPort.hpp
 * @brief Secondary port interface for custom outgoing data transmission
 * @details Hexagonal architecture'da secondary port tanımı.
 *          Domain katmanı sadece bu interface'i bilir.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#ifndef CUSTOM_OUTGOING_PORT_HPP
#define CUSTOM_OUTGOING_PORT_HPP

#include "domain/ports/CustomData.hpp"

namespace domain {
namespace ports {
namespace outgoing {

/**
 * @class ICustomOutgoingPort
 * @brief Secondary Port interface for custom data transmission
 * 
 * @par Hexagonal Architecture Role
 * Secondary Port - Domain ile dış dünya arasındaki sınırı tanımlar.
 * Outgoing adapter'lar bu interface'i implement eder.
 */
class ICustomOutgoingPort {
public:
    virtual ~ICustomOutgoingPort() = default;
    
    /**
     * @brief Send custom data to external system
     * @param data Custom data to be transmitted
     */
    virtual void sendCustomData(const domain::ports::CustomData& data) = 0;
    
    /**
     * @brief Check if the outgoing connection is ready
     * @return true if connection is ready
     */
    [[nodiscard]] virtual bool isReady() const = 0;
};

} // namespace outgoing
} // namespace ports
} // namespace domain

#endif // CUSTOM_OUTGOING_PORT_HPP
```

---

### ADIM 2: Data Model Oluşturma (Opsiyonel)

**Dosya:** `src/c_hexagon/domain/ports/CustomData.hpp`

```cpp
/**
 * @file CustomData.hpp
 * @brief Domain model for custom data
 * @details Gönderilecek veri yapısını tanımlar.
 */

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

namespace domain {
namespace ports {

/**
 * @class CustomData
 * @brief Custom veri modeli
 */
class CustomData final {
public:
    // ═══════════════════════════════════════════════════════════════════
    // Constructors / Destructor
    // ═══════════════════════════════════════════════════════════════════
    
    explicit CustomData() noexcept = default;
    ~CustomData() = default;
    
    // Copy/Move semantics
    CustomData(const CustomData&) = default;
    CustomData& operator=(const CustomData&) = default;
    CustomData(CustomData&&) noexcept = default;
    CustomData& operator=(CustomData&&) noexcept = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // Getters and Setters
    // ═══════════════════════════════════════════════════════════════════
    
    int32_t getId() const noexcept { return id_; }
    void setId(int32_t value) { id_ = value; }
    
    const std::string& getName() const noexcept { return name_; }
    void setName(const std::string& value) { name_ = value; }
    
    double getValue() const noexcept { return value_; }
    void setValue(double value) { value_ = value; }
    
    int64_t getTimestamp() const noexcept { return timestamp_; }
    void setTimestamp(int64_t value) { timestamp_ = value; }
    
    // ═══════════════════════════════════════════════════════════════════
    // Serialization
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Serialize data to binary format
     * @return Serialized byte vector
     */
    [[nodiscard]] std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;
        buffer.reserve(sizeof(id_) + sizeof(value_) + sizeof(timestamp_) + 
                       sizeof(uint32_t) + name_.size());
        
        // ID
        const uint8_t* id_ptr = reinterpret_cast<const uint8_t*>(&id_);
        buffer.insert(buffer.end(), id_ptr, id_ptr + sizeof(id_));
        
        // Value
        const uint8_t* val_ptr = reinterpret_cast<const uint8_t*>(&value_);
        buffer.insert(buffer.end(), val_ptr, val_ptr + sizeof(value_));
        
        // Timestamp
        const uint8_t* ts_ptr = reinterpret_cast<const uint8_t*>(&timestamp_);
        buffer.insert(buffer.end(), ts_ptr, ts_ptr + sizeof(timestamp_));
        
        // Name (length + data)
        uint32_t name_len = static_cast<uint32_t>(name_.size());
        const uint8_t* len_ptr = reinterpret_cast<const uint8_t*>(&name_len);
        buffer.insert(buffer.end(), len_ptr, len_ptr + sizeof(name_len));
        buffer.insert(buffer.end(), name_.begin(), name_.end());
        
        return buffer;
    }
    
    /**
     * @brief Deserialize from binary format
     * @param data Binary data
     * @return true if successful
     */
    bool deserialize(const std::vector<uint8_t>& data) noexcept {
        if (data.size() < sizeof(id_) + sizeof(value_) + sizeof(timestamp_) + sizeof(uint32_t)) {
            return false;
        }
        
        size_t offset = 0;
        
        // ID
        std::memcpy(&id_, data.data() + offset, sizeof(id_));
        offset += sizeof(id_);
        
        // Value
        std::memcpy(&value_, data.data() + offset, sizeof(value_));
        offset += sizeof(value_);
        
        // Timestamp
        std::memcpy(&timestamp_, data.data() + offset, sizeof(timestamp_));
        offset += sizeof(timestamp_);
        
        // Name
        uint32_t name_len;
        std::memcpy(&name_len, data.data() + offset, sizeof(name_len));
        offset += sizeof(name_len);
        
        if (data.size() < offset + name_len) {
            return false;
        }
        
        name_.assign(reinterpret_cast<const char*>(data.data() + offset), name_len);
        
        return true;
    }
    
    /**
     * @brief Validation check
     */
    [[nodiscard]] bool isValid() const noexcept {
        return id_ > 0;
    }

private:
    int32_t id_{0};
    std::string name_;
    double value_{0.0};
    int64_t timestamp_{0};
};

} // namespace ports
} // namespace domain
```

---

### ADIM 3: Outgoing Adapter Header

**Dosya:** `src/c_hexagon/adapters/outgoing/custom/CustomOutgoingAdapter.hpp`

```cpp
/**
 * @file CustomOutgoingAdapter.hpp
 * @brief Custom outgoing adapter for external system communication
 * @details Implements outgoing adapter in hexagonal architecture.
 *          Both IAdapter (lifecycle) and ICustomOutgoingPort (domain) interfaces.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/outgoing/ICustomOutgoingPort.hpp"
#include "domain/ports/CustomData.hpp"
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>

// Kullanılacak kütüphaneye göre include ekleyin:
// ZeroMQ için:
// #include <zmq_config.hpp>
// #include <zmq.hpp>
// #include <zmq_addon.hpp>

namespace adapters {
namespace outgoing {
namespace custom {

/**
 * @class CustomOutgoingAdapter
 * @brief Custom Outgoing Adapter Implementation
 * 
 * @details Thread-safe, non-blocking outgoing adapter.
 *          Background worker thread ile asenkron mesaj gönderimi.
 */
class CustomOutgoingAdapter 
    : public adapters::IAdapter
    , public domain::ports::outgoing::ICustomOutgoingPort
{
public:
    // ═══════════════════════════════════════════════════════════════════
    // Constructors / Destructor
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Default constructor
     */
    CustomOutgoingAdapter();
    
    /**
     * @brief Custom configuration constructor
     * @param endpoint Connection endpoint
     * @param channel_name Channel/group name
     */
    CustomOutgoingAdapter(
        const std::string& endpoint,
        const std::string& channel_name);
    
    /**
     * @brief Destructor
     */
    ~CustomOutgoingAdapter() override;
    
    // Delete copy (thread management)
    CustomOutgoingAdapter(const CustomOutgoingAdapter&) = delete;
    CustomOutgoingAdapter& operator=(const CustomOutgoingAdapter&) = delete;
    
    // ═══════════════════════════════════════════════════════════════════
    // IAdapter Interface
    // ═══════════════════════════════════════════════════════════════════
    
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const override;
    
    // ═══════════════════════════════════════════════════════════════════
    // ICustomOutgoingPort Interface
    // ═══════════════════════════════════════════════════════════════════
    
    void sendCustomData(const domain::ports::CustomData& data) override;
    [[nodiscard]] bool isReady() const override;

private:
    void initializeConnection();
    void workerThread();
    void enqueueMessage(const domain::ports::CustomData& data);

private:
    // Configuration
    static constexpr const char* DEFAULT_ENDPOINT = "tcp://127.0.0.1:15004";
    static constexpr const char* DEFAULT_CHANNEL = "CustomData";
    static constexpr int SEND_TIMEOUT_MS = 100;
    static constexpr std::size_t MAX_QUEUE_SIZE = 1000;
    
    // Members
    std::string endpoint_;
    std::string channel_;
    std::string adapter_name_;
    
    // Thread management
    std::thread worker_thread_;
    std::atomic<bool> running_;
    std::atomic<bool> ready_;
    
    // Message queue
    mutable std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::queue<domain::ports::CustomData> message_queue_;
};

} // namespace custom
} // namespace outgoing
} // namespace adapters
```

---

### ADIM 4: Outgoing Adapter Implementation

**Dosya:** `src/c_hexagon/adapters/outgoing/custom/CustomOutgoingAdapter.cpp`

```cpp
/**
 * @file CustomOutgoingAdapter.cpp
 * @brief Implementation of Custom Outgoing Adapter
 */

#include "CustomOutgoingAdapter.hpp"
#include "utils/Logger.hpp"
#include <chrono>

namespace adapters {
namespace outgoing {
namespace custom {

// ═══════════════════════════════════════════════════════════════════════════
// Constructors / Destructor
// ═══════════════════════════════════════════════════════════════════════════

CustomOutgoingAdapter::CustomOutgoingAdapter()
    : endpoint_(DEFAULT_ENDPOINT)
    , channel_(DEFAULT_CHANNEL)
    , adapter_name_("CustomData-OutAdapter")
    , running_(false)
    , ready_(false)
{
    initializeConnection();
}

CustomOutgoingAdapter::CustomOutgoingAdapter(
    const std::string& endpoint,
    const std::string& channel_name)
    : endpoint_(endpoint)
    , channel_(channel_name)
    , adapter_name_(channel_name + "-OutAdapter")
    , running_(false)
    , ready_(false)
{
    initializeConnection();
}

CustomOutgoingAdapter::~CustomOutgoingAdapter() {
    stop();
}

// ═══════════════════════════════════════════════════════════════════════════
// Private Methods
// ═══════════════════════════════════════════════════════════════════════════

void CustomOutgoingAdapter::initializeConnection() {
    try {
        LOG_INFO("Initializing Custom Outgoing Adapter - Endpoint: {}, Channel: {}", 
                 endpoint_, channel_);
        
        // ═══════════════════════════════════════════════════════════════
        // CONNECTION INITIALIZATION
        // Kullanılacak protokole göre burayı düzenleyin
        // ═══════════════════════════════════════════════════════════════
        
        // ZeroMQ örneği:
        // zmq_context_ = zmq::context_t(1);
        // socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::radio);
        // socket_->set(zmq::sockopt::sndhwm, 1000);
        // socket_->connect(endpoint_);
        
        // HTTP örneği:
        // http_client_ = std::make_unique<HttpClient>(endpoint_);
        
        // File örneği:
        // output_file_.open(endpoint_, std::ios::binary);
        
        ready_ = true;
        LOG_INFO("Custom Outgoing Adapter initialized successfully");
        
    } catch (const std::exception& e) {
        LOG_ERROR("Initialization error: {}", e.what());
        ready_ = false;
        throw;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// IAdapter Interface Implementation
// ═══════════════════════════════════════════════════════════════════════════

bool CustomOutgoingAdapter::start() {
    if (running_.load()) {
        LOG_WARN("Adapter already running: {}", adapter_name_);
        return false;
    }
    
    running_ = true;
    
    worker_thread_ = std::thread([this]() {
        workerThread();
    });
    
    LOG_INFO("Custom Outgoing Adapter started: {}", adapter_name_);
    return true;
}

void CustomOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    LOG_INFO("Stopping Custom Outgoing Adapter: {}", adapter_name_);
    running_ = false;
    ready_ = false;
    
    queue_cv_.notify_all();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    LOG_INFO("Custom Outgoing Adapter stopped: {}", adapter_name_);
}

bool CustomOutgoingAdapter::isRunning() const {
    return running_.load();
}

std::string CustomOutgoingAdapter::getName() const {
    return adapter_name_;
}

// ═══════════════════════════════════════════════════════════════════════════
// ICustomOutgoingPort Interface Implementation
// ═══════════════════════════════════════════════════════════════════════════

bool CustomOutgoingAdapter::isReady() const {
    return ready_.load() && running_.load();
}

void CustomOutgoingAdapter::sendCustomData(const domain::ports::CustomData& data) {
    if (!isReady()) {
        LOG_WARN("Adapter not ready, dropping message - ID: {}", data.getId());
        return;
    }
    
    enqueueMessage(data);
}

void CustomOutgoingAdapter::enqueueMessage(const domain::ports::CustomData& data) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        
        if (message_queue_.size() >= MAX_QUEUE_SIZE) {
            LOG_WARN("Message queue full, dropping oldest message");
            message_queue_.pop();
        }
        
        message_queue_.push(data);
    }
    
    queue_cv_.notify_one();
}

void CustomOutgoingAdapter::workerThread() {
    LOG_DEBUG("Worker thread started");
    
    while (running_.load()) {
        domain::ports::CustomData data;
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            if (queue_cv_.wait_for(lock, std::chrono::milliseconds(100),
                [this]() { return !message_queue_.empty() || !running_.load(); })) {
                
                if (!running_.load()) {
                    break;
                }
                
                if (!message_queue_.empty()) {
                    data = std::move(message_queue_.front());
                    message_queue_.pop();
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }
        
        try {
            std::vector<uint8_t> serialized = data.serialize();
            
            // ═══════════════════════════════════════════════════════════
            // SEND IMPLEMENTATION
            // Kullanılacak protokole göre burayı düzenleyin
            // ═══════════════════════════════════════════════════════════
            
            // ZeroMQ örneği:
            // zmq::message_t msg(serialized.data(), serialized.size());
            // msg.set_group(channel_.c_str());
            // socket_->send(msg, zmq::send_flags::dontwait);
            
            // HTTP örneği:
            // http_client_->post("/api/data", serialized);
            
            // File örneği:
            // output_file_.write(reinterpret_cast<char*>(serialized.data()), 
            //                    serialized.size());
            
            LOG_DEBUG("[CustomAdapter] Data sent - ID: {}, Size: {} bytes",
                     data.getId(), serialized.size());
                     
        } catch (const std::exception& e) {
            LOG_ERROR("Send error: {}", e.what());
        }
    }
    
    LOG_DEBUG("Worker thread stopped");
}

} // namespace custom
} // namespace outgoing
} // namespace adapters
```

---

## Entegrasyon (main.cpp)

### Temel Entegrasyon Adımları

```cpp
// ═══════════════════════════════════════════════════════════════════════════
// ADIM 1: Include dosyasını ekle
// ═══════════════════════════════════════════════════════════════════════════
#include "adapters/outgoing/custom/CustomOutgoingAdapter.hpp"

// main() fonksiyonu içinde:

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 2: Adapter oluştur
// ═══════════════════════════════════════════════════════════════════════════

// Seçenek A: Default configuration
auto custom_outgoing = std::make_shared<
    adapters::outgoing::custom::CustomOutgoingAdapter>();

// Seçenek B: Custom configuration
auto custom_outgoing = std::make_shared<
    adapters::outgoing::custom::CustomOutgoingAdapter>(
    "tcp://192.168.1.100:15004",  // endpoint
    "CustomDataChannel"           // channel name
);

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 3: Domain Service'e inject et (Dependency Inversion)
// ═══════════════════════════════════════════════════════════════════════════
auto custom_service = std::make_shared<domain::logic::CustomDataService>(
    custom_outgoing  // ICustomOutgoingPort olarak inject
);

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 4: Incoming Adapter oluştur (varsa)
// ═══════════════════════════════════════════════════════════════════════════
auto custom_incoming = std::make_shared<
    adapters::incoming::custom::CustomIncomingAdapter>(
    custom_service,
    "tcp://127.0.0.1:15005",
    "CustomInput"
);

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 5: Pipeline oluştur ve kaydet
// ═══════════════════════════════════════════════════════════════════════════
adapters::MessagePipeline custom_pipeline(
    "CustomData",
    custom_incoming,  // nullptr olabilir (sadece outgoing)
    custom_outgoing
);

adapter_manager.registerPipeline(std::move(custom_pipeline));
```

### Sadece Outgoing Adapter Kullanımı

```cpp
// Incoming adapter olmadan sadece outgoing kullanmak için:
adapters::MessagePipeline outgoing_only_pipeline(
    "CustomDataOut",
    nullptr,         // Incoming yok
    custom_outgoing  // Sadece outgoing
);

adapter_manager.registerPipeline(std::move(outgoing_only_pipeline));
```

---

## Makefile Güncellemesi

Yeni .cpp dosyasını Makefile'a ekleyin:

```makefile
# Source files listesine ekle
SRCS += src/c_hexagon/adapters/outgoing/custom/CustomOutgoingAdapter.cpp

# Veya ayrı değişken olarak
CUSTOM_ADAPTER_SRC = src/c_hexagon/adapters/outgoing/custom/CustomOutgoingAdapter.cpp
```

---

## Örnek Kullanım Senaryoları

### Senaryo 1: ZeroMQ RADIO/DISH Adapter

```cpp
// UDP Multicast ile veri gönderimi
auto zmq_adapter = std::make_shared<ZeroMQOutgoingAdapter>(
    "udp://239.1.1.5:9597",
    "TrackDataGroup"
);
```

### Senaryo 2: HTTP REST Adapter

```cpp
// REST API'ye POST isteği
auto http_adapter = std::make_shared<HttpOutgoingAdapter>(
    "https://api.example.com",
    "/api/v1/data"
);
```

### Senaryo 3: File Writer Adapter

```cpp
// Binary dosyaya yazma
auto file_adapter = std::make_shared<FileOutgoingAdapter>(
    "/var/log/data/output.bin",
    FileMode::Binary
);
```

### Senaryo 4: gRPC Adapter

```cpp
// gRPC stream ile veri gönderimi
auto grpc_adapter = std::make_shared<GrpcOutgoingAdapter>(
    "grpc://server:50051",
    "DataService"
);
```

---

## Checklist

Yeni bir Outgoing Adapter eklerken aşağıdaki adımları takip edin:

| # | Adım | Dosya | Durum |
|---|------|-------|-------|
| 1 | Port Interface oluştur | `domain/ports/outgoing/ICustomOutgoingPort.hpp` | ☐ |
| 2 | Data Model oluştur (opsiyonel) | `domain/ports/CustomData.hpp` | ☐ |
| 3 | Adapter Header oluştur | `adapters/outgoing/custom/CustomOutgoingAdapter.hpp` | ☐ |
| 4 | Adapter Implementation oluştur | `adapters/outgoing/custom/CustomOutgoingAdapter.cpp` | ☐ |
| 5 | Makefile'a ekle | `Makefile` | ☐ |
| 6 | main.cpp'de adapter oluştur | `main.cpp` | ☐ |
| 7 | Service'e inject et | `main.cpp` | ☐ |
| 8 | Pipeline oluştur ve kaydet | `main.cpp` | ☐ |
| 9 | Derleme testi | `make clean && make` | ☐ |
| 10 | Çalışma testi | `./bin/c_hexagon` | ☐ |

---

## Referanslar

- [Hexagonal Architecture (Ports & Adapters)](https://alistair.cockburn.us/hexagonal-architecture/)
- [Clean Architecture by Robert C. Martin](https://blog.cleancoder.com/uncle-bob/2012/08/13/the-clean-architecture.html)
- [ZeroMQ RADIO/DISH Pattern](https://zeromq.org/socket-api/#radio-dish-pattern)
- [MISRA C++ 2023 Guidelines](https://www.misra.org.uk/)

---

**Hazırlayan:** c_hexagon Team  
**Versiyon:** 1.0  
**Tarih:** Aralık 2025
