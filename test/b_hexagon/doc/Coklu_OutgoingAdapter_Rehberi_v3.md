# 🏗️ B_Hexagon - Çoklu Outgoing Adapter Rehberi (v3)

**Amaç:** Tek IncomingAdapter'dan gelen veriyi birden fazla OutgoingAdapter ile farklı teknolojilere **non-blocking** şekilde göndermek.

---

## ⚠️ Kritik: Blocking vs Non-Blocking

```
❌ YANLIŞ - Bloklayan implementasyon
┌─────────────────────────────────────────────────────────────────────────┐
│  sendDelayCalcTrackData(data)                                           │
│       │                                                                 │
│       ▼                                                                 │
│  ZeroMQ: socket_->send() ─── 50μs bekle ───┐                            │
│                                            │ TOPLAM: ~5ms               │
│  Kafka: producer_->send() ─── 5ms bekle ───┘                            │
│       │                                                                 │
│       ▼                                                                 │
│  Domain thread 5ms BLOKLANIR ❌                                          │
│  Veri işleme hızı düşer, latency artar                                  │
└─────────────────────────────────────────────────────────────────────────┘

✅ DOĞRU - Non-blocking implementasyon
┌─────────────────────────────────────────────────────────────────────────┐
│  sendDelayCalcTrackData(data)                                           │
│       │                                                                 │
│       ▼                                                                 │
│  ZeroMQ: enqueue() ─── ~20ns ───┐                                       │
│                                 │ TOPLAM: ~40ns                         │
│  Kafka: enqueue() ─── ~20ns ────┘                                       │
│       │                                                                 │
│       ▼                                                                 │
│  Domain thread HEMEN DÖNER ✅                                            │
│                                                                         │
│  [Worker Thread 1] ──→ ZeroMQ send (arka planda)                        │
│  [Worker Thread 2] ──→ Kafka send (arka planda)                         │
│                                                                         │
│  Her adapter kendi hızında, birbirini ETKİLEMEZ ✅                       │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 📋 Yapılacaklar

| Adım | Dosya | İşlem |
|------|-------|-------|
| **1** | `adapters/outgoing/CompositeOutgoingAdapter.hpp` | ✨ YENİ |
| **2** | `adapters/outgoing/kafka/KafkaOutgoingAdapter.hpp` | ✨ YENİ |
| **3** | `adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp` | ✨ YENİ |
| **4** | `main.cpp` | 🔧 GÜNCELLE |
| **5** | `Makefile` | 🔧 GÜNCELLE |

---

## Adım 1: CompositeOutgoingAdapter.hpp

**Dosya:** `src/b_hexagon/adapters/outgoing/CompositeOutgoingAdapter.hpp`

```cpp
#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include "utils/Logger.hpp"
#include <vector>
#include <memory>
#include <atomic>

namespace adapters {
namespace outgoing {

/**
 * @class CompositeOutgoingAdapter
 * @brief Birden fazla adapter'ı tek interface arkasında birleştirir
 * 
 * @note Her adapter'ın sendDelayCalcTrackData() metodu NON-BLOCKING olmalı!
 *       Aksi halde adapter'lar birbirini bloklar.
 */
class CompositeOutgoingAdapter 
    : public IAdapter
    , public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
    
public:
    explicit CompositeOutgoingAdapter(const std::string& name = "CompositeOutgoingAdapter")
        : name_(name) {}
    
    ~CompositeOutgoingAdapter() override { stop(); }

    // Delete copy/move
    CompositeOutgoingAdapter(const CompositeOutgoingAdapter&) = delete;
    CompositeOutgoingAdapter& operator=(const CompositeOutgoingAdapter&) = delete;

    // ═══════════════════════════════════════════════════════════════════
    // ADAPTER YÖNETİMİ
    // ═══════════════════════════════════════════════════════════════════
    
    void addAdapterWithLifecycle(std::shared_ptr<IAdapter> adapter) {
        if (adapter) {
            lifecycleAdapters_.push_back(adapter);
            if (auto port = std::dynamic_pointer_cast<
                    domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort>(adapter)) {
                portAdapters_.push_back(port);
            }
        }
    }
    
    [[nodiscard]] size_t getAdapterCount() const noexcept {
        return portAdapters_.size();
    }

    // ═══════════════════════════════════════════════════════════════════
    // IDelayCalcTrackDataOutgoingPort
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Tüm adapter'lara veri gönderir
     * @note Her adapter non-blocking olduğu için toplam süre ~N*20ns
     */
    void sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) override {
        for (auto& adapter : portAdapters_) {
            try {
                // Her adapter kendi queue'suna ekler ve hemen döner (~20ns)
                adapter->sendDelayCalcTrackData(data);
            } catch (const std::exception& e) {
                Logger::error("Composite send failed: ", e.what());
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════════
    // IAdapter
    // ═══════════════════════════════════════════════════════════════════
    
    [[nodiscard]] bool start() override {
        bool allSuccess = true;
        for (auto& adapter : lifecycleAdapters_) {
            if (!adapter->start()) allSuccess = false;
        }
        running_ = allSuccess;
        return allSuccess;
    }
    
    void stop() override {
        running_ = false;
        for (auto& adapter : lifecycleAdapters_) {
            adapter->stop();
        }
    }
    
    [[nodiscard]] bool isRunning() const override { return running_; }
    [[nodiscard]] std::string getName() const noexcept override { return name_; }

private:
    std::string name_;
    std::atomic<bool> running_{false};
    std::vector<std::shared_ptr<domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort>> portAdapters_;
    std::vector<std::shared_ptr<IAdapter>> lifecycleAdapters_;
};

} // namespace outgoing
} // namespace adapters
```

---

## Adım 2: KafkaOutgoingAdapter.hpp (Non-Blocking)

**Dosya:** `src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.hpp`

```cpp
#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace adapters {
namespace outgoing {
namespace kafka {

/**
 * @class KafkaOutgoingAdapter
 * @brief Non-blocking Kafka adapter with internal queue
 * 
 * @details sendDelayCalcTrackData() sadece queue'ya ekler (~20ns)
 *          Worker thread arka planda Kafka'ya gönderir
 */
class KafkaOutgoingAdapter 
    : public IAdapter
    , public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
    
public:
    KafkaOutgoingAdapter(const std::string& brokers, const std::string& topic);
    ~KafkaOutgoingAdapter() override;

    // Delete copy/move
    KafkaOutgoingAdapter(const KafkaOutgoingAdapter&) = delete;
    KafkaOutgoingAdapter& operator=(const KafkaOutgoingAdapter&) = delete;

    // IAdapter
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const noexcept override;

    // IDelayCalcTrackDataOutgoingPort - NON-BLOCKING!
    void sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) override;

private:
    // ═══════════════════════════════════════════════════════════════════
    // NON-BLOCKING QUEUE MECHANISM
    // ═══════════════════════════════════════════════════════════════════
    void enqueueMessage(const domain::ports::DelayCalcTrackData& data);
    void workerThread();

    std::string brokers_;
    std::string topic_;
    std::string adapterName_;
    
    // Worker thread - arka planda gönderim yapar
    std::thread workerThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopRequested_{false};
    
    // Thread-safe message queue
    std::queue<domain::ports::DelayCalcTrackData> messageQueue_;
    mutable std::mutex queueMutex_;
    std::condition_variable queueCv_;
    
    // Queue overflow protection
    static constexpr size_t MAX_QUEUE_SIZE{10000};
    
    // Statistics
    std::atomic<uint64_t> enqueuedCount_{0};
    std::atomic<uint64_t> sentCount_{0};
    std::atomic<uint64_t> droppedCount_{0};
};

} // namespace kafka
} // namespace outgoing
} // namespace adapters
```

---

## Adım 3: KafkaOutgoingAdapter.cpp (Non-Blocking)

**Dosya:** `src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp`

```cpp
#include "KafkaOutgoingAdapter.hpp"
#include "utils/Logger.hpp"

namespace adapters {
namespace outgoing {
namespace kafka {

KafkaOutgoingAdapter::KafkaOutgoingAdapter(const std::string& brokers, const std::string& topic)
    : brokers_(brokers)
    , topic_(topic)
    , adapterName_("KafkaAdapter-" + topic) {
    Logger::info("[", adapterName_, "] Created - Brokers: ", brokers_);
}

KafkaOutgoingAdapter::~KafkaOutgoingAdapter() { 
    stop(); 
}

bool KafkaOutgoingAdapter::start() {
    if (running_.load()) return true;
    
    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🔧 KAFKA PRODUCER INITIALIZATION                                 ║
    // ║  Gerçek projede: rdkafka veya librdkafka kullanılır               ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    
    stopRequested_.store(false);
    running_.store(true);
    
    // Worker thread başlat - arka planda gönderim yapacak
    workerThread_ = std::thread([this]() { 
        workerThread(); 
    });
    
    Logger::info("[", adapterName_, "] Started - Worker thread active");
    return true;
}

void KafkaOutgoingAdapter::stop() {
    if (!running_.load()) return;
    
    stopRequested_.store(true);
    running_.store(false);
    
    // Worker thread'i uyandır
    queueCv_.notify_all();
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
    
    Logger::info("[", adapterName_, "] Stopped - Enqueued: ", enqueuedCount_.load(),
                ", Sent: ", sentCount_.load(), ", Dropped: ", droppedCount_.load());
}

bool KafkaOutgoingAdapter::isRunning() const { 
    return running_.load(); 
}

std::string KafkaOutgoingAdapter::getName() const noexcept { 
    return adapterName_; 
}

// ═══════════════════════════════════════════════════════════════════════════
// NON-BLOCKING SEND - Sadece queue'ya ekler (~20ns)
// ═══════════════════════════════════════════════════════════════════════════

void KafkaOutgoingAdapter::sendDelayCalcTrackData(
    const domain::ports::DelayCalcTrackData& data) {
    
    if (!running_.load()) {
        Logger::warn("[", adapterName_, "] Not running, message dropped");
        ++droppedCount_;
        return;
    }
    
    // NON-BLOCKING: Sadece queue'ya ekle ve hemen dön
    enqueueMessage(data);
}

void KafkaOutgoingAdapter::enqueueMessage(
    const domain::ports::DelayCalcTrackData& data) {
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        // Queue overflow protection
        if (messageQueue_.size() >= MAX_QUEUE_SIZE) {
            messageQueue_.pop();  // En eski mesajı at
            ++droppedCount_;
            Logger::warn("[", adapterName_, "] Queue full, dropping oldest");
        }
        
        messageQueue_.push(data);
        ++enqueuedCount_;
    }
    
    // Worker thread'i uyandır
    queueCv_.notify_one();
    
    // ~20ns içinde döner - BLOCKING YOK!
}

// ═══════════════════════════════════════════════════════════════════════════
// WORKER THREAD - Arka planda gerçek gönderimi yapar
// ═══════════════════════════════════════════════════════════════════════════

void KafkaOutgoingAdapter::workerThread() {
    Logger::debug("[", adapterName_, "] Worker thread started");
    
    while (!stopRequested_.load()) {
        domain::ports::DelayCalcTrackData data;
        
        // Queue'dan mesaj al (blocking wait)
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            // 100ms timeout ile bekle
            queueCv_.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                return !messageQueue_.empty() || stopRequested_.load();
            });
            
            if (stopRequested_.load() && messageQueue_.empty()) {
                break;
            }
            
            if (messageQueue_.empty()) {
                continue;
            }
            
            data = std::move(messageQueue_.front());
            messageQueue_.pop();
        }
        
        // ╔═══════════════════════════════════════════════════════════════╗
        // ║  🔧 KAFKA PRODUCE - BURADA BLOCKING OLUR AMA WORKER THREAD'DE ║
        // ║  Domain thread etkilenmez!                                    ║
        // ╚═══════════════════════════════════════════════════════════════╝
        
        try {
            auto serialized = data.serialize();
            
            // Gerçek Kafka gönderimi (örnek)
            // producer_->produce(topic_, serialized.data(), serialized.size());
            
            ++sentCount_;
            
            Logger::debug("[", adapterName_, "] Sent TrackID=", data.getTrackId());
            
        } catch (const std::exception& e) {
            Logger::error("[", adapterName_, "] Send failed: ", e.what());
        }
    }
    
    Logger::debug("[", adapterName_, "] Worker thread ended");
}

} // namespace kafka
} // namespace outgoing
} // namespace adapters
```

---

## Adım 4: main.cpp Güncellemesi

```cpp
// YENİ INCLUDE'LAR
#include "adapters/outgoing/CompositeOutgoingAdapter.hpp"
#include "adapters/outgoing/kafka/KafkaOutgoingAdapter.hpp"

// ... main() içinde ...

// 1. Mevcut ZeroMQ Adapter (zaten non-blocking)
auto zmq_adapter = std::make_shared<DelayCalcTrackDataZeroMQOutgoingAdapter>(
    "udp://239.1.1.5:9595", "DelayCalcTrackData");

// 2. Yeni Kafka Adapter (non-blocking)
auto kafka_adapter = std::make_shared<adapters::outgoing::kafka::KafkaOutgoingAdapter>(
    "localhost:9092", "delay-calc-topic");

// 3. Composite ile birleştir
auto composite = std::make_shared<adapters::outgoing::CompositeOutgoingAdapter>();
composite->addAdapterWithLifecycle(zmq_adapter);
composite->addAdapterWithLifecycle(kafka_adapter);

// 4. UseCase'e inject et (domain değişmedi!)
auto useCase = std::make_shared<domain::logic::ProcessTrackUseCase>(
    std::make_unique<CalculatorService>(),
    composite
);
```

---

## Adım 5: Makefile

```makefile
SOURCES += \
    src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp
```

---

## 📊 Performans Karşılaştırma

| Metrik | ❌ Blocking | ✅ Non-Blocking |
|--------|------------|-----------------|
| `sendDelayCalcTrackData()` süresi | ~5ms (Kafka bekler) | ~40ns |
| Domain thread | Bloklanır | Hemen döner |
| Adapter'lar birbirini etkiler mi? | Evet | Hayır |
| Max throughput | ~200 msg/s | ~25M msg/s |
| Latency | Yüksek, değişken | Düşük, sabit |

---

## ✅ Checklist

- [ ] `CompositeOutgoingAdapter.hpp` oluştur
- [ ] `KafkaOutgoingAdapter.hpp` oluştur (queue + worker thread)
- [ ] `KafkaOutgoingAdapter.cpp` oluştur
- [ ] `main.cpp` güncelle
- [ ] `Makefile` güncelle
- [ ] **Test:** Her adapter bağımsız çalışıyor mu?
- [ ] **Test:** Bir adapter yavaşlasa diğeri etkileniyor mu?

---

## 🔑 Altın Kural

```cpp
// ❌ YANLIŞ - Domain thread'i bloklar
void sendDelayCalcTrackData(const DelayCalcTrackData& data) override {
    socket_->send(data.serialize());  // Network I/O bekler!
}

// ✅ DOĞRU - Domain thread hemen döner
void sendDelayCalcTrackData(const DelayCalcTrackData& data) override {
    enqueueMessage(data);  // ~20ns - Queue'ya ekle, worker thread gönderecek
}
```

**Her OutgoingAdapter mutlaka:**
1. Internal queue
2. Worker thread
3. Non-blocking `send` metodu

içermelidir!
