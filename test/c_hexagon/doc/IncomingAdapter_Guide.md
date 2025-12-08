# 🏗️ C_Hexagon - Yeni Incoming Adapter Ekleme Rehberi

Bu dokümantasyon, c_hexagon projesine yeni bir **Incoming Adapter** ekleme sürecini detaylı olarak açıklar.

---

## 📋 İçindekiler

1. [Hexagonal Architecture'da Incoming Adapter](#hexagonal-architectureda-incoming-adapter)
2. [Incoming vs Outgoing Adapter Farkları](#incoming-vs-outgoing-adapter-farkları)
3. [Incoming Adapter Yapısı](#incoming-adapter-yapısı)
4. [Oluşturulması Gereken Dosyalar](#oluşturulması-gereken-dosyalar)
5. [Adım Adım Uygulama](#adım-adım-uygulama)
6. [Entegrasyon (main.cpp)](#entegrasyon-maincpp)
7. [Makefile Güncellemesi](#makefile-güncellemesi)
8. [Örnek Kullanım Senaryoları](#örnek-kullanım-senaryoları)
9. [Checklist](#checklist)

---

## Hexagonal Architecture'da Incoming Adapter

Incoming Adapter (Primary Adapter), dış dünyadan gelen istekleri alıp domain katmanına ileten bileşendir:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         EXTERNAL SYSTEMS                                │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐              │
│   │  ZeroMQ  │  │   HTTP   │  │   File   │  │  gRPC    │              │
│   │  RADIO   │  │  Request │  │  Reader  │  │  Client  │              │
│   └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘              │
└────────┼─────────────┼─────────────┼─────────────┼──────────────────────┘
         │             │             │             │
         ▼             ▼             ▼             ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                      ADAPTERS LAYER (Incoming)                          │
│   ┌──────────────┐ ┌──────────────┐ ┌──────────────┐ ┌──────────────┐  │
│   │ZMQInAdapter  │ │HTTPInAdapter │ │FileInAdapter │ │gRPCInAdapter │  │
│   └──────┬───────┘ └──────┬───────┘ └──────┬───────┘ └──────┬───────┘  │
│          │                │                │                │          │
│          └────────────────┴────────────────┴────────────────┘          │
│                                    │                                    │
│                                    ▼                                    │
│                            calls IIncomingPort                          │
└────────────────────────────────────┼────────────────────────────────────┘
                                     │
┌────────────────────────────────────┼────────────────────────────────────┐
│                         DOMAIN LAYER                                    │
│                                    ▼                                    │
│   ┌────────────────────────────────────────────────────────────────┐   │
│   │              IIncomingPort (Primary Port Interface)             │   │
│   │   - submitData(data): void                                      │   │
│   │   - Domain Service implements this interface                    │   │
│   └────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                    │
│                                    │ implements                         │
│   ┌────────────────────────────────────────────────────────────────┐   │
│   │                      Domain Service                             │   │
│   │   - submitData() metodunu implement eder                        │   │
│   │   - İş mantığını uygular                                        │   │
│   │   - Outgoing port üzerinden çıktı verir                         │   │
│   └────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────┘
```

### Veri Akış Yönü

```
┌─────────────┐     ┌─────────────────┐     ┌─────────────────┐     ┌─────────────┐
│   Dış       │     │    Incoming     │     │    Domain       │     │  Outgoing   │
│   Sistem    │ ──▶ │    Adapter      │ ──▶ │    Service      │ ──▶ │  Adapter    │
│ (Publisher) │     │ (DISH/Receiver) │     │ (Port Impl)     │     │ (RADIO/Send)│
└─────────────┘     └─────────────────┘     └─────────────────┘     └─────────────┘
       │                    │                       │                      │
       │                    │                       │                      │
    ZMQ RADIO           Deserialize            Business Logic          Serialize
    HTTP POST           + Validate             + Processing            + Send
    File Read                                  + Transform
```

---

## Incoming vs Outgoing Adapter Farkları

| Özellik | Incoming Adapter | Outgoing Adapter |
|---------|------------------|------------------|
| **Yön** | Dış dünya → Domain | Domain → Dış dünya |
| **Port Tipi** | Primary Port | Secondary Port |
| **Port'u Kim Implement Eder?** | Domain Service | Adapter |
| **Adapter Ne Yapar?** | Port'u ÇAĞIRIR | Port'u IMPLEMENT EDER |
| **ZMQ Socket** | DISH (bind/receive) | RADIO (connect/send) |
| **HTTP** | Server (listen) | Client (request) |
| **Veri İşleme** | Deserialize | Serialize |

### Interface Ownership

```
INCOMING ADAPTER:
┌─────────────────────┐          ┌─────────────────────┐
│  Incoming Adapter   │          │   Domain Service    │
│                     │  calls   │                     │
│  - Has reference to │ ───────▶ │  - Implements       │
│    IIncomingPort    │          │    IIncomingPort    │
└─────────────────────┘          └─────────────────────┘

OUTGOING ADAPTER:
┌─────────────────────┐          ┌─────────────────────┐
│   Domain Service    │          │  Outgoing Adapter   │
│                     │  calls   │                     │
│  - Has reference to │ ───────▶ │  - Implements       │
│    IOutgoingPort    │          │    IOutgoingPort    │
└─────────────────────┘          └─────────────────────┘
```

---

## Incoming Adapter Yapısı

Incoming Adapter sadece `IAdapter` interface'ini implement eder, ancak constructor'da `IIncomingPort` referansı alır:

### 1. IAdapter (Lifecycle Management)

```cpp
class IAdapter {
public:
    virtual ~IAdapter() = default;
    virtual bool start() = 0;           // Adapter'ı başlat (listening)
    virtual void stop() = 0;            // Adapter'ı durdur
    virtual bool isRunning() const = 0; // Çalışıyor mu?
    virtual std::string getName() const = 0; // Adapter adı
};
```

### 2. IIncomingPort (Domain Contract - Domain Service Implement Eder)

```cpp
class ICustomIncomingPort {
public:
    virtual ~ICustomIncomingPort() = default;
    virtual void submitCustomData(const CustomData& data) = 0; // Veri al
};
```

### Sınıf Diyagramı

```
┌─────────────────────────────────────────────────────────────┐
│                    CustomIncomingAdapter                     │
├─────────────────────────────────────────────────────────────┤
│ - domain_port_: shared_ptr<ICustomIncomingPort>              │ ◆──────┐
│ - endpoint_: string                                          │        │
│ - channel_: string                                           │        │
│ - adapter_name_: string                                      │        │
│ - listener_thread_: thread                                   │        │
│ - running_: atomic<bool>                                     │        │
├─────────────────────────────────────────────────────────────┤        │
│ + CustomIncomingAdapter(port)                                │        │
│ + CustomIncomingAdapter(port, endpoint, channel)             │        │
│ + ~CustomIncomingAdapter()                                   │        │
│ ─────────────────────────────────────────────────────────── │        │
│ «IAdapter»                                                   │        │
│ + start(): bool                                              │        │
│ + stop(): void                                               │        │
│ + isRunning(): bool                                          │        │
│ + getName(): string                                          │        │
│ ─────────────────────────────────────────────────────────── │        │
│ - initializeConnection(): void                               │        │
│ - listenerWorker(): void                                     │        │
│ - deserializeData(bytes): optional<CustomData>               │        │
└─────────────────────────────────────────────────────────────┘        │
                    △                                                   │
                    │                                                   │
                    │ implements                                        │
                    │                                                   │
        ┌───────────┴───────┐                                          │
        │     IAdapter      │                                          │
        └───────────────────┘                                          │
                                                                        │
                                                                        │
┌───────────────────────────────────────────────────────────────────────┘
│  calls (has reference to)
▼
┌─────────────────────────────────────────────────────────────┐
│                   ICustomIncomingPort                        │
│                     (Primary Port)                           │
├─────────────────────────────────────────────────────────────┤
│ + submitCustomData(data): void                               │
└─────────────────────────────────────────────────────────────┘
                    △
                    │
                    │ implements
                    │
┌─────────────────────────────────────────────────────────────┐
│                   CustomDataService                          │
│                    (Domain Logic)                            │
├─────────────────────────────────────────────────────────────┤
│ - outgoing_port_: shared_ptr<ICustomOutgoingPort>            │
│ + submitCustomData(data): void   ← İş mantığı burada        │
└─────────────────────────────────────────────────────────────┘
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
│   └── incoming/
│       ├── zeromq/                   ← Mevcut ZMQ adapter (MEVCUT)
│       │   ├── TrackDataZeroMQIncomingAdapter.hpp
│       │   └── TrackDataZeroMQIncomingAdapter.cpp
│       └── custom/                   ← YENİ KLASÖR
│           ├── CustomIncomingAdapter.hpp   ← YENİ
│           └── CustomIncomingAdapter.cpp   ← YENİ
├── domain/
│   ├── logic/
│   │   └── CustomDataService.hpp     ← Domain Service (Port'u implement eder)
│   │   └── CustomDataService.cpp
│   └── ports/
│       ├── CustomData.hpp            ← Data model (MEVCUT veya YENİ)
│       └── incoming/
│           ├── IDelayCalcTrackDataIncomingPort.hpp  (MEVCUT)
│           └── ICustomIncomingPort.hpp              ← YENİ
└── main.cpp                          ← Entegrasyon noktası
```

---

## Adım Adım Uygulama

### ADIM 1: Incoming Port Interface Oluşturma

**Dosya:** `src/c_hexagon/domain/ports/incoming/ICustomIncomingPort.hpp`

```cpp
/**
 * @file ICustomIncomingPort.hpp
 * @brief Primary port interface for incoming custom data
 * @details Hexagonal architecture'da primary port tanımı.
 *          DOMAIN SERVICE bu interface'i IMPLEMENT EDER.
 *          Incoming adapter bu interface'i ÇAĞIRIR.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#ifndef CUSTOM_INCOMING_PORT_HPP
#define CUSTOM_INCOMING_PORT_HPP

#include "domain/ports/CustomData.hpp"

namespace domain {
namespace ports {
namespace incoming {

/**
 * @class ICustomIncomingPort
 * @brief Primary Port interface for incoming custom data
 * 
 * @par Hexagonal Architecture Role
 * Primary Port - Dış dünya ile domain logic arasındaki sınırı tanımlar.
 * Domain service'ler bu interface'i implement eder.
 * Incoming adapter'lar bu interface'i çağırır.
 * 
 * @par Usage Example
 * @code
 * // Domain Service - PORT'U IMPLEMENT EDER
 * class CustomDataService : public ICustomIncomingPort {
 * public:
 *     void submitCustomData(const CustomData& data) override {
 *         // İş mantığını uygula
 *         processData(data);
 *         // Çıktıyı outgoing port üzerinden gönder
 *         outgoing_port_->sendResult(result);
 *     }
 * };
 * 
 * // Incoming Adapter - PORT'U ÇAĞIRIR
 * class CustomIncomingAdapter {
 * public:
 *     CustomIncomingAdapter(shared_ptr<ICustomIncomingPort> port)
 *         : domain_port_(port) {}
 *     
 *     void onDataReceived(const CustomData& data) {
 *         domain_port_->submitCustomData(data);  // Domain'e ilet
 *     }
 * };
 * @endcode
 */
class ICustomIncomingPort {
public:
    virtual ~ICustomIncomingPort() = default;
    
    /**
     * @brief Receive and process incoming custom data
     * @param data Custom data received from external system
     * 
     * @note Bu metod DOMAIN SERVICE tarafından implement edilir
     * @note Incoming adapter bu metodu çağırarak veriyi domain'e iletir
     */
    virtual void submitCustomData(const domain::ports::CustomData& data) = 0;
};

} // namespace incoming
} // namespace ports
} // namespace domain

#endif // CUSTOM_INCOMING_PORT_HPP
```

---

### ADIM 2: Domain Service Oluşturma (Port'u Implement Eden)

**Dosya:** `src/c_hexagon/domain/logic/CustomDataService.hpp`

```cpp
/**
 * @file CustomDataService.hpp
 * @brief Domain service that implements ICustomIncomingPort
 * @details İş mantığını içerir ve incoming port'u implement eder.
 *          Outgoing port üzerinden sonuçları dış dünyaya gönderir.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "domain/ports/incoming/ICustomIncomingPort.hpp"
#include "domain/ports/outgoing/ICustomOutgoingPort.hpp"
#include "domain/ports/CustomData.hpp"
#include <memory>

namespace domain {
namespace logic {

/**
 * @class CustomDataService
 * @brief Domain service implementing business logic
 * 
 * @details 
 * - ICustomIncomingPort implement eder (incoming adapter çağırır)
 * - ICustomOutgoingPort kullanır (outgoing adapter'a veri gönderir)
 * 
 * @par Hexagonal Architecture Flow
 * 1. Incoming Adapter → submitCustomData() çağırır
 * 2. Service iş mantığını uygular
 * 3. Service → outgoing_port_->sendCustomData() çağırır
 * 4. Outgoing Adapter veriyi dış sisteme gönderir
 */
class CustomDataService 
    : public domain::ports::incoming::ICustomIncomingPort
{
public:
    /**
     * @brief Constructor with dependency injection
     * @param outgoing_port Outgoing port for sending processed data
     * 
     * @note Dependency Inversion: Service concrete adapter'ı değil,
     *       abstract port interface'ini alır
     */
    explicit CustomDataService(
        std::shared_ptr<domain::ports::outgoing::ICustomOutgoingPort> outgoing_port)
        : outgoing_port_(outgoing_port)
    {}
    
    ~CustomDataService() override = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // ICustomIncomingPort Implementation
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Process incoming custom data
     * @param data Incoming data from external system
     * 
     * @details
     * 1. Veriyi validate et
     * 2. İş mantığını uygula (transform, calculate, etc.)
     * 3. Sonucu outgoing port üzerinden gönder
     */
    void submitCustomData(const domain::ports::CustomData& data) override {
        // ═══════════════════════════════════════════════════════════════
        // ADIM 1: Validation
        // ═══════════════════════════════════════════════════════════════
        if (!data.isValid()) {
            LOG_WARN("Invalid data received, skipping - ID: {}", data.getId());
            return;
        }
        
        // ═══════════════════════════════════════════════════════════════
        // ADIM 2: Business Logic
        // ═══════════════════════════════════════════════════════════════
        
        // Örnek: Veriyi işle, dönüştür
        domain::ports::CustomData processed_data = processData(data);
        
        // ═══════════════════════════════════════════════════════════════
        // ADIM 3: Output via Outgoing Port
        // ═══════════════════════════════════════════════════════════════
        if (outgoing_port_ && outgoing_port_->isReady()) {
            outgoing_port_->sendCustomData(processed_data);
            LOG_DEBUG("Processed data sent - ID: {}", processed_data.getId());
        } else {
            LOG_WARN("Outgoing port not ready, data not sent - ID: {}", data.getId());
        }
    }

private:
    /**
     * @brief Apply business logic to data
     * @param data Input data
     * @return Processed data
     */
    domain::ports::CustomData processData(const domain::ports::CustomData& data) {
        domain::ports::CustomData result = data;
        
        // ═══════════════════════════════════════════════════════════════
        // İŞ MANTIĞI BURAYA GELECEK
        // ═══════════════════════════════════════════════════════════════
        
        // Örnek: Değeri 2 ile çarp
        result.setValue(data.getValue() * 2.0);
        
        // Örnek: Timestamp güncelle
        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()).count();
        result.setTimestamp(timestamp);
        
        return result;
    }

private:
    /// Outgoing port for sending processed data
    std::shared_ptr<domain::ports::outgoing::ICustomOutgoingPort> outgoing_port_;
};

} // namespace logic
} // namespace domain
```

---

### ADIM 3: Incoming Adapter Header

**Dosya:** `src/c_hexagon/adapters/incoming/custom/CustomIncomingAdapter.hpp`

```cpp
/**
 * @file CustomIncomingAdapter.hpp
 * @brief Custom incoming adapter for receiving data from external systems
 * @details Implements incoming adapter in hexagonal architecture.
 *          Receives data and forwards to domain via ICustomIncomingPort.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/incoming/ICustomIncomingPort.hpp"
#include "domain/ports/CustomData.hpp"
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <optional>

// Kullanılacak kütüphaneye göre include ekleyin:
// ZeroMQ için:
// #include <zmq_config.hpp>
// #include <zmq.hpp>
// #include <zmq_addon.hpp>

namespace adapters {
namespace incoming {
namespace custom {

/**
 * @class CustomIncomingAdapter
 * @brief Custom Incoming Adapter Implementation
 * 
 * @details 
 * - IAdapter interface'ini implement eder (lifecycle management)
 * - ICustomIncomingPort'u ÇAĞIRIR (domain'e veri iletir)
 * - Background thread ile asenkron mesaj alımı
 * 
 * @par Key Difference from Outgoing Adapter
 * - Outgoing: Port interface'ini IMPLEMENT EDER
 * - Incoming: Port interface'ini ÇAĞIRIR (constructor'da alır)
 */
class CustomIncomingAdapter : public adapters::IAdapter {
public:
    // ═══════════════════════════════════════════════════════════════════
    // Constructors / Destructor
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Constructor with domain port
     * @param domain_port Port for forwarding data to domain layer
     * 
     * @note Domain port, domain service tarafından implement edilir
     * @note Adapter bu port üzerinden domain'e veri iletir
     */
    explicit CustomIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ICustomIncomingPort> domain_port);
    
    /**
     * @brief Constructor with custom configuration
     * @param domain_port Port for forwarding data to domain layer
     * @param endpoint Connection endpoint (e.g., "udp://239.1.1.5:9001")
     * @param channel_name Channel/group name for subscription
     */
    CustomIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ICustomIncomingPort> domain_port,
        const std::string& endpoint,
        const std::string& channel_name);
    
    /**
     * @brief Destructor
     */
    ~CustomIncomingAdapter() override;
    
    // Delete copy (thread management)
    CustomIncomingAdapter(const CustomIncomingAdapter&) = delete;
    CustomIncomingAdapter& operator=(const CustomIncomingAdapter&) = delete;
    
    // ═══════════════════════════════════════════════════════════════════
    // IAdapter Interface
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Start listening for incoming data
     * @return true if started successfully
     */
    [[nodiscard]] bool start() override;
    
    /**
     * @brief Stop listening
     */
    void stop() override;
    
    /**
     * @brief Check if adapter is running
     * @return true if running
     */
    [[nodiscard]] bool isRunning() const override;
    
    /**
     * @brief Get adapter name for logging
     * @return Adapter identifier
     */
    [[nodiscard]] std::string getName() const override;

private:
    // ═══════════════════════════════════════════════════════════════════
    // Private Helper Methods
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Initialize connection/socket for receiving
     */
    void initializeConnection();
    
    /**
     * @brief Background worker thread for message reception
     */
    void listenerWorker();
    
    /**
     * @brief Deserialize binary data to CustomData
     * @param binary_data Raw binary data
     * @return Optional containing deserialized data if successful
     */
    std::optional<domain::ports::CustomData> deserializeData(
        const std::vector<uint8_t>& binary_data);

private:
    // ═══════════════════════════════════════════════════════════════════
    // Configuration Constants
    // ═══════════════════════════════════════════════════════════════════
    
    static constexpr const char* DEFAULT_ENDPOINT = "udp://239.1.1.5:9001";
    static constexpr const char* DEFAULT_CHANNEL = "CustomData";
    static constexpr int RECEIVE_TIMEOUT_MS = 100;
    static constexpr int LINGER_MS = 0;
    static constexpr int HIGH_WATER_MARK = 0;  // Unlimited
    
    // Real-time thread configuration (Linux)
    static constexpr int REALTIME_THREAD_PRIORITY = 95;
    static constexpr int DEDICATED_CPU_CORE = 2;
    
    // ═══════════════════════════════════════════════════════════════════
    // Member Variables
    // ═══════════════════════════════════════════════════════════════════
    
    /// Domain port for forwarding received data (ÇAĞIRILIR, implement edilmez)
    std::shared_ptr<domain::ports::incoming::ICustomIncomingPort> domain_port_;
    
    // Configuration
    std::string endpoint_;
    std::string channel_;
    std::string adapter_name_;
    
    // Connection/Socket components (örnek: ZMQ için)
    // zmq::context_t zmq_context_;
    // std::unique_ptr<zmq::socket_t> dish_socket_;
    
    // Thread management
    std::thread listener_thread_;
    std::atomic<bool> running_;
};

} // namespace custom
} // namespace incoming
} // namespace adapters
```

---

### ADIM 4: Incoming Adapter Implementation

**Dosya:** `src/c_hexagon/adapters/incoming/custom/CustomIncomingAdapter.cpp`

```cpp
/**
 * @file CustomIncomingAdapter.cpp
 * @brief Implementation of Custom Incoming Adapter
 */

#include "CustomIncomingAdapter.hpp"
#include "utils/Logger.hpp"
#include <chrono>

namespace adapters {
namespace incoming {
namespace custom {

// ═══════════════════════════════════════════════════════════════════════════
// Constructors / Destructor
// ═══════════════════════════════════════════════════════════════════════════

CustomIncomingAdapter::CustomIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::ICustomIncomingPort> domain_port)
    : domain_port_(domain_port)
    , endpoint_(DEFAULT_ENDPOINT)
    , channel_(DEFAULT_CHANNEL)
    , adapter_name_("CustomData-InAdapter")
    , running_(false)
{
    initializeConnection();
}

CustomIncomingAdapter::CustomIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::ICustomIncomingPort> domain_port,
    const std::string& endpoint,
    const std::string& channel_name)
    : domain_port_(domain_port)
    , endpoint_(endpoint)
    , channel_(channel_name)
    , adapter_name_(channel_name + "-InAdapter")
    , running_(false)
{
    initializeConnection();
}

CustomIncomingAdapter::~CustomIncomingAdapter() {
    stop();
}

// ═══════════════════════════════════════════════════════════════════════════
// Private Methods
// ═══════════════════════════════════════════════════════════════════════════

void CustomIncomingAdapter::initializeConnection() {
    try {
        LOG_INFO("Initializing Custom Incoming Adapter - Endpoint: {}, Channel: {}", 
                 endpoint_, channel_);
        
        // ═══════════════════════════════════════════════════════════════
        // CONNECTION INITIALIZATION - RECEIVER/LISTENER
        // Kullanılacak protokole göre burayı düzenleyin
        // ═══════════════════════════════════════════════════════════════
        
        // ZeroMQ DISH örneği (receiver):
        // zmq_context_ = zmq::context_t(1);
        // dish_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::dish);
        // dish_socket_->set(zmq::sockopt::rcvhwm, HIGH_WATER_MARK);
        // dish_socket_->set(zmq::sockopt::rcvtimeo, RECEIVE_TIMEOUT_MS);
        // dish_socket_->set(zmq::sockopt::linger, LINGER_MS);
        // dish_socket_->bind(endpoint_);      // DISH binds (listens)
        // dish_socket_->join(channel_.c_str()); // Subscribe to group
        
        // HTTP Server örneği:
        // http_server_ = std::make_unique<HttpServer>(endpoint_);
        // http_server_->route("/api/data", [this](Request& req) {
        //     onDataReceived(req.body());
        // });
        
        // File Watcher örneği:
        // file_watcher_ = std::make_unique<FileWatcher>(endpoint_);
        
        LOG_INFO("Custom Incoming Adapter initialized successfully");
        
    } catch (const std::exception& e) {
        LOG_ERROR("Initialization error: {}", e.what());
        throw;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// IAdapter Interface Implementation
// ═══════════════════════════════════════════════════════════════════════════

bool CustomIncomingAdapter::start() {
    if (running_.load()) {
        LOG_WARN("Adapter already running: {}", adapter_name_);
        return false;
    }
    
    running_ = true;
    
    // Start background listener thread
    listener_thread_ = std::thread([this]() {
        // Configure real-time thread priority on Linux
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = REALTIME_THREAD_PRIORITY;
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        
        // Set CPU affinity to dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(DEDICATED_CPU_CORE, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        listenerWorker();
    });
    
    LOG_INFO("Custom Incoming Adapter started: {}", adapter_name_);
    return true;
}

void CustomIncomingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    LOG_INFO("Stopping Custom Incoming Adapter: {}", adapter_name_);
    running_ = false;
    
    if (listener_thread_.joinable()) {
        listener_thread_.join();
    }
    
    LOG_INFO("Custom Incoming Adapter stopped: {}", adapter_name_);
}

bool CustomIncomingAdapter::isRunning() const {
    return running_.load();
}

std::string CustomIncomingAdapter::getName() const {
    return adapter_name_;
}

// ═══════════════════════════════════════════════════════════════════════════
// Listener Worker
// ═══════════════════════════════════════════════════════════════════════════

void CustomIncomingAdapter::listenerWorker() {
    LOG_DEBUG("Listener worker started");
    
    while (running_.load()) {
        try {
            // ═══════════════════════════════════════════════════════════
            // RECEIVE IMPLEMENTATION
            // Kullanılacak protokole göre burayı düzenleyin
            // ═══════════════════════════════════════════════════════════
            
            // ZeroMQ DISH örneği:
            // zmq::message_t received_msg;
            // auto result = dish_socket_->recv(received_msg, zmq::recv_flags::dontwait);
            // 
            // if (!result.has_value() || received_msg.size() == 0U) {
            //     std::this_thread::sleep_for(std::chrono::microseconds(10));
            //     continue;
            // }
            // 
            // const uint8_t* msg_data = static_cast<const uint8_t*>(received_msg.data());
            // std::vector<uint8_t> binary_data(msg_data, msg_data + received_msg.size());
            
            // Placeholder - gerçek uygulamada yukarıdaki kodu kullanın
            std::vector<uint8_t> binary_data;  // Received data
            
            // Simüle et (gerçek uygulamada kaldırın)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (binary_data.empty()) {
                continue;
            }
            
            // ═══════════════════════════════════════════════════════════
            // DESERIALIZE AND FORWARD TO DOMAIN
            // ═══════════════════════════════════════════════════════════
            
            auto data_opt = deserializeData(binary_data);
            
            if (data_opt.has_value() && domain_port_) {
                const auto& data = data_opt.value();
                
                if (data.isValid()) {
                    LOG_INFO("[CustomAdapter] Data received - ID: {}, Size: {} bytes",
                             data.getId(), binary_data.size());
                    
                    // ═══════════════════════════════════════════════════
                    // DOMAIN'E İLET - Port üzerinden
                    // ═══════════════════════════════════════════════════
                    domain_port_->submitCustomData(data);
                    
                } else {
                    LOG_WARN("Invalid data received, skipping");
                }
            } else {
                LOG_ERROR("Failed to deserialize data - Size: {} bytes", binary_data.size());
            }
            
        } catch (const std::exception& e) {
            LOG_ERROR("Listener worker error: {}", e.what());
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
    
    LOG_DEBUG("Listener worker stopped");
}

// ═══════════════════════════════════════════════════════════════════════════
// Deserialization
// ═══════════════════════════════════════════════════════════════════════════

std::optional<domain::ports::CustomData> CustomIncomingAdapter::deserializeData(
    const std::vector<uint8_t>& binary_data) {
    
    try {
        domain::ports::CustomData data;
        
        if (data.deserialize(binary_data)) {
            if (data.isValid()) {
                LOG_DEBUG("Data deserialized - ID: {}", data.getId());
                return data;
            } else {
                LOG_WARN("Data validation failed after deserialization");
                return std::nullopt;
            }
        } else {
            LOG_ERROR("Deserialization failed - Size: {} bytes", binary_data.size());
            return std::nullopt;
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Exception during deserialization: {}", e.what());
        return std::nullopt;
    }
}

} // namespace custom
} // namespace incoming
} // namespace adapters
```

---

## Entegrasyon (main.cpp)

### Tam Pipeline Entegrasyonu

```cpp
// ═══════════════════════════════════════════════════════════════════════════
// ADIM 1: Include dosyalarını ekle
// ═══════════════════════════════════════════════════════════════════════════
#include "adapters/incoming/custom/CustomIncomingAdapter.hpp"
#include "adapters/outgoing/custom/CustomOutgoingAdapter.hpp"
#include "domain/logic/CustomDataService.hpp"

// main() fonksiyonu içinde:

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 2: Outgoing Adapter oluştur (önce oluşturulmalı - service'e inject edilecek)
// ═══════════════════════════════════════════════════════════════════════════
LOG_INFO("Creating CustomData outgoing adapter...");

auto custom_outgoing = std::make_shared<
    adapters::outgoing::custom::CustomOutgoingAdapter>(
    "tcp://192.168.1.100:15004",  // endpoint
    "CustomDataOutput"            // channel name
);

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 3: Domain Service oluştur (outgoing port'u alır, incoming port'u implement eder)
// ═══════════════════════════════════════════════════════════════════════════
LOG_INFO("Creating CustomData domain service...");

auto custom_service = std::make_shared<domain::logic::CustomDataService>(
    custom_outgoing  // ICustomOutgoingPort olarak inject
);
// custom_service artık ICustomIncomingPort olarak da kullanılabilir

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 4: Incoming Adapter oluştur (domain service'i alır - port olarak)
// ═══════════════════════════════════════════════════════════════════════════
LOG_INFO("Creating CustomData incoming adapter...");

auto custom_incoming = std::make_shared<
    adapters::incoming::custom::CustomIncomingAdapter>(
    custom_service,               // ICustomIncomingPort olarak inject
    "udp://239.1.1.5:9001",       // endpoint
    "CustomDataInput"             // channel name
);

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 5: Pipeline oluştur ve AdapterManager'a kaydet
// ═══════════════════════════════════════════════════════════════════════════
LOG_INFO("Creating CustomData pipeline...");

adapters::MessagePipeline custom_pipeline(
    "CustomData",        // Pipeline adı
    custom_incoming,     // Incoming adapter
    custom_outgoing      // Outgoing adapter
);

adapter_manager.registerPipeline(std::move(custom_pipeline));

// ═══════════════════════════════════════════════════════════════════════════
// ADIM 6: Tüm pipeline'ları başlat
// ═══════════════════════════════════════════════════════════════════════════
LOG_INFO("Starting all pipelines...");
if (!adapter_manager.startAll()) {
    LOG_ERROR("Failed to start all pipelines");
    return 1;
}
```

### Veri Akış Diyagramı (main.cpp sonrası)

```
                        main.cpp'de oluşturulan nesneler
                        ================================

┌─────────────────────────────────────────────────────────────────────────────┐
│                                                                             │
│  ┌─────────────────────┐                                                   │
│  │   External System   │  (örn: b_hexagon)                                 │
│  │   (ZMQ RADIO)       │                                                   │
│  └──────────┬──────────┘                                                   │
│             │                                                               │
│             │ UDP Multicast                                                 │
│             ▼                                                               │
│  ┌─────────────────────┐                                                   │
│  │  custom_incoming    │  CustomIncomingAdapter                            │
│  │  (ZMQ DISH)         │  - Binds to udp://239.1.1.5:9001                  │
│  │                     │  - Receives & deserializes                        │
│  └──────────┬──────────┘                                                   │
│             │                                                               │
│             │ domain_port_->submitCustomData(data)                         │
│             ▼                                                               │
│  ┌─────────────────────┐                                                   │
│  │  custom_service     │  CustomDataService                                │
│  │                     │  - Implements ICustomIncomingPort                 │
│  │  submitCustomData() │  - Applies business logic                         │
│  │                     │  - Calls outgoing_port_->sendCustomData()         │
│  └──────────┬──────────┘                                                   │
│             │                                                               │
│             │ outgoing_port_->sendCustomData(processed)                    │
│             ▼                                                               │
│  ┌─────────────────────┐                                                   │
│  │  custom_outgoing    │  CustomOutgoingAdapter                            │
│  │  (ZMQ RADIO)        │  - Implements ICustomOutgoingPort                 │
│  │                     │  - Connects to tcp://192.168.1.100:15004          │
│  │                     │  - Serializes & sends                             │
│  └──────────┬──────────┘                                                   │
│             │                                                               │
│             │ TCP/UDP                                                       │
│             ▼                                                               │
│  ┌─────────────────────┐                                                   │
│  │   External System   │  (örn: d_hexagon veya listener)                   │
│  │   (ZMQ DISH)        │                                                   │
│  └─────────────────────┘                                                   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Makefile Güncellemesi

Yeni .cpp dosyalarını Makefile'a ekleyin:

```makefile
# Source files listesine ekle
SRCS += src/c_hexagon/adapters/incoming/custom/CustomIncomingAdapter.cpp
SRCS += src/c_hexagon/domain/logic/CustomDataService.cpp

# Veya ayrı değişken olarak
CUSTOM_INCOMING_ADAPTER_SRC = src/c_hexagon/adapters/incoming/custom/CustomIncomingAdapter.cpp
CUSTOM_SERVICE_SRC = src/c_hexagon/domain/logic/CustomDataService.cpp
```

---

## Örnek Kullanım Senaryoları

### Senaryo 1: ZeroMQ DISH Adapter (UDP Multicast Receiver)

```cpp
// UDP Multicast ile veri alımı
auto zmq_incoming = std::make_shared<ZeroMQIncomingAdapter>(
    domain_service,              // IIncomingPort'u implement eden service
    "udp://239.1.1.5:9595",      // Bind endpoint
    "TrackDataGroup"             // Subscribe group
);
```

### Senaryo 2: HTTP Server Adapter

```cpp
// REST API endpoint olarak dinleme
auto http_incoming = std::make_shared<HttpIncomingAdapter>(
    domain_service,
    "0.0.0.0:8080",
    "/api/v1/data"
);
// POST /api/v1/data istekleri domain_service->submitData() çağırır
```

### Senaryo 3: File Watcher Adapter

```cpp
// Dosya değişikliklerini izleme
auto file_incoming = std::make_shared<FileWatcherIncomingAdapter>(
    domain_service,
    "/var/data/input/",
    "*.bin"
);
// Yeni dosya geldiğinde domain_service->submitData() çağırır
```

### Senaryo 4: gRPC Server Adapter

```cpp
// gRPC server olarak dinleme
auto grpc_incoming = std::make_shared<GrpcIncomingAdapter>(
    domain_service,
    "0.0.0.0:50051",
    "DataService"
);
```

### Senaryo 5: Kafka Consumer Adapter

```cpp
// Kafka topic'inden mesaj okuma
auto kafka_incoming = std::make_shared<KafkaIncomingAdapter>(
    domain_service,
    "localhost:9092",
    "custom-data-topic"
);
```

---

## Checklist

Yeni bir Incoming Adapter eklerken aşağıdaki adımları takip edin:

| # | Adım | Dosya | Durum |
|---|------|-------|-------|
| 1 | Incoming Port Interface oluştur | `domain/ports/incoming/ICustomIncomingPort.hpp` | ☐ |
| 2 | Data Model oluştur (opsiyonel) | `domain/ports/CustomData.hpp` | ☐ |
| 3 | Domain Service oluştur | `domain/logic/CustomDataService.hpp/cpp` | ☐ |
| 4 | Incoming Adapter Header oluştur | `adapters/incoming/custom/CustomIncomingAdapter.hpp` | ☐ |
| 5 | Incoming Adapter Implementation oluştur | `adapters/incoming/custom/CustomIncomingAdapter.cpp` | ☐ |
| 6 | Makefile'a ekle | `Makefile` | ☐ |
| 7 | main.cpp'de outgoing adapter oluştur | `main.cpp` | ☐ |
| 8 | main.cpp'de domain service oluştur | `main.cpp` | ☐ |
| 9 | main.cpp'de incoming adapter oluştur | `main.cpp` | ☐ |
| 10 | Pipeline oluştur ve kaydet | `main.cpp` | ☐ |
| 11 | Derleme testi | `make clean && make` | ☐ |
| 12 | Çalışma testi | `./bin/c_hexagon` | ☐ |

---

## Incoming vs Outgoing Özet Tablosu

| Özellik | Incoming Adapter | Outgoing Adapter |
|---------|------------------|------------------|
| **Implement ettiği interface** | `IAdapter` | `IAdapter` + `IOutgoingPort` |
| **Constructor'da aldığı** | `IIncomingPort` (domain service) | Endpoint config |
| **Port'u kim implement eder?** | Domain Service | Adapter kendisi |
| **Adapter ne yapar?** | Port'u ÇAĞIRIR | Port'u IMPLEMENT EDER |
| **Socket tipi (ZMQ)** | DISH (bind/receive) | RADIO (connect/send) |
| **Ana işlev** | Receive → Deserialize → Forward | Queue → Serialize → Send |
| **Thread görevi** | Listener (receive loop) | Publisher (send loop) |

---

## Referanslar

- [Hexagonal Architecture (Ports & Adapters)](https://alistair.cockburn.us/hexagonal-architecture/)
- [Clean Architecture by Robert C. Martin](https://blog.cleancoder.com/uncle-bob/2012/08/13/the-clean-architecture.html)
- [ZeroMQ RADIO/DISH Pattern](https://zeromq.org/socket-api/#radio-dish-pattern)
- [MISRA C++ 2023 Guidelines](https://www.misra.org.uk/)
- [Dependency Inversion Principle](https://en.wikipedia.org/wiki/Dependency_inversion_principle)

---

**Hazırlayan:** c_hexagon Team  
**Versiyon:** 1.0  
**Tarih:** Aralık 2025
