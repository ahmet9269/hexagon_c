# 🏗️ B_Hexagon - Çoklu Outgoing Adapter Ekleme Rehberi

Bu doküman, b_hexagon projesinde **tek bir IncomingAdapter'dan gelen veriyi birden fazla OutgoingAdapter ile farklı teknolojilere gönderme** sürecini açıklar.

---

## 📋 İçindekiler

1. [Mevcut Durum Analizi](#mevcut-durum-analizi)
2. [İstenen Yapı](#i̇stenen-yapı)
3. [Composite Adapter Pattern](#composite-adapter-pattern)
4. [Yapılacaklar Listesi](#yapılacaklar-listesi)
5. [Adım 1: CompositeOutgoingAdapter Oluşturma](#adım-1-compositeoutgoingadapter-oluşturma)
6. [Adım 2: İkinci Teknoloji Adapter'ı](#adım-2-i̇kinci-teknoloji-adapteri)
7. [Adım 3: main.cpp Entegrasyonu](#adım-3-maincpp-entegrasyonu)
8. [Adım 4: Makefile Güncellemesi](#adım-4-makefile-güncellemesi)
9. [Veri Akışı](#veri-akışı)
10. [Checklist](#checklist)

---

## Mevcut Durum Analizi

### 📂 Mevcut Bileşenler (Değişmeyecek)

| Bileşen | Dosya | Açıklama |
|---------|-------|----------|
| **Incoming Port** | `IExtrapTrackDataIncomingPort.hpp` | Gelen veri interface'i |
| **Incoming Model** | `ExtrapTrackData.hpp` | a_hexagon'dan gelen veri |
| **Outgoing Port** | `IDelayCalcTrackDataOutgoingPort.hpp` | Çıkan veri interface'i |
| **Outgoing Model** | `DelayCalcTrackData.hpp` | c_hexagon'a giden veri |
| **Domain Service** | `ProcessTrackUseCase.hpp` | İş mantığı |
| **Mevcut Adapter** | `DelayCalcTrackDataZeroMQOutgoingAdapter.hpp` | ZeroMQ ile gönderim |

### ❌ Mevcut Yapının Sınırlaması

```cpp
// ProcessTrackUseCase.hpp - MEVCUT
class ProcessTrackUseCase {
private:
    // 👇 SADECE TEK BİR OUTGOING PORT
    std::shared_ptr<IDelayCalcTrackDataOutgoingPort> dataSender_;
    
    void sendDelayCalcTrackData(const DelayCalcTrackData& data) {
        dataSender_->sendDelayCalcTrackData(data);  // 👈 TEK ÇAĞRI
    }
};
```

**Sorun:** `ProcessTrackUseCase` sadece tek bir `IDelayCalcTrackDataOutgoingPort` kabul ediyor.

---

## İstenen Yapı

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      a_hexagon                                          │
│                          │                                              │
│                          ▼ ExtrapTrackData                              │
│   ┌──────────────────────────────────────────────────────────────────┐ │
│   │  ExtrapTrackDataZeroMQIncomingAdapter                            │ │
│   │  (IExtrapTrackDataIncomingPort'a veri aktarır)                   │ │
│   └──────────────────────────────────────────────────────────────────┘ │
│                          │                                              │
│                          ▼                                              │
│   ┌──────────────────────────────────────────────────────────────────┐ │
│   │  ProcessTrackUseCase : IExtrapTrackDataIncomingPort              │ │
│   │  - onDataReceived(ExtrapTrackData)                               │ │
│   │  - calculator_->calculateDelay()                                 │ │
│   │  - dataSender_->sendDelayCalcTrackData(DelayCalcTrackData)       │ │
│   └──────────────────────────────────────────────────────────────────┘ │
│                          │                                              │
│                          ▼                                              │
│   ┌──────────────────────────────────────────────────────────────────┐ │
│   │  CompositeOutgoingAdapter : IDelayCalcTrackDataOutgoingPort      │ │
│   │                          │                                        │ │
│   │     sendDelayCalcTrackData(data) {                               │ │
│   │         for (auto& adapter : adapters_) {                        │ │
│   │             adapter->sendDelayCalcTrackData(data);               │ │
│   │         }                                                        │ │
│   │     }                                                            │ │
│   └──────────────────────────────────────────────────────────────────┘ │
│                          │                                              │
│          ┌───────────────┴───────────────┐                              │
│          ▼                               ▼                              │
│   ┌──────────────────┐          ┌──────────────────┐                   │
│   │ ZeroMQ Adapter   │          │ Kafka Adapter    │                   │
│   │ (Mevcut)         │          │ (Yeni)           │                   │
│   │                  │          │                  │                   │
│   │ → c_hexagon      │          │ → Kafka Broker   │                   │
│   └──────────────────┘          └──────────────────┘                   │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Composite Adapter Pattern

### Neden Composite Pattern?

| Avantaj | Açıklama |
|---------|----------|
| **Domain değişmez** | `ProcessTrackUseCase` hiç değişmiyor |
| **Interface değişmez** | `IDelayCalcTrackDataOutgoingPort` aynı kalıyor |
| **Kolay genişleme** | Yeni adapter eklemek sadece `addAdapter()` çağrısı |
| **SOLID uyumlu** | Open/Closed prensibi korunuyor |
| **Test edilebilir** | Her adapter bağımsız test edilebilir |

### Composite Pattern Yapısı

```
┌────────────────────────────────────────────────────────────────┐
│  IDelayCalcTrackDataOutgoingPort (Interface)                   │
│  + sendDelayCalcTrackData(data): void                          │
└────────────────────────────────────────────────────────────────┘
                              ▲
                              │ implements
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
┌───────┴───────┐    ┌───────┴───────┐    ┌───────┴───────┐
│ ZeroMQ        │    │ Kafka         │    │ Composite     │
│ Adapter       │    │ Adapter       │    │ Adapter       │
│               │    │               │    │               │
│ (Mevcut)      │    │ (Yeni)        │    │ (Yeni)        │
└───────────────┘    └───────────────┘    └───────┬───────┘
                                                  │
                                                  │ contains
                                                  ▼
                                    ┌─────────────────────────┐
                                    │ vector<IOutgoingPort*>  │
                                    │ - ZeroMQ Adapter        │
                                    │ - Kafka Adapter         │
                                    │ - ... (N adapter)       │
                                    └─────────────────────────┘
```

---

## Yapılacaklar Listesi

| Adım | Dosya | İşlem | Açıklama |
|------|-------|-------|----------|
| **1** | `adapters/outgoing/CompositeOutgoingAdapter.hpp` | ✨ YENİ | Composite pattern |
| **2** | `adapters/outgoing/kafka/KafkaOutgoingAdapter.hpp` | ✨ YENİ | 2. teknoloji header |
| **3** | `adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp` | ✨ YENİ | 2. teknoloji impl |
| **4** | `main.cpp` | 🔧 GÜNCELLE | Composite oluştur |
| **5** | `Makefile` | 🔧 GÜNCELLE | Yeni dosyaları ekle |

---

## Adım 1: CompositeOutgoingAdapter Oluşturma

**Dosya:** `src/b_hexagon/adapters/outgoing/CompositeOutgoingAdapter.hpp`

```cpp
/**
 * @file CompositeOutgoingAdapter.hpp
 * @brief Composite pattern for multiple outgoing adapters
 * @details Allows sending data to multiple destinations simultaneously
 *          without modifying domain services.
 * 
 * Pattern: Composite (Gang of Four)
 * Purpose: Treat multiple adapters as a single adapter
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include "utils/Logger.hpp"

#include <vector>
#include <memory>
#include <string>

namespace adapters {
namespace outgoing {

/**
 * @class CompositeOutgoingAdapter
 * @brief Combines multiple outgoing adapters into one
 * @details Implements both IAdapter (lifecycle) and IDelayCalcTrackDataOutgoingPort (data)
 * 
 * Usage:
 * @code
 * auto composite = std::make_shared<CompositeOutgoingAdapter>();
 * composite->addAdapter(zmqAdapter);
 * composite->addAdapter(kafkaAdapter);
 * 
 * // Domain service sadece composite'i görür
 * auto useCase = std::make_shared<ProcessTrackUseCase>(calculator, composite);
 * @endcode
 */
class CompositeOutgoingAdapter 
    : public IAdapter
    , public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
    
public:
    // ═══════════════════════════════════════════════════════════════════
    // CONSTRUCTOR / DESTRUCTOR
    // ═══════════════════════════════════════════════════════════════════
    
    explicit CompositeOutgoingAdapter(const std::string& name = "CompositeOutgoingAdapter")
        : name_(name) {
        Logger::info("CompositeOutgoingAdapter created");
    }
    
    ~CompositeOutgoingAdapter() override {
        stop();
    }

    // Delete copy/move for thread safety
    CompositeOutgoingAdapter(const CompositeOutgoingAdapter&) = delete;
    CompositeOutgoingAdapter& operator=(const CompositeOutgoingAdapter&) = delete;
    CompositeOutgoingAdapter(CompositeOutgoingAdapter&&) = delete;
    CompositeOutgoingAdapter& operator=(CompositeOutgoingAdapter&&) = delete;

    // ═══════════════════════════════════════════════════════════════════
    // ADAPTER YÖNETİMİ
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Add an adapter that only implements the port interface
     * @param adapter Outgoing port implementation
     */
    void addAdapter(std::shared_ptr<domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort> adapter) {
        if (adapter) {
            portAdapters_.push_back(std::move(adapter));
            Logger::info("Added port adapter to composite. Total: ", portAdapters_.size());
        }
    }
    
    /**
     * @brief Add an adapter that implements both IAdapter and port interface
     * @param adapter Full adapter with lifecycle management
     * @details This adapter's start/stop will be managed by composite
     */
    void addAdapterWithLifecycle(std::shared_ptr<IAdapter> adapter) {
        if (adapter) {
            lifecycleAdapters_.push_back(adapter);
            
            // Also add to port adapters if it implements the port interface
            if (auto portAdapter = std::dynamic_pointer_cast<
                    domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort>(adapter)) {
                portAdapters_.push_back(portAdapter);
            }
            
            Logger::info("Added lifecycle adapter to composite. Total lifecycle: ", 
                        lifecycleAdapters_.size(), ", Total ports: ", portAdapters_.size());
        }
    }
    
    /**
     * @brief Get number of registered adapters
     */
    [[nodiscard]] size_t getAdapterCount() const noexcept {
        return portAdapters_.size();
    }

    // ═══════════════════════════════════════════════════════════════════
    // IDelayCalcTrackDataOutgoingPort IMPLEMENTATION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Send data to ALL registered adapters
     * @param data The data to send
     * @details Each adapter is called sequentially. Errors are logged but
     *          don't prevent other adapters from receiving the data.
     */
    void sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) override {
        size_t successCount = 0;
        size_t errorCount = 0;
        
        for (auto& adapter : portAdapters_) {
            try {
                adapter->sendDelayCalcTrackData(data);
                ++successCount;
            } catch (const std::exception& e) {
                ++errorCount;
                Logger::error("Composite adapter send failed: ", e.what());
                // Continue to next adapter - don't let one failure stop others
            }
        }
        
        if (errorCount > 0) {
            Logger::warn("CompositeOutgoingAdapter: ", successCount, " succeeded, ", 
                        errorCount, " failed for TrackID=", data.getTrackId());
        }
    }

    // ═══════════════════════════════════════════════════════════════════
    // IAdapter IMPLEMENTATION (Lifecycle Management)
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Start all lifecycle-managed adapters
     * @return true if all started successfully
     */
    [[nodiscard]] bool start() override {
        if (running_) {
            return true;
        }
        
        bool allSuccess = true;
        size_t startedCount = 0;
        
        for (auto& adapter : lifecycleAdapters_) {
            if (adapter->start()) {
                ++startedCount;
                Logger::debug("Started adapter: ", adapter->getName());
            } else {
                allSuccess = false;
                Logger::error("Failed to start adapter: ", adapter->getName());
            }
        }
        
        running_ = allSuccess;
        Logger::info("CompositeOutgoingAdapter started. ", startedCount, "/", 
                    lifecycleAdapters_.size(), " adapters running");
        
        return allSuccess;
    }
    
    /**
     * @brief Stop all lifecycle-managed adapters
     */
    void stop() override {
        if (!running_) {
            return;
        }
        
        running_ = false;
        
        for (auto& adapter : lifecycleAdapters_) {
            try {
                adapter->stop();
                Logger::debug("Stopped adapter: ", adapter->getName());
            } catch (const std::exception& e) {
                Logger::error("Error stopping adapter ", adapter->getName(), ": ", e.what());
            }
        }
        
        Logger::info("CompositeOutgoingAdapter stopped");
    }
    
    /**
     * @brief Check if all adapters are running
     */
    [[nodiscard]] bool isRunning() const override {
        if (!running_ || lifecycleAdapters_.empty()) {
            return running_;
        }
        
        for (const auto& adapter : lifecycleAdapters_) {
            if (!adapter->isRunning()) {
                return false;
            }
        }
        return true;
    }
    
    /**
     * @brief Get composite adapter name
     */
    [[nodiscard]] std::string getName() const noexcept override {
        return name_;
    }

private:
    std::string name_;
    std::atomic<bool> running_{false};
    
    /// Adapters that only implement the port interface (no lifecycle)
    std::vector<std::shared_ptr<domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort>> portAdapters_;
    
    /// Adapters that also implement IAdapter (have lifecycle)
    std::vector<std::shared_ptr<IAdapter>> lifecycleAdapters_;
};

} // namespace outgoing
} // namespace adapters
```

---

## Adım 2: İkinci Teknoloji Adapter'ı

Örnek olarak Kafka adapter'ı gösterilmiştir. Herhangi bir teknoloji için aynı yapı kullanılabilir.

### 2.1 Header Dosyası

**Dosya:** `src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.hpp`

```cpp
/**
 * @file KafkaOutgoingAdapter.hpp
 * @brief Kafka adapter for outbound DelayCalcTrackData transmission
 * @details Example second technology adapter for composite pattern
 */

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
 * @brief Sends DelayCalcTrackData to Kafka topic
 * @details Implements both IAdapter and IDelayCalcTrackDataOutgoingPort
 */
class KafkaOutgoingAdapter 
    : public IAdapter
    , public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
    
public:
    /**
     * @brief Constructor with Kafka configuration
     * @param brokers Kafka broker list (e.g., "localhost:9092")
     * @param topic Kafka topic name
     */
    KafkaOutgoingAdapter(const std::string& brokers, const std::string& topic);
    
    ~KafkaOutgoingAdapter() override;

    // Delete copy/move
    KafkaOutgoingAdapter(const KafkaOutgoingAdapter&) = delete;
    KafkaOutgoingAdapter& operator=(const KafkaOutgoingAdapter&) = delete;
    KafkaOutgoingAdapter(KafkaOutgoingAdapter&&) = delete;
    KafkaOutgoingAdapter& operator=(KafkaOutgoingAdapter&&) = delete;

    // IAdapter interface
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const noexcept override;

    // IDelayCalcTrackDataOutgoingPort interface
    void sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) override;

private:
    void workerThread();
    void enqueueMessage(const domain::ports::DelayCalcTrackData& data);

    std::string brokers_;
    std::string topic_;
    std::string adapterName_;
    
    std::thread workerThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopRequested_{false};
    
    std::queue<domain::ports::DelayCalcTrackData> messageQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCv_;
    
    static constexpr size_t MAX_QUEUE_SIZE{1000};
};

} // namespace kafka
} // namespace outgoing
} // namespace adapters
```

### 2.2 Implementation Dosyası

**Dosya:** `src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp`

```cpp
/**
 * @file KafkaOutgoingAdapter.cpp
 * @brief Implementation of Kafka outgoing adapter
 */

#include "KafkaOutgoingAdapter.hpp"
#include "utils/Logger.hpp"

namespace adapters {
namespace outgoing {
namespace kafka {

KafkaOutgoingAdapter::KafkaOutgoingAdapter(
    const std::string& brokers, 
    const std::string& topic)
    : brokers_(brokers)
    , topic_(topic)
    , adapterName_("KafkaOutgoingAdapter-" + topic) {
    
    Logger::info("KafkaOutgoingAdapter created - Brokers: ", brokers_, ", Topic: ", topic_);
}

KafkaOutgoingAdapter::~KafkaOutgoingAdapter() {
    stop();
}

bool KafkaOutgoingAdapter::start() {
    if (running_.load()) {
        return true;
    }

    // ╔═══════════════════════════════════════════════════════════════════╗
    // ║  🔧 KAFKA PRODUCER INITIALIZATION                                 ║
    // ║  Gerçek projede: rdkafka veya librdkafka kullanılır               ║
    // ╚═══════════════════════════════════════════════════════════════════╝
    
    // Örnek: Kafka producer oluştur
    // producer_ = std::make_unique<RdKafka::Producer>(...);
    
    stopRequested_.store(false);
    running_.store(true);
    
    workerThread_ = std::thread([this]() {
        workerThread();
    });
    
    Logger::info("[", adapterName_, "] Started successfully");
    return true;
}

void KafkaOutgoingAdapter::stop() {
    if (!running_.load()) {
        return;
    }
    
    stopRequested_.store(true);
    running_.store(false);
    
    // Wake up worker thread
    queueCv_.notify_all();
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
    
    Logger::info("[", adapterName_, "] Stopped");
}

bool KafkaOutgoingAdapter::isRunning() const {
    return running_.load();
}

std::string KafkaOutgoingAdapter::getName() const noexcept {
    return adapterName_;
}

void KafkaOutgoingAdapter::sendDelayCalcTrackData(
    const domain::ports::DelayCalcTrackData& data) {
    
    if (!running_.load()) {
        Logger::warn("[", adapterName_, "] Not running, message dropped");
        return;
    }
    
    enqueueMessage(data);
}

void KafkaOutgoingAdapter::enqueueMessage(
    const domain::ports::DelayCalcTrackData& data) {
    
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    if (messageQueue_.size() >= MAX_QUEUE_SIZE) {
        Logger::warn("[", adapterName_, "] Queue full, dropping oldest message");
        messageQueue_.pop();
    }
    
    messageQueue_.push(data);
    queueCv_.notify_one();
}

void KafkaOutgoingAdapter::workerThread() {
    Logger::debug("[", adapterName_, "] Worker thread started");
    
    while (!stopRequested_.load()) {
        domain::ports::DelayCalcTrackData data;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
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
        
        try {
            // ╔═══════════════════════════════════════════════════════════╗
            // ║  🔧 KAFKA PRODUCE                                         ║
            // ║  Gerçek projede: producer_->produce(topic_, data)         ║
            // ╚═══════════════════════════════════════════════════════════╝
            
            auto serialized = data.serialize();
            
            // Örnek: Kafka'ya gönder
            // producer_->produce(topic_, serialized.data(), serialized.size());
            
            Logger::debug("[", adapterName_, "] Sent TrackID=", data.getTrackId(), 
                         " to topic: ", topic_);
            
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

## Adım 3: main.cpp Entegrasyonu

**Dosya:** `src/b_hexagon/main.cpp`

### 3.1 Include'lar Ekle

```cpp
// Mevcut includes
#include "adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.hpp"

// ═══════════════════════════════════════════════════════════════════════
// YENİ INCLUDE'LAR
// ═══════════════════════════════════════════════════════════════════════
#include "adapters/outgoing/CompositeOutgoingAdapter.hpp"
#include "adapters/outgoing/kafka/KafkaOutgoingAdapter.hpp"
```

### 3.2 Composite Adapter Oluştur

```cpp
int main(int argc, char* argv[]) {
    // ... existing initialization code ...

    try {
        // ═══════════════════════════════════════════════════════════════
        // MEVCUT ZeroMQ Adapter
        // ═══════════════════════════════════════════════════════════════
        auto zmq_adapter = std::make_shared<DelayCalcTrackDataZeroMQOutgoingAdapter>(
            "udp://239.1.1.5:9595",
            "DelayCalcTrackData"
        );
        
        // ═══════════════════════════════════════════════════════════════
        // YENİ Kafka Adapter
        // ═══════════════════════════════════════════════════════════════
        auto kafka_adapter = std::make_shared<adapters::outgoing::kafka::KafkaOutgoingAdapter>(
            "localhost:9092",           // Kafka brokers
            "delay-calc-track-data"     // Topic name
        );
        
        // ═══════════════════════════════════════════════════════════════
        // COMPOSITE ADAPTER - İKİ ADAPTER'I BİRLEŞTİR
        // ═══════════════════════════════════════════════════════════════
        auto composite_adapter = std::make_shared<adapters::outgoing::CompositeOutgoingAdapter>(
            "DelayCalcTrackData-CompositeAdapter"
        );
        
        // Adapter'ları composite'a ekle
        composite_adapter->addAdapterWithLifecycle(zmq_adapter);
        composite_adapter->addAdapterWithLifecycle(kafka_adapter);
        
        Logger::info("Composite adapter configured with ", 
                    composite_adapter->getAdapterCount(), " adapters");
        
        // ═══════════════════════════════════════════════════════════════
        // DOMAIN SERVICE - DEĞİŞMEDİ!
        // ═══════════════════════════════════════════════════════════════
        auto calculator = std::make_unique<domain::logic::CalculatorService>();
        
        // UseCase'e composite adapter'ı inject et (TEK adapter olarak)
        auto useCase = std::make_shared<domain::logic::ProcessTrackUseCase>(
            std::move(calculator),
            composite_adapter  // 👈 Domain service değişmedi, sadece composite geçtik
        );
        
        // ═══════════════════════════════════════════════════════════════
        // INCOMING ADAPTER
        // ═══════════════════════════════════════════════════════════════
        auto incomingAdapter = std::make_shared<ExtrapTrackDataZeroMQIncomingAdapter>(
            useCase
        );
        
        // ═══════════════════════════════════════════════════════════════
        // PIPELINE OLUŞTUR
        // ═══════════════════════════════════════════════════════════════
        adapters::MessagePipeline track_pipeline(
            "TrackDataProcessing",
            incomingAdapter,
            composite_adapter  // Composite'ı outgoing olarak kullan
        );
        
        adapter_manager.registerPipeline(std::move(track_pipeline));
        
        // Start all
        if (!adapter_manager.startAll()) {
            Logger::error("Failed to start pipelines");
            return 1;
        }
        
        // ... rest of main loop ...
        
    } catch (const std::exception& e) {
        Logger::critical("Application error: ", e.what());
        return 1;
    }
    
    return 0;
}
```

---

## Adım 4: Makefile Güncellemesi

**Dosya:** `Makefile`

```makefile
# Mevcut sources
SOURCES = \
    src/b_hexagon/main.cpp \
    src/b_hexagon/domain/logic/ProcessTrackUseCase.cpp \
    src/b_hexagon/domain/logic/CalculatorService.cpp \
    src/b_hexagon/adapters/incoming/zeromq/ExtrapTrackDataZeroMQIncomingAdapter.cpp \
    src/b_hexagon/adapters/outgoing/zeromq/DelayCalcTrackDataZeroMQOutgoingAdapter.cpp \
    # ... diğer mevcut dosyalar ...

# ═══════════════════════════════════════════════════════════════════════
# YENİ DOSYALAR
# ═══════════════════════════════════════════════════════════════════════
SOURCES += \
    src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp
    # CompositeOutgoingAdapter header-only, .cpp yok

# Kafka kütüphanesi gerekiyorsa
# LIBS += -lrdkafka++
```

---

## Veri Akışı

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        ÇOKLU OUTGOING ADAPTER AKIŞI                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   [a_hexagon] ─── ExtrapTrackData ──→ [ExtrapTrackDataZeroMQInAdapter]  │
│                                              │                          │
│                                              ▼                          │
│                                 ┌────────────────────────┐              │
│                                 │ ProcessTrackUseCase    │              │
│                                 │ (IExtrapTrackDataIn)   │              │
│                                 │                        │              │
│                                 │ onDataReceived() {     │              │
│                                 │   calculated = calc(); │              │
│                                 │   dataSender_->send(); │ ◄── TEK ÇAĞRI│
│                                 │ }                      │              │
│                                 └────────────┬───────────┘              │
│                                              │                          │
│                                              ▼                          │
│                                 ┌────────────────────────┐              │
│                                 │ CompositeOutgoingAdapter│              │
│                                 │ (IDelayCalcTrackDataOut)│              │
│                                 │                        │              │
│                                 │ send(data) {           │              │
│                                 │   for (adapter : all)  │              │
│                                 │     adapter->send();   │              │
│                                 │ }                      │              │
│                                 └────────────┬───────────┘              │
│                                              │                          │
│                         ┌────────────────────┼────────────────────┐     │
│                         │                    │                    │     │
│                         ▼                    ▼                    ▼     │
│               ┌──────────────┐     ┌──────────────┐     ┌──────────────┐│
│               │ ZeroMQ       │     │ Kafka        │     │ File         ││
│               │ Adapter      │     │ Adapter      │     │ Adapter      ││
│               │              │     │              │     │ (gelecekte)  ││
│               │ → c_hexagon  │     │ → Kafka      │     │ → log.json   ││
│               └──────────────┘     └──────────────┘     └──────────────┘│
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Checklist

### Oluşturulacak Dosyalar

- [ ] `adapters/outgoing/CompositeOutgoingAdapter.hpp`
- [ ] `adapters/outgoing/kafka/KafkaOutgoingAdapter.hpp`
- [ ] `adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp`

### Güncellenecek Dosyalar

- [ ] `main.cpp` - Include'lar ekle
- [ ] `main.cpp` - Composite adapter oluştur
- [ ] `main.cpp` - Adapter'ları composite'a ekle
- [ ] `Makefile` - Yeni .cpp dosyasını ekle

### Değişmeyen Dosyalar ✅

- [ ] `IExtrapTrackDataIncomingPort.hpp` - DEĞİŞMEZ
- [ ] `ExtrapTrackData.hpp` - DEĞİŞMEZ
- [ ] `IDelayCalcTrackDataOutgoingPort.hpp` - DEĞİŞMEZ
- [ ] `DelayCalcTrackData.hpp` - DEĞİŞMEZ
- [ ] `ProcessTrackUseCase.hpp/cpp` - DEĞİŞMEZ

### Test

- [ ] Derleme başarılı
- [ ] Composite adapter başlatıldı
- [ ] ZeroMQ ile veri gönderildi
- [ ] Kafka ile veri gönderildi
- [ ] Bir adapter hata verse diğeri çalışmaya devam etti

---

## 📋 Özet

| Soru | Cevap |
|------|-------|
| Mevcut yapı destekliyor muydu? | ❌ Hayır (tek port) |
| Çözüm | ✅ Composite Adapter Pattern |
| Domain değişikliği gerekti mi? | ❌ Hayır |
| Interface değişikliği gerekti mi? | ❌ Hayır |
| Kaç yeni dosya? | 3 (Composite + Kafka header/cpp) |
| Kaç güncelleme? | 2 (main.cpp + Makefile) |
| Genişletilebilir mi? | ✅ Sınırsız adapter eklenebilir |
