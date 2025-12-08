# 🏗️ B_Hexagon - Çoklu Outgoing Adapter Rehberi (v2)

**Amaç:** Tek IncomingAdapter'dan gelen veriyi birden fazla OutgoingAdapter ile farklı teknolojilere göndermek.

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
    
    void sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) override {
        for (auto& adapter : portAdapters_) {
            try {
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

## Adım 2: KafkaOutgoingAdapter.hpp

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

    // IDelayCalcTrackDataOutgoingPort
    void sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) override;

private:
    void workerThread();

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

---

## Adım 3: KafkaOutgoingAdapter.cpp

**Dosya:** `src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp`

```cpp
#include "KafkaOutgoingAdapter.hpp"
#include "utils/Logger.hpp"

namespace adapters {
namespace outgoing {
namespace kafka {

KafkaOutgoingAdapter::KafkaOutgoingAdapter(const std::string& brokers, const std::string& topic)
    : brokers_(brokers), topic_(topic), adapterName_("KafkaAdapter-" + topic) {}

KafkaOutgoingAdapter::~KafkaOutgoingAdapter() { stop(); }

bool KafkaOutgoingAdapter::start() {
    if (running_.load()) return true;
    
    // 🔧 Kafka producer initialization here
    
    stopRequested_.store(false);
    running_.store(true);
    workerThread_ = std::thread([this]() { workerThread(); });
    return true;
}

void KafkaOutgoingAdapter::stop() {
    if (!running_.load()) return;
    stopRequested_.store(true);
    running_.store(false);
    queueCv_.notify_all();
    if (workerThread_.joinable()) workerThread_.join();
}

bool KafkaOutgoingAdapter::isRunning() const { return running_.load(); }
std::string KafkaOutgoingAdapter::getName() const noexcept { return adapterName_; }

void KafkaOutgoingAdapter::sendDelayCalcTrackData(const domain::ports::DelayCalcTrackData& data) {
    if (!running_.load()) return;
    
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (messageQueue_.size() >= MAX_QUEUE_SIZE) messageQueue_.pop();
    messageQueue_.push(data);
    queueCv_.notify_one();
}

void KafkaOutgoingAdapter::workerThread() {
    while (!stopRequested_.load()) {
        domain::ports::DelayCalcTrackData data;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCv_.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                return !messageQueue_.empty() || stopRequested_.load();
            });
            if (stopRequested_.load() && messageQueue_.empty()) break;
            if (messageQueue_.empty()) continue;
            data = std::move(messageQueue_.front());
            messageQueue_.pop();
        }
        
        // 🔧 Kafka produce here
        auto serialized = data.serialize();
        // producer_->produce(topic_, serialized);
    }
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

// 1. Mevcut ZeroMQ Adapter
auto zmq_adapter = std::make_shared<DelayCalcTrackDataZeroMQOutgoingAdapter>(
    "udp://239.1.1.5:9595", "DelayCalcTrackData");

// 2. Yeni Kafka Adapter
auto kafka_adapter = std::make_shared<adapters::outgoing::kafka::KafkaOutgoingAdapter>(
    "localhost:9092", "delay-calc-topic");

// 3. Composite ile birleştir
auto composite = std::make_shared<adapters::outgoing::CompositeOutgoingAdapter>();
composite->addAdapterWithLifecycle(zmq_adapter);
composite->addAdapterWithLifecycle(kafka_adapter);

// 4. UseCase'e inject et (domain değişmedi!)
auto useCase = std::make_shared<domain::logic::ProcessTrackUseCase>(
    std::make_unique<CalculatorService>(),
    composite  // 👈 Tek adapter olarak geçiyor
);
```

---

## Adım 5: Makefile

```makefile
SOURCES += \
    src/b_hexagon/adapters/outgoing/kafka/KafkaOutgoingAdapter.cpp
```

---

## ✅ Checklist

- [ ] `CompositeOutgoingAdapter.hpp` oluştur
- [ ] `KafkaOutgoingAdapter.hpp` oluştur
- [ ] `KafkaOutgoingAdapter.cpp` oluştur
- [ ] `main.cpp` güncelle
- [ ] `Makefile` güncelle
- [ ] Derleme başarılı
- [ ] Test: Her iki adapter'a veri gidiyor
