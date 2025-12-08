# 🏗️ A_Hexagon - Yeni Incoming Adapter Ekleme Rehberi (v2)

Bu doküman, **mevcut `ITrackDataIncomingPort` ve `TrackData` model** kullanarak yeni bir Incoming Adapter ekleme sürecini açıklar.

> **Not:** Port interface ve Data Model zaten mevcut olduğundan, sadece **Adapter** ve **main.cpp entegrasyonu** gereklidir.

---

## 📋 Yapılacaklar Özeti

| Adım | Dosya | İşlem |
|------|-------|-------|
| **1** | `adapters/incoming/custom/CustomTrackDataIncomingAdapter.hpp` | ✨ YENİ OLUŞTUR |
| **2** | `adapters/incoming/custom/CustomTrackDataIncomingAdapter.cpp` | ✨ YENİ OLUŞTUR |
| **3** | `main.cpp` | 🔧 GÜNCELLE |
| **4** | `Makefile` | 🔧 GÜNCELLE |

---

## 📂 Mevcut Yapı (Değişmeyecek)

```
src/a_hexagon/
├── domain/
│   ├── model/
│   │   └── TrackData.hpp             ← MEVCUT ✅ (Değişmeyecek)
│   └── ports/
│       └── incoming/
│           └── ITrackDataIncomingPort.hpp  ← MEVCUT ✅ (Değişmeyecek)
```

### Mevcut Port Interface (Referans)

```cpp
// ITrackDataIncomingPort.hpp
class ITrackDataIncomingPort {
public:
    virtual ~ITrackDataIncomingPort() = default;
    virtual void processAndForwardTrackData(const model::TrackData& trackData) = 0;
};
```

### Mevcut Data Model (Referans)

```cpp
// TrackData.hpp
class TrackData final {
    int32_t trackId_;
    double xVelocityECEF_, yVelocityECEF_, zVelocityECEF_;
    double xPositionECEF_, yPositionECEF_, zPositionECEF_;
    int64_t originalUpdateTime_;
    
    // serialize(), deserialize(), isValid(), getSerializedSize()
};
```

---

## Adım 1: Adapter Header Oluştur

**Dosya:** `src/a_hexagon/adapters/incoming/custom/CustomTrackDataIncomingAdapter.hpp`

```cpp
/**
 * @file CustomTrackDataIncomingAdapter.hpp
 * @brief Technology-independent incoming adapter for TrackData
 * @details Uses existing ITrackDataIncomingPort and TrackData model
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/incoming/ITrackDataIncomingPort.hpp"
#include "domain/model/TrackData.hpp"

#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <vector>

namespace adapters {
namespace incoming {
namespace custom {

/**
 * @class CustomTrackDataIncomingAdapter
 * @brief Receives TrackData from custom external system
 * @details Implements IAdapter for lifecycle management.
 *          Override receiveData() for specific technology.
 */
class CustomTrackDataIncomingAdapter : public ::adapters::IAdapter {
public:
    // ═══════════════════════════════════════════════════════════════════
    // CONSTRUCTORS
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Constructor with domain port
     * @param incomingPort Domain service implementing ITrackDataIncomingPort
     * @param adapterName Unique identifier
     */
    explicit CustomTrackDataIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort,
        const std::string& adapterName = "CustomTrackDataIncomingAdapter");

    /**
     * @brief Constructor with endpoint configuration
     */
    CustomTrackDataIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort,
        const std::string& endpoint,
        const std::string& adapterName);

    /**
     * @brief Destructor
     */
    ~CustomTrackDataIncomingAdapter() override;

    // ═══════════════════════════════════════════════════════════════════
    // IAdapter INTERFACE
    // ═══════════════════════════════════════════════════════════════════
    
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const noexcept override;

    // Delete copy/move
    CustomTrackDataIncomingAdapter(const CustomTrackDataIncomingAdapter&) = delete;
    CustomTrackDataIncomingAdapter& operator=(const CustomTrackDataIncomingAdapter&) = delete;
    CustomTrackDataIncomingAdapter(CustomTrackDataIncomingAdapter&&) = delete;
    CustomTrackDataIncomingAdapter& operator=(CustomTrackDataIncomingAdapter&&) = delete;

protected:
    // ═══════════════════════════════════════════════════════════════════
    // OVERRIDE FOR SPECIFIC TECHNOLOGY
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Initialize connection - Override for your technology
     */
    virtual bool initializeConnection();

    /**
     * @brief Receive raw data - Override for your technology
     * @param buffer Output buffer for received data
     * @return true if data received
     */
    virtual bool receiveData(std::vector<uint8_t>& buffer);

    /**
     * @brief Close connection - Override for your technology
     */
    virtual void closeConnection();

private:
    void receiveLoop();

    // Domain port (MEVCUT interface kullanılıyor)
    std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort_;

    // Configuration
    std::string endpoint_;
    std::string adapterName_;

    // Thread management
    std::thread receiveThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopRequested_{false};

    // Statistics
    std::atomic<uint64_t> receivedCount_{0};

    // Constants
    static constexpr int32_t RECEIVE_TIMEOUT_MS{100};
};

} // namespace custom
} // namespace incoming
} // namespace adapters
```

---

## Adım 2: Adapter Implementation Oluştur

**Dosya:** `src/a_hexagon/adapters/incoming/custom/CustomTrackDataIncomingAdapter.cpp`

```cpp
/**
 * @file CustomTrackDataIncomingAdapter.cpp
 * @brief Implementation of CustomTrackDataIncomingAdapter
 */

#include "CustomTrackDataIncomingAdapter.hpp"
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

CustomTrackDataIncomingAdapter::CustomTrackDataIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort,
    const std::string& adapterName)
    : incomingPort_(std::move(incomingPort))
    , adapterName_(adapterName) {
    
    LOG_INFO("[{}] Adapter created", adapterName_);
}

CustomTrackDataIncomingAdapter::CustomTrackDataIncomingAdapter(
    std::shared_ptr<domain::ports::incoming::ITrackDataIncomingPort> incomingPort,
    const std::string& endpoint,
    const std::string& adapterName)
    : incomingPort_(std::move(incomingPort))
    , endpoint_(endpoint)
    , adapterName_(adapterName) {
    
    LOG_INFO("[{}] Adapter created - Endpoint: {}", adapterName_, endpoint_);
}

CustomTrackDataIncomingAdapter::~CustomTrackDataIncomingAdapter() {
    stop();
    LOG_INFO("[{}] Adapter destroyed", adapterName_);
}

// ═══════════════════════════════════════════════════════════════════════════
// IAdapter INTERFACE
// ═══════════════════════════════════════════════════════════════════════════

bool CustomTrackDataIncomingAdapter::start() {
    if (running_.load()) {
        LOG_WARN("[{}] Already running", adapterName_);
        return false;
    }

    if (!initializeConnection()) {
        LOG_ERROR("[{}] Failed to initialize connection", adapterName_);
        return false;
    }

    stopRequested_.store(false);
    running_.store(true);

    receiveThread_ = std::thread([this]() {
        receiveLoop();
    });

    LOG_INFO("[{}] Started successfully", adapterName_);
    return true;
}

void CustomTrackDataIncomingAdapter::stop() {
    if (!running_.load()) {
        return;
    }

    stopRequested_.store(true);
    running_.store(false);

    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }

    closeConnection();
    LOG_INFO("[{}] Stopped - Received: {}", adapterName_, receivedCount_.load());
}

bool CustomTrackDataIncomingAdapter::isRunning() const {
    return running_.load();
}

std::string CustomTrackDataIncomingAdapter::getName() const noexcept {
    return adapterName_;
}

// ═══════════════════════════════════════════════════════════════════════════
// VIRTUAL METHODS - Override for specific technology
// ═══════════════════════════════════════════════════════════════════════════

bool CustomTrackDataIncomingAdapter::initializeConnection() {
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🔧 TEKNOLOJİNİZE GÖRE OVERRIDE EDİN                              ║
    // ║  Örnek: Kafka consumer, TCP socket, gRPC channel, etc.            ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    LOG_DEBUG("[{}] initializeConnection() - Override for your technology", adapterName_);
    return true;
}

bool CustomTrackDataIncomingAdapter::receiveData(std::vector<uint8_t>& buffer) {
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🔧 TEKNOLOJİNİZE GÖRE OVERRIDE EDİN                              ║
    // ║  Örnek: kafka->poll(), socket->recv(), grpc->read(), etc.         ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    std::this_thread::sleep_for(std::chrono::milliseconds(RECEIVE_TIMEOUT_MS));
    return false;  // No data by default
}

void CustomTrackDataIncomingAdapter::closeConnection() {
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🔧 TEKNOLOJİNİZE GÖRE OVERRIDE EDİN                              ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    LOG_DEBUG("[{}] closeConnection()", adapterName_);
}

// ═══════════════════════════════════════════════════════════════════════════
// RECEIVE LOOP
// ═══════════════════════════════════════════════════════════════════════════

void CustomTrackDataIncomingAdapter::receiveLoop() {
    LOG_DEBUG("[{}] Receive loop started", adapterName_);

    std::vector<uint8_t> buffer;
    buffer.reserve(4096);

    while (!stopRequested_.load()) {
        try {
            buffer.clear();

            if (receiveData(buffer) && !buffer.empty()) {
                
                // ═══════════════════════════════════════════════════════
                // MEVCUT TrackData MODEL KULLANILIYOR
                // ═══════════════════════════════════════════════════════
                domain::model::TrackData trackData;
                
                if (trackData.deserialize(buffer)) {
                    if (trackData.isValid() && incomingPort_) {
                        ++receivedCount_;
                        
                        // ═══════════════════════════════════════════════
                        // 🔥 MEVCUT PORT INTERFACE ÇAĞRILIYOR
                        // ═══════════════════════════════════════════════
                        incomingPort_->processAndForwardTrackData(trackData);
                        
                        LOG_DEBUG("[{}] TrackData forwarded - ID: {}",
                                  adapterName_, trackData.getTrackId());
                    }
                } else {
                    LOG_WARN("[{}] Deserialize failed - Size: {}",
                             adapterName_, buffer.size());
                }
            }

        } catch (const std::exception& e) {
            LOG_ERROR("[{}] Error: {}", adapterName_, e.what());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    LOG_DEBUG("[{}] Receive loop ended", adapterName_);
}

} // namespace custom
} // namespace incoming
} // namespace adapters
```

---

## Adım 3: main.cpp Güncellemesi

**Dosya:** `src/a_hexagon/main.cpp`

### 3.1 Include Ekle

```cpp
// Mevcut includes
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"

// ═══════════════════════════════════════════════════════════════════════
// YENİ INCLUDE
// ═══════════════════════════════════════════════════════════════════════
#include "adapters/incoming/custom/CustomTrackDataIncomingAdapter.hpp"
```

### 3.2 Pipeline Ekle (startAll() öncesine)

```cpp
        // ═══════════════════════════════════════════════════════════════
        // MEVCUT ZeroMQ Pipeline
        // ═══════════════════════════════════════════════════════════════
        // ... existing TrackData ZeroMQ pipeline code ...

        // ═══════════════════════════════════════════════════════════════
        // YENİ CUSTOM PIPELINE
        // ═══════════════════════════════════════════════════════════════
        LOG_INFO("Creating Custom TrackData pipeline...");

        // Mevcut domain service'i kullan (TrackDataExtrapolator)
        // VEYA yeni bir instance oluştur
        auto custom_extrapolator = std::make_shared<domain::logic::TrackDataExtrapolator>(
            outgoing_adapter.get()  // Mevcut outgoing adapter
        );
        
        // Custom adapter oluştur
        auto custom_adapter = std::make_shared<
            adapters::incoming::custom::CustomTrackDataIncomingAdapter>(
            custom_extrapolator,              // ITrackDataIncomingPort
            "tcp://127.0.0.1:15020",         // Endpoint
            "CustomTrackDataAdapter"          // Adapter adı
        );

        // Pipeline oluştur ve kaydet
        adapters::MessagePipeline custom_pipeline(
            "CustomTrackData",
            custom_adapter,
            nullptr  // veya outgoing_adapter
        );

        adapter_manager.registerPipeline(std::move(custom_pipeline));
        
        // ═══════════════════════════════════════════════════════════════
        // TÜM PIPELINE'LARI BAŞLAT
        // ═══════════════════════════════════════════════════════════════
        if (!adapter_manager.startAll()) {
```

---

## Adım 4: Makefile Güncellemesi

**Dosya:** `Makefile`

```makefile
# Mevcut sources
SOURCES = \
    src/a_hexagon/main.cpp \
    src/a_hexagon/adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.cpp \
    # ... diğer mevcut dosyalar ...

# ═══════════════════════════════════════════════════════════════════════
# YENİ DOSYA EKLE
# ═══════════════════════════════════════════════════════════════════════
SOURCES += \
    src/a_hexagon/adapters/incoming/custom/CustomTrackDataIncomingAdapter.cpp
```

---

## 📊 Veri Akışı

```
┌─────────────────────────────────────────────────────────────────────┐
│  [Dış Sistem]                                                       │
│       │                                                             │
│       ▼ binary data                                                 │
│  CustomTrackDataIncomingAdapter::receiveData()  ← 🔧 Override et    │
│       │                                                             │
│       ▼ vector<uint8_t>                                             │
│  TrackData::deserialize()  ← MEVCUT model                           │
│       │                                                             │
│       ▼ TrackData object                                            │
│  incomingPort_->processAndForwardTrackData()  ← MEVCUT interface    │
│       │                                                             │
│       ▼                                                             │
│  TrackDataExtrapolator (veya başka service)  ← MEVCUT service       │
└─────────────────────────────────────────────────────────────────────┘
```

---

## ✅ Checklist

### Oluşturulacak Dosyalar

- [ ] `adapters/incoming/custom/CustomTrackDataIncomingAdapter.hpp`
- [ ] `adapters/incoming/custom/CustomTrackDataIncomingAdapter.cpp`

### Güncellenecek Dosyalar

- [ ] `main.cpp` - Include ekle
- [ ] `main.cpp` - Pipeline oluştur ve kaydet
- [ ] `Makefile` - Yeni .cpp dosyasını ekle

### Test

- [ ] Derleme başarılı
- [ ] Adapter start() edildi (log kontrolü)
- [ ] Veri alınıp domain'e aktarılıyor

---

## 🔧 Teknoloji Uyarlama Örneği

Farklı bir teknoloji için sadece 3 metodu override edin:

**Örnek: TCP Socket**

```cpp
class TCPTrackDataIncomingAdapter : public CustomTrackDataIncomingAdapter {
protected:
    bool initializeConnection() override {
        socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        // connect()...
        return socket_fd_ >= 0;
    }

    bool receiveData(std::vector<uint8_t>& buffer) override {
        buffer.resize(TrackData::getSerializedSize());
        ssize_t n = recv(socket_fd_, buffer.data(), buffer.size(), 0);
        return n > 0;
    }

    void closeConnection() override {
        close(socket_fd_);
    }

private:
    int socket_fd_{-1};
};
```

---

## 📋 Özet Karşılaştırma

| Bileşen | v1 (Tam Rehber) | v2 (Sadeleştirilmiş) |
|---------|-----------------|----------------------|
| Data Model | ✨ Yeni oluştur | ✅ Mevcut `TrackData` |
| Port Interface | ✨ Yeni oluştur | ✅ Mevcut `ITrackDataIncomingPort` |
| Domain Service | ✨ Yeni oluştur | ✅ Mevcut `TrackDataExtrapolator` |
| Adapter Header | ✨ Yeni oluştur | ✨ Yeni oluştur |
| Adapter Impl | ✨ Yeni oluştur | ✨ Yeni oluştur |
| main.cpp | 🔧 Güncelle | 🔧 Güncelle |
| Makefile | 🔧 Güncelle | 🔧 Güncelle |
| **Toplam Dosya** | 6-7 dosya | **2 yeni + 2 güncelleme** |
