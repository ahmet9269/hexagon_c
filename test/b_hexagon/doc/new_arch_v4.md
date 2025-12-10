# 🏗️ New Architecture v4 - Thread-per-Component with Adapter Queue

Bu doküman, Hexagonal Architecture'ı SOLID ve DIP prensiplerine tam uyumlu hale getiren **yeni mimari tasarımı** detaylı olarak açıklar.

---

## 📋 Koşullar (Revize Edilmiş)

```
✅ Her model için IncomingAdapter, Service, OutgoingAdapter ayrı thread
✅ IncomingAdapter gelen veriyi KENDİ queue'suna yazar (tek işi bu)
✅ Domain servisi, IDataProvider interface'i üzerinden veri çeker
✅ Service, IDataPublisher interface'i üzerinden outgoing'e yazar
✅ OutgoingAdapter, KENDİ queue'sundan okuyup dışarı gönderir
```

---

## 🎯 Temel Prensipler

| Prensip | Uygulama |
|---------|----------|
| **SRP** | Her bileşen tek sorumluluk |
| **OCP** | Yeni adapter eklemek mevcut kodu değiştirmez |
| **LSP** | Interface implementasyonları değiştirilebilir |
| **ISP** | Her bileşen sadece ihtiyacı olan interface'i görür |
| **DIP** | Domain sadece interface'lere bağımlı, queue detayı gizli |

---

## 📐 Mimari Genel Bakış

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    NEW ARCHITECTURE v4                                      │
│                Thread-per-Component with Adapter Queue                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │  THREAD 1: IncomingAdapter                                          │   │
│   │  ════════════════════════════════════════════════════════════════   │   │
│   │                                                                     │   │
│   │   [ZeroMQ DISH] ──► deserialize() ──► ┌────────────────────────┐   │   │
│   │                                       │  📦 INTERNAL QUEUE     │   │   │
│   │                                       │  (adapter içinde)      │   │   │
│   │                                       └──────────┬─────────────┘   │   │
│   │                                                  │                 │   │
│   │   implements: IDataProvider<IncomingData>        │                 │   │
│   │   + tryReceive(timeout): optional<T>             │                 │   │
│   │   + hasData(): bool                              │                 │   │
│   └──────────────────────────────────────────────────┼─────────────────┘   │
│                                                      │                     │
│                                                      │ interface           │
│                                                      ▼                     │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │  DOMAIN LAYER: Port Interfaces                                      │   │
│   │  ════════════════════════════════════════════════════════════════   │   │
│   │                                                                     │   │
│   │   ┌─────────────────────────┐     ┌─────────────────────────┐      │   │
│   │   │  IDataProvider<T>       │     │  IDataPublisher<T>      │      │   │
│   │   │  (Primary Port)         │     │  (Secondary Port)       │      │   │
│   │   │                         │     │                         │      │   │
│   │   │  + tryReceive(timeout)  │     │  + publish(data)        │      │   │
│   │   │  + hasData()            │     │  + isReady()            │      │   │
│   │   │  + stop()               │     │  + stop()               │      │   │
│   │   └─────────────────────────┘     └─────────────────────────┘      │   │
│   │               ▲                               ▲                    │   │
│   │               │ uses                          │ uses               │   │
│   └───────────────┼───────────────────────────────┼────────────────────┘   │
│                   │                               │                        │
│   ┌───────────────┴───────────────────────────────┴────────────────────┐   │
│   │  THREAD 2: DomainService                                           │   │
│   │  ════════════════════════════════════════════════════════════════  │   │
│   │                                                                    │   │
│   │   std::shared_ptr<IDataProvider<In>> provider_;                    │   │
│   │   std::shared_ptr<IDataPublisher<Out>> publisher_;                 │   │
│   │                                                                    │   │
│   │   void run() {                                                     │   │
│   │       while (running_) {                                           │   │
│   │           auto data = provider_->tryReceive(100ms);  // interface  │   │
│   │           if (data) {                                              │   │
│   │               auto result = processBusinessLogic(*data);           │   │
│   │               publisher_->publish(result);           // interface  │   │
│   │           }                                                        │   │
│   │       }                                                            │   │
│   │   }                                                                │   │
│   │                                                                    │   │
│   │   ✅ Queue detayı bilmez - sadece interface kullanır              │   │
│   │   ✅ Pure business logic                                          │   │
│   └────────────────────────────────────────────────────────────────────┘   │
│                                                      │                     │
│                                                      │ interface           │
│                                                      ▼                     │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │  THREAD 3: OutgoingAdapter                                          │   │
│   │  ════════════════════════════════════════════════════════════════   │   │
│   │                                                                     │   │
│   │   implements: IDataPublisher<OutgoingData>                          │   │
│   │                                                                     │   │
│   │   ┌────────────────────────┐                                        │   │
│   │   │  📦 INTERNAL QUEUE     │ ──► serialize() ──► [ZeroMQ RADIO]    │   │
│   │   │  (adapter içinde)      │                                        │   │
│   │   └────────────────────────┘                                        │   │
│   │                                                                     │   │
│   │   void publish(const Data& data) {                                  │   │
│   │       enqueue(data);  // Non-blocking                               │   │
│   │   }                                                                 │   │
│   │                                                                     │   │
│   │   void workerThread() {                                             │   │
│   │       while (running_) {                                            │   │
│   │           auto data = dequeue();                                    │   │
│   │           sendToZeroMQ(data);                                       │   │
│   │       }                                                             │   │
│   │   }                                                                 │   │
│   └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🔄 Veri Akışı

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         DATA FLOW - NEW ARCH v4                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   EXTERNAL                                                                  │
│   ════════                                                                  │
│   [a_hexagon] ─── ZeroMQ ───►                                              │
│                               │                                             │
│   THREAD 1                    ▼                                             │
│   ════════   ┌────────────────────────────────────────────────────────┐    │
│              │  ZeroMQIncomingAdapter                                  │    │
│              │                                                         │    │
│              │  receiverWorker() {                                     │    │
│              │      data = zmq_recv();           // ZeroMQ'dan al      │    │
│              │      internal_queue_.push(data);  // Queue'ya yaz       │    │
│              │      cv_.notify_one();            // Service'i uyandır  │    │
│              │  }                                                      │    │
│              │                                                         │    │
│              │  // IDataProvider implementation                        │    │
│              │  tryReceive(timeout) {                                  │    │
│              │      cv_.wait_for(timeout);                             │    │
│              │      return internal_queue_.pop();                      │    │
│              │  }                                                      │    │
│              └─────────────────────────────────────┬──────────────────┘    │
│                                                    │                        │
│                                                    │ IDataProvider          │
│                                                    │ tryReceive()           │
│   THREAD 2                                         ▼                        │
│   ════════   ┌────────────────────────────────────────────────────────┐    │
│              │  ProcessingService                                      │    │
│              │                                                         │    │
│              │  run() {                                                │    │
│              │      while (running_) {                                 │    │
│              │          // Interface üzerinden al - queue bilmez       │    │
│              │          auto data = provider_->tryReceive(100ms);      │    │
│              │                                                         │    │
│              │          if (data) {                                    │    │
│              │              // PURE BUSINESS LOGIC                     │    │
│              │              auto result = calculate(*data);            │    │
│              │              result.setTimestamp(now());                │    │
│              │                                                         │    │
│              │              // Interface üzerinden yayınla             │    │
│              │              publisher_->publish(result);               │    │
│              │          }                                              │    │
│              │      }                                                  │    │
│              │  }                                                      │    │
│              └─────────────────────────────────────┬──────────────────┘    │
│                                                    │                        │
│                                                    │ IDataPublisher         │
│                                                    │ publish()              │
│   THREAD 3                                         ▼                        │
│   ════════   ┌────────────────────────────────────────────────────────┐    │
│              │  ZeroMQOutgoingAdapter                                  │    │
│              │                                                         │    │
│              │  // IDataPublisher implementation                       │    │
│              │  publish(data) {                                        │    │
│              │      internal_queue_.push(data);  // Non-blocking       │    │
│              │      cv_.notify_one();                                  │    │
│              │  }                                                      │    │
│              │                                                         │    │
│              │  publisherWorker() {                                    │    │
│              │      data = internal_queue_.pop();                      │    │
│              │      zmq_send(serialize(data));   // ZeroMQ'ya gönder   │    │
│              │  }                                                      │    │
│              └─────────────────────────────────────┬──────────────────┘    │
│                                                    │                        │
│   EXTERNAL                                         ▼                        │
│   ════════                               ─── ZeroMQ ───► [c_hexagon]       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 📁 Dosya Yapısı

```
src/
├── domain/
│   ├── ports/
│   │   ├── incoming/
│   │   │   ├── IDataProvider.hpp          ✨ YENİ - Generic provider interface
│   │   │   └── IDelayCalcTrackDataIncomingPort.hpp  (mevcut, opsiyonel)
│   │   │
│   │   ├── outgoing/
│   │   │   ├── IDataPublisher.hpp         ✨ YENİ - Generic publisher interface
│   │   │   └── ITrackDataStatisticOutgoingPort.hpp  (mevcut, opsiyonel)
│   │   │
│   │   ├── DelayCalcTrackData.hpp         (mevcut)
│   │   └── FinalCalcTrackData.hpp         (mevcut)
│   │
│   └── logic/
│       ├── FinalCalculationService.hpp    🔧 GÜNCELLE - Thread + interface
│       └── FinalCalculationService.cpp    🔧 GÜNCELLE
│
├── adapters/
│   ├── common/
│   │   ├── IAdapter.hpp                   (mevcut)
│   │   ├── ThreadSafeQueue.hpp            ✨ YENİ - Reusable queue
│   │   ├── AdapterManager.hpp             (mevcut)
│   │   └── MessagePipeline.hpp            (mevcut)
│   │
│   ├── incoming/
│   │   └── zeromq/
│   │       ├── TrackDataZeroMQIncomingAdapter.hpp  🔧 GÜNCELLE - IDataProvider impl
│   │       └── TrackDataZeroMQIncomingAdapter.cpp  🔧 GÜNCELLE
│   │
│   └── outgoing/
│       └── zeromq/
│           ├── FinalCalcTrackDataZeroMQOutgoingAdapter.hpp  (mevcut, uyumlu)
│           └── FinalCalcTrackDataZeroMQOutgoingAdapter.cpp  (mevcut, uyumlu)
│
└── main.cpp                               🔧 GÜNCELLE - Yeni wiring
```

---

## 🔧 Interface Tanımları

### 1. IDataProvider (Primary Port)

```cpp
/**
 * @file IDataProvider.hpp
 * @brief Generic interface for receiving data from adapters
 * @details Domain uses this interface to receive data without knowing
 *          the underlying queue or transport mechanism.
 * 
 * @note DIP compliant - Domain depends on abstraction, not implementation
 */

#pragma once

#include <optional>
#include <chrono>

namespace domain {
namespace ports {
namespace incoming {

/**
 * @class IDataProvider
 * @brief Abstract interface for data providers
 * @tparam T The type of data to provide
 * 
 * @par Hexagonal Architecture Role
 * This is a Primary Port - defines how external data enters the domain.
 * Implemented by IncomingAdapters.
 * 
 * @par Key Design Decisions
 * - tryReceive() with timeout: Non-blocking, allows graceful shutdown
 * - hasData(): Peek without blocking
 * - stop(): Coordinated shutdown
 * - No queue exposure: Implementation detail hidden
 */
template<typename T>
class IDataProvider {
public:
    virtual ~IDataProvider() = default;
    
    /**
     * @brief Try to receive data with timeout
     * @param timeout Maximum time to wait for data
     * @return Data if available, std::nullopt if timeout or stopped
     * 
     * @details Non-blocking call that allows the service to check
     *          for shutdown signals between receive attempts.
     */
    [[nodiscard]] virtual std::optional<T> tryReceive(
        std::chrono::milliseconds timeout) = 0;
    
    /**
     * @brief Check if data is available without blocking
     * @return true if data is available for immediate receive
     */
    [[nodiscard]] virtual bool hasData() const = 0;
    
    /**
     * @brief Signal provider to stop
     * @details Wakes up any waiting tryReceive() calls
     */
    virtual void stop() = 0;
    
    /**
     * @brief Check if provider is still active
     * @return true if provider is running and can provide data
     */
    [[nodiscard]] virtual bool isActive() const = 0;
};

} // namespace incoming
} // namespace ports
} // namespace domain
```

### 2. IDataPublisher (Secondary Port)

```cpp
/**
 * @file IDataPublisher.hpp
 * @brief Generic interface for publishing data to external systems
 * @details Domain uses this interface to send data without knowing
 *          the underlying queue or transport mechanism.
 * 
 * @note DIP compliant - Domain depends on abstraction, not implementation
 */

#pragma once

namespace domain {
namespace ports {
namespace outgoing {

/**
 * @class IDataPublisher
 * @brief Abstract interface for data publishers
 * @tparam T The type of data to publish
 * 
 * @par Hexagonal Architecture Role
 * This is a Secondary Port - defines how data exits the domain.
 * Implemented by OutgoingAdapters.
 * 
 * @par Key Design Decisions
 * - publish() is non-blocking: Returns immediately after queuing
 * - isReady(): Check if publisher can accept data
 * - No queue exposure: Implementation detail hidden
 */
template<typename T>
class IDataPublisher {
public:
    virtual ~IDataPublisher() = default;
    
    /**
     * @brief Publish data to external system
     * @param data Data to publish
     * 
     * @details Non-blocking call. Data is queued for async transmission.
     *          If queue is full, oldest message may be dropped (configurable).
     */
    virtual void publish(const T& data) = 0;
    
    /**
     * @brief Check if publisher is ready to accept data
     * @return true if publisher is running and can accept data
     */
    [[nodiscard]] virtual bool isReady() const = 0;
    
    /**
     * @brief Signal publisher to stop
     * @details Allows pending messages to be sent before shutdown
     */
    virtual void stop() = 0;
};

} // namespace outgoing
} // namespace ports
} // namespace domain
```

### 3. ThreadSafeQueue (Reusable Component)

```cpp
/**
 * @file ThreadSafeQueue.hpp
 * @brief Thread-safe queue for inter-thread communication
 * @details Used internally by adapters - NOT exposed to domain
 */

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>
#include <atomic>

namespace adapters {
namespace common {

/**
 * @class ThreadSafeQueue
 * @brief Lock-based thread-safe queue with timeout support
 * @tparam T Element type
 * 
 * @note This is an infrastructure component - should NOT be used in domain layer
 */
template<typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(std::size_t maxSize = 1000)
        : maxSize_(maxSize), stopped_(false) {}
    
    /**
     * @brief Push element to queue
     * @param item Element to push
     * @return true if pushed, false if queue is stopped
     */
    bool push(T item) {
        if (stopped_.load()) {
            return false;
        }
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            // Drop oldest if full
            if (queue_.size() >= maxSize_) {
                queue_.pop();
            }
            
            queue_.push(std::move(item));
        }
        
        cv_.notify_one();
        return true;
    }
    
    /**
     * @brief Try to pop with timeout
     * @param timeout Maximum time to wait
     * @return Element if available, nullopt otherwise
     */
    [[nodiscard]] std::optional<T> tryPop(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (cv_.wait_for(lock, timeout, [this]() {
            return !queue_.empty() || stopped_.load();
        })) {
            if (!queue_.empty()) {
                T item = std::move(queue_.front());
                queue_.pop();
                return item;
            }
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief Check if queue has data
     */
    [[nodiscard]] bool hasData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !queue_.empty();
    }
    
    /**
     * @brief Get current queue size
     */
    [[nodiscard]] std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    /**
     * @brief Stop the queue and wake up waiting threads
     */
    void stop() {
        stopped_.store(true);
        cv_.notify_all();
    }
    
    /**
     * @brief Check if queue is stopped
     */
    [[nodiscard]] bool isStopped() const {
        return stopped_.load();
    }
    
    /**
     * @brief Clear all elements
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::queue<T> empty;
        std::swap(queue_, empty);
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::size_t maxSize_;
    std::atomic<bool> stopped_;
};

} // namespace common
} // namespace adapters
```

---

## 🔧 Adapter Implementasyonları

### 1. IncomingAdapter (IDataProvider Implementasyonu)

```cpp
/**
 * @file TrackDataZeroMQIncomingAdapter.hpp
 * @brief ZeroMQ adapter implementing IDataProvider interface
 * @details Queue is internal - domain only sees the interface
 */

#pragma once

#include "adapters/common/IAdapter.hpp"
#include "adapters/common/ThreadSafeQueue.hpp"
#include "domain/ports/incoming/IDataProvider.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include <zmq.hpp>
#include <thread>
#include <atomic>

namespace adapters {
namespace incoming {
namespace zeromq {

using domain::ports::DelayCalcTrackData;

/**
 * @class TrackDataZeroMQIncomingAdapter
 * @brief Receives data via ZeroMQ and provides it through IDataProvider interface
 * 
 * @details
 * - THREAD 1 (internal): receiverWorker() - ZeroMQ recv → internal queue
 * - IDataProvider: Domain service calls tryReceive() to get data
 * - Queue is INTERNAL - domain doesn't know about it
 */
class TrackDataZeroMQIncomingAdapter 
    : public IAdapter
    , public domain::ports::incoming::IDataProvider<DelayCalcTrackData> {
    
public:
    TrackDataZeroMQIncomingAdapter(
        const std::string& endpoint,
        const std::string& group);
    
    ~TrackDataZeroMQIncomingAdapter() override;
    
    // ═══════════════════════════════════════════════════════════════════
    // IAdapter Implementation (Lifecycle)
    // ═══════════════════════════════════════════════════════════════════
    
    [[nodiscard]] bool start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;
    [[nodiscard]] std::string getName() const override;
    
    // ═══════════════════════════════════════════════════════════════════
    // IDataProvider Implementation (Domain uses this)
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Domain calls this to receive data
     * @details Domain doesn't know about internal queue - just calls interface
     */
    [[nodiscard]] std::optional<DelayCalcTrackData> tryReceive(
        std::chrono::milliseconds timeout) override {
        return internal_queue_.tryPop(timeout);
    }
    
    [[nodiscard]] bool hasData() const override {
        return internal_queue_.hasData();
    }
    
    [[nodiscard]] bool isActive() const override {
        return running_.load() && !internal_queue_.isStopped();
    }

private:
    /**
     * @brief Internal worker thread - receives from ZeroMQ, writes to queue
     */
    void receiverWorker();
    
    // Configuration
    std::string endpoint_;
    std::string group_;
    std::string adapter_name_;
    
    // ZeroMQ
    zmq::context_t zmq_context_;
    std::unique_ptr<zmq::socket_t> dish_socket_;
    
    // Threading
    std::thread receiver_thread_;
    std::atomic<bool> running_{false};
    
    // ✅ INTERNAL QUEUE - Domain doesn't see this
    common::ThreadSafeQueue<DelayCalcTrackData> internal_queue_;
};

} // namespace zeromq
} // namespace incoming
} // namespace adapters
```

### 2. Domain Service (Interface Kullanan)

```cpp
/**
 * @file FinalCalculationService.hpp
 * @brief Domain service with its own thread
 * @details Uses interfaces only - no knowledge of queues
 */

#pragma once

#include "domain/ports/incoming/IDataProvider.hpp"
#include "domain/ports/outgoing/IDataPublisher.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include "domain/ports/FinalCalcTrackData.hpp"
#include <thread>
#include <atomic>
#include <memory>

namespace domain {
namespace logic {

using ports::DelayCalcTrackData;
using ports::FinalCalcTrackData;

/**
 * @class FinalCalculationService
 * @brief Domain service running in its own thread
 * 
 * @details
 * - Has its OWN thread (THREAD 2)
 * - Uses IDataProvider to receive data (no queue knowledge)
 * - Uses IDataPublisher to publish results (no queue knowledge)
 * - Contains PURE business logic
 * 
 * @par DIP Compliance
 * - Depends on IDataProvider<T> abstraction
 * - Depends on IDataPublisher<T> abstraction
 * - No knowledge of queues or ZeroMQ
 */
class FinalCalculationService {
public:
    FinalCalculationService(
        std::shared_ptr<ports::incoming::IDataProvider<DelayCalcTrackData>> provider,
        std::shared_ptr<ports::outgoing::IDataPublisher<FinalCalcTrackData>> publisher);
    
    ~FinalCalculationService();
    
    // Delete copy/move
    FinalCalculationService(const FinalCalculationService&) = delete;
    FinalCalculationService& operator=(const FinalCalculationService&) = delete;
    
    /**
     * @brief Start the service thread
     */
    bool start();
    
    /**
     * @brief Stop the service thread gracefully
     */
    void stop();
    
    /**
     * @brief Check if service is running
     */
    [[nodiscard]] bool isRunning() const;

private:
    /**
     * @brief Main service loop running in dedicated thread
     */
    void run();
    
    /**
     * @brief Pure business logic - no infrastructure dependencies
     */
    FinalCalcTrackData processBusinessLogic(const DelayCalcTrackData& input);
    
    // ═══════════════════════════════════════════════════════════════════
    // Dependencies - INTERFACES ONLY (DIP compliant)
    // ═══════════════════════════════════════════════════════════════════
    
    std::shared_ptr<ports::incoming::IDataProvider<DelayCalcTrackData>> provider_;
    std::shared_ptr<ports::outgoing::IDataPublisher<FinalCalcTrackData>> publisher_;
    
    // ═══════════════════════════════════════════════════════════════════
    // Own thread
    // ═══════════════════════════════════════════════════════════════════
    
    std::thread service_thread_;
    std::atomic<bool> running_{false};
    
    static constexpr auto RECEIVE_TIMEOUT = std::chrono::milliseconds(100);
};

} // namespace logic
} // namespace domain
```

### 3. Domain Service Implementation

```cpp
/**
 * @file FinalCalculationService.cpp
 * @brief Implementation of domain service
 */

#include "FinalCalculationService.hpp"
#include "utils/Logger.hpp"
#include <chrono>

namespace domain {
namespace logic {

FinalCalculationService::FinalCalculationService(
    std::shared_ptr<ports::incoming::IDataProvider<DelayCalcTrackData>> provider,
    std::shared_ptr<ports::outgoing::IDataPublisher<FinalCalcTrackData>> publisher)
    : provider_(std::move(provider))
    , publisher_(std::move(publisher)) {
    
    LOG_DEBUG("FinalCalculationService created with interface dependencies");
}

FinalCalculationService::~FinalCalculationService() {
    stop();
}

bool FinalCalculationService::start() {
    if (running_.load()) {
        return true;
    }
    
    if (!provider_ || !publisher_) {
        LOG_ERROR("Cannot start service - missing dependencies");
        return false;
    }
    
    running_.store(true);
    
    service_thread_ = std::thread([this]() {
        run();
    });
    
    LOG_INFO("FinalCalculationService started in dedicated thread");
    return true;
}

void FinalCalculationService::stop() {
    if (!running_.load()) {
        return;
    }
    
    running_.store(false);
    
    // Provider'ı durdur - tryReceive() beklemesini sonlandırır
    if (provider_) {
        provider_->stop();
    }
    
    if (service_thread_.joinable()) {
        service_thread_.join();
    }
    
    LOG_INFO("FinalCalculationService stopped");
}

bool FinalCalculationService::isRunning() const {
    return running_.load();
}

void FinalCalculationService::run() {
    LOG_DEBUG("Service thread started");
    
    while (running_.load()) {
        // ═══════════════════════════════════════════════════════════════
        // RECEIVE via INTERFACE - no queue knowledge
        // ═══════════════════════════════════════════════════════════════
        auto data = provider_->tryReceive(RECEIVE_TIMEOUT);
        
        if (!data.has_value()) {
            // Timeout - check running flag and continue
            continue;
        }
        
        // ═══════════════════════════════════════════════════════════════
        // PURE BUSINESS LOGIC - no infrastructure code
        // ═══════════════════════════════════════════════════════════════
        FinalCalcTrackData result = processBusinessLogic(data.value());
        
        // ═══════════════════════════════════════════════════════════════
        // PUBLISH via INTERFACE - no queue knowledge
        // ═══════════════════════════════════════════════════════════════
        if (publisher_->isReady()) {
            publisher_->publish(result);
            
            LOG_DEBUG("Processed and published TrackID={}", result.getTrackId());
        } else {
            LOG_WARN("Publisher not ready, dropping result for TrackID={}", 
                    result.getTrackId());
        }
    }
    
    LOG_DEBUG("Service thread ended");
}

FinalCalcTrackData FinalCalculationService::processBusinessLogic(
    const DelayCalcTrackData& input) {
    
    FinalCalcTrackData result;
    
    // Copy track data
    result.setTrackId(input.getTrackId());
    result.setXPositionECEF(input.getXPositionECEF());
    result.setYPositionECEF(input.getYPositionECEF());
    result.setZPositionECEF(input.getZPositionECEF());
    result.setXVelocityECEF(input.getXVelocityECEF());
    result.setYVelocityECEF(input.getYVelocityECEF());
    result.setZVelocityECEF(input.getZVelocityECEF());
    
    // Calculate timing
    auto currentTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    result.setThirdHopSentTime(currentTime);
    result.setSecondHopSentTime(input.getSecondHopSentTime());
    result.setFirstHopDelayTime(input.getFirstHopDelayTime());
    
    auto secondHopDelay = currentTime - input.getSecondHopSentTime();
    result.setSecondHopDelayTime(secondHopDelay);
    result.setTotalDelayTime(input.getFirstHopDelayTime() + secondHopDelay);
    
    return result;
}

} // namespace logic
} // namespace domain
```

---

## 🔌 Main.cpp Wiring

```cpp
/**
 * @file main.cpp
 * @brief Application entry point with new architecture wiring
 */

#include <iostream>
#include <csignal>
#include <atomic>

#include "utils/Logger.hpp"
#include "adapters/common/AdapterManager.hpp"
#include "adapters/incoming/zeromq/TrackDataZeroMQIncomingAdapter.hpp"
#include "adapters/outgoing/zeromq/FinalCalcTrackDataZeroMQOutgoingAdapter.hpp"
#include "domain/logic/FinalCalculationService.hpp"

static std::atomic<bool> g_running{true};

void signalHandler(int signum) {
    LOG_INFO("Received signal {}, initiating shutdown...", signum);
    g_running.store(false);
}

int main() {
    try {
        utils::Logger::init("c_hexagon");
        
        LOG_INFO("=================================================");
        LOG_INFO("  C_Hexagon - New Architecture v4");
        LOG_INFO("  Thread-per-Component with Adapter Queue");
        LOG_INFO("=================================================");
        
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // ═══════════════════════════════════════════════════════════════
        // CREATE ADAPTERS
        // ═══════════════════════════════════════════════════════════════
        
        // Incoming Adapter (implements IDataProvider)
        auto incoming_adapter = std::make_shared<
            adapters::incoming::zeromq::TrackDataZeroMQIncomingAdapter>(
                "udp://239.1.1.5:9595",
                "DelayCalcTrackData"
        );
        
        // Outgoing Adapter (implements IDataPublisher)
        auto outgoing_adapter = std::make_shared<
            adapters::outgoing::zeromq::FinalCalcTrackDataZeroMQOutgoingAdapter>(
                "tcp://127.0.0.1:15003",
                "FinalCalcTrackData"
        );
        
        // ═══════════════════════════════════════════════════════════════
        // CREATE DOMAIN SERVICE with INTERFACE DEPENDENCIES
        // ═══════════════════════════════════════════════════════════════
        
        // Service receives interfaces - no knowledge of adapters or queues
        auto service = std::make_shared<domain::logic::FinalCalculationService>(
            incoming_adapter,   // IDataProvider<DelayCalcTrackData>
            outgoing_adapter    // IDataPublisher<FinalCalcTrackData>
        );
        
        // ═══════════════════════════════════════════════════════════════
        // START ALL COMPONENTS (3 threads will be created)
        // ═══════════════════════════════════════════════════════════════
        
        LOG_INFO("Starting all components...");
        
        // Thread 1: IncomingAdapter receiver worker
        if (!incoming_adapter->start()) {
            LOG_ERROR("Failed to start incoming adapter");
            return 1;
        }
        
        // Thread 2: Domain service worker
        if (!service->start()) {
            LOG_ERROR("Failed to start domain service");
            return 1;
        }
        
        // Thread 3: OutgoingAdapter publisher worker
        if (!outgoing_adapter->start()) {
            LOG_ERROR("Failed to start outgoing adapter");
            return 1;
        }
        
        LOG_INFO("All components started successfully");
        LOG_INFO("  - Thread 1: IncomingAdapter (ZeroMQ → Queue)");
        LOG_INFO("  - Thread 2: DomainService (Queue → Logic → Queue)");
        LOG_INFO("  - Thread 3: OutgoingAdapter (Queue → ZeroMQ)");
        LOG_INFO("Press Ctrl+C to shutdown gracefully");
        
        // ═══════════════════════════════════════════════════════════════
        // MAIN LOOP
        // ═══════════════════════════════════════════════════════════════
        
        while (g_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // ═══════════════════════════════════════════════════════════════
        // GRACEFUL SHUTDOWN
        // ═══════════════════════════════════════════════════════════════
        
        LOG_INFO("Stopping all components...");
        
        // Stop in reverse order
        service->stop();
        outgoing_adapter->stop();
        incoming_adapter->stop();
        
        LOG_INFO("=================================================");
        LOG_INFO("  C_Hexagon Shutdown Complete");
        LOG_INFO("=================================================");
        
        utils::Logger::shutdown();
        
    } catch (const std::exception& e) {
        LOG_CRITICAL("Application error: {}", e.what());
        return 1;
    }
    
    return 0;
}
```

---

## 📊 Mevcut Yapı vs Yeni Yapı Karşılaştırması

| Özellik | Mevcut Yapı | Yeni Arch v4 |
|---------|-------------|--------------|
| **Thread Sayısı** | 2 (In + Out) | 3 (In + Service + Out) |
| **Service Thread** | ❌ InAdapter'da çalışıyor | ✅ Kendi thread'inde |
| **Incoming Queue** | ❌ Yok | ✅ Adapter içinde |
| **Domain → Queue Bağımlılığı** | N/A | ❌ Yok (interface) |
| **DIP Uyumu** | ⚠️ Kısmen | ✅ Tam |
| **SRP Uyumu** | ⚠️ Kısmen | ✅ Tam |
| **Test Edilebilirlik** | ⚠️ Zor | ✅ Mock ile kolay |

---

## ✅ Değişiklik Listesi

### Yeni Dosyalar

| Dosya | Açıklama |
|-------|----------|
| `domain/ports/incoming/IDataProvider.hpp` | Generic provider interface |
| `domain/ports/outgoing/IDataPublisher.hpp` | Generic publisher interface |
| `adapters/common/ThreadSafeQueue.hpp` | Reusable queue component |

### Güncellenecek Dosyalar

| Dosya | Değişiklik |
|-------|------------|
| `TrackDataZeroMQIncomingAdapter.hpp/cpp` | IDataProvider impl, internal queue |
| `FinalCalculationService.hpp/cpp` | Kendi thread, interface dependencies |
| `main.cpp` | Yeni wiring |

### Değişmeyen Dosyalar

| Dosya | Neden |
|-------|-------|
| `FinalCalcTrackDataZeroMQOutgoingAdapter` | Zaten IDataPublisher gibi çalışıyor |
| `DelayCalcTrackData.hpp` | Model değişmiyor |
| `FinalCalcTrackData.hpp` | Model değişmiyor |

---

## 🧪 Test Stratejisi

```cpp
// Mock ile domain service testi
TEST(FinalCalculationServiceTest, ProcessesDataCorrectly) {
    // Mock provider
    auto mock_provider = std::make_shared<MockDataProvider>();
    EXPECT_CALL(*mock_provider, tryReceive(_))
        .WillOnce(Return(std::optional<DelayCalcTrackData>(testData)));
    
    // Mock publisher
    auto mock_publisher = std::make_shared<MockDataPublisher>();
    EXPECT_CALL(*mock_publisher, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_publisher, publish(_)).Times(1);
    
    // Create service with mocks - NO real adapters needed
    auto service = FinalCalculationService(mock_provider, mock_publisher);
    
    // Test pure business logic
    service.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    service.stop();
}
```

---

## 📋 Implementasyon Sırası

| Adım | Dosya | İşlem |
|------|-------|-------|
| 1 | `IDataProvider.hpp` | ✨ Oluştur |
| 2 | `IDataPublisher.hpp` | ✨ Oluştur |
| 3 | `ThreadSafeQueue.hpp` | ✨ Oluştur |
| 4 | `TrackDataZeroMQIncomingAdapter` | 🔧 Güncelle |
| 5 | `FinalCalculationService` | 🔧 Güncelle |
| 6 | `main.cpp` | 🔧 Güncelle |
| 7 | Test | 🧪 Doğrula |

---

## 📝 Özet

| Soru | Cevap |
|------|-------|
| Thread yapısı | ✅ 3 ayrı thread (In, Service, Out) |
| Queue konumu | ✅ Adapter içinde (domain bilmez) |
| DIP uyumu | ✅ Tam uyumlu |
| SOLID uyumu | ✅ Tam uyumlu |
| Hexagonal uyumu | ✅ Tam uyumlu |
| Test edilebilirlik | ✅ Mock ile kolay |
| Mevcut kod etkisi | 🟡 Orta (Service ve Adapter güncelleme) |
