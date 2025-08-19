// #pragma once = Header guard modern syntax'ı
// Bu header dosyasının aynı compilation unit'te sadece bir kez include edilmesini sağlar
// Multiple inclusion problem'ini önler, compilation time optimize eder
#pragma once

// DOMAIN MODEL INCLUDES - İş mantığı modelleri
// ../../ = iki seviye yukarı çık (outgoing -> ports -> domain -> src)
// Relative path kullanımı, proje yapısına bağımlılık
#include "../../model/DelayCalcTrackData.hpp"    // Gecikme hesaplama track verisi
#include "../../model/FinalCalcDelayData.hpp"    // Final gecikme hesaplama verisi

// STL (Standard Template Library) INCLUDES
#include <vector>      // Dynamic array için (std::vector<T>)
#include <memory>      // Smart pointer'lar için (std::shared_ptr, std::unique_ptr)

// NAMESPACE DEFINITION - Hat projesinin domain katmanı outgoing ports namespace'i
// :: = namespace separator (scope resolution operator)
// Nested namespace: hat -> domain -> ports -> outgoing
// Outgoing = Domain'den dış dünyaya çıkan interface'ler
namespace hat::domain::ports::outgoing {

/**
 * SECONDARY PORT INTERFACE - Hexagonal Architecture'da Secondary Port
 * 
 * HEXAGONAL ARCHITECTURE AÇIKLAMASI:
 * - Secondary Port = Domain'in dış dünyayı kullanması için tanımladığı interface
 * - Bu port DOMAIN LOGIC tarafından çağrılır (outgoing direction)
 * - ADAPTER'lar bu port'u implement eder (ZeroMQTrackDataPublisher)
 * - İşlenen verileri ZeroMQ üzerinden dışarı gönderen arayüz
 * 
 * DESIGN PATTERN: Port & Adapter Pattern
 * - Port = Interface definition (bu dosya) 
 * - Adapter = External system integration (ZeroMQTrackDataPublisher)
 * - Domain Logic = Business rules implementation (TrackDataProcessor)
 * 
 * DEPENDENCY DIRECTION:
 * Domain Logic -> Secondary Port (interface) <- Secondary Adapter
 * Domain sadece interface'i bilir, concrete implementation'ı bilmez
 */
class TrackDataPublisher {
public:    // Public section - dışarıdan erişilebilir members
    
    // VIRTUAL DESTRUCTOR - Polymorphism için kritik
    // virtual = bu fonksiyon derived class'larda override edilebilir
    // ~ = destructor operator (object yok edilirken çağrılır)
    // = default = compiler'ın default implementation'ını kullan
    // 
    // NEDEN VIRTUAL DESTRUCTOR GEREKLİ:
    // - Base class pointer ile derived object silinirken
    // - Derived class'ın destructor'ının doğru çağrılması için
    // - Resource leak'leri (memory, file handle, socket) önlemek için
    // - C++ polymorphism'in temel kuralı
    virtual ~TrackDataPublisher() = default;

    /**
     * DELAY CALC TRACK DATA PUBLISHING - Ana veri yayınlama fonksiyonu
     * İşlenen DelayCalcTrackData'yı dış dünyaya yayınlar
     * 
     * PURE VIRTUAL FUNCTION (= 0):
     * - Bu class'da implementation yok, abstract class yapıyor
     * - Derived class'larda mutlaka implement edilmeli
     * - Interface contract'ı tanımlar, implementation detayını gizler
     * 
     * @param data Yayınlanacak veri
     * @return Yayınlama başarılı mı?
     */
    // virtual = polymorphic behavior (runtime'da hangi implementation çağrılacağı belirlenir)
    // bool = return type, success/failure indication
    // const model::DelayCalcTrackData& = referans parametresi
    //   - const = fonksiyon içinde parametre değiştirilemez (immutable)
    //   - & = reference, kopyalama yapmaz (performance optimization)
    //   - Large object'lerin kopyalanması maliyetli, reference kullan
    // = 0 = pure virtual function, implementation yok
    virtual bool publishDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * FINAL CALC DELAY DATA PUBLISHING - Final veri yayınlama fonksiyonu
     * İşlenen FinalCalcDelayData'yı dış dünyaya yayınlar
     * 
     * BUSINESS LOGIC CONTEXT:
     * - DelayCalcTrackData'dan türetilen final hesaplama sonucu
     * - Daha detaylı gecikme analizi içerir
     * - Monitoring ve reporting için kritik veri
     * 
     * @param data Yayınlanacak final veri
     * @return Yayınlama başarılı mı?
     */
    virtual bool publishFinalCalcDelayData(const model::FinalCalcDelayData& data) = 0;

    /**
     * PUBLISHER STATUS CHECK - Yayınlayıcı durum kontrolü
     * Publisher'ın aktif durumda olup olmadığını kontrol eder
     * 
     * CONST MEMBER FUNCTION:
     * - const = bu fonksiyon object'in state'ini değiştirmez
     * - Read-only operation garantisi
     * - Thread-safety için önemli ipucu
     * - Compiler optimization'a yardım eder
     * 
     * HEALTH CHECK PATTERN:
     * - System health monitoring için kullanılır
     * - Circuit breaker pattern'de kullanılabilir
     * - Graceful degradation için kritik
     * 
     * @return Publisher aktif mi?
     */
    // const = member function modifier, object state'ini değiştirmez
    virtual bool isPublisherActive() const = 0;

    /**
     * NESTED STRUCT - Publisher istatistik verileri
     * 
     * STRUCT vs CLASS:
     * - struct = default public members (C compatibility)
     * - class = default private members
     * - Functionality açısından aynı
     * - Struct genelde data container için kullanılır
     * 
     * STATISTICS PATTERN:
     * - System monitoring için kritik veriler
     * - Performance analysis için gerekli
     * - Debugging ve troubleshooting için yararlı
     */
    struct PublisherStats {
        // size_t = platform-specific unsigned integer type
        // Genelde pointer boyutunda (32-bit'te 4 byte, 64-bit'te 8 byte)
        // Array indexing ve memory size için optimize edilmiş
        size_t total_published;        // Toplam yayınlanan mesaj sayısı
        size_t failed_publications;    // Başarısız yayın sayısı
        size_t active_connections;     // Aktif bağlantı sayısı
        
        // double = double-precision floating point (64-bit)
        // float'tan daha hassas, scientific calculation'lar için uygun
        // Latency measurement için yeterli precision
        double average_latency_ms;     // Ortalama gecikme süresi (milisaniye)
        
        // STRUCT DESIGN NOTES:
        // - POD (Plain Old Data) struct
        // - Copyable ve movable
        // - Stack'te efficient storage
        // - Serialization'a uygun
    };

    /**
     * STATISTICS RETRIEVAL - İstatistik verilerini alma fonksiyonu
     * Publisher'ın performans ve durum istatistiklerini döner
     * 
     * RETURN BY VALUE:
     * - Struct küçük olduğu için copy overhead düşük
     * - RVO (Return Value Optimization) ile optimize edilebilir
     * - Thread-safe (caller'ın kendi copy'si)
     * 
     * MONITORING PATTERN:
     * - Real-time system monitoring
     * - Performance metrics collection
     * - Alerting ve dashboard'lar için veri kaynağı
     * 
     * @return Publisher istatistikleri
     */
    virtual PublisherStats getPublisherStats() const = 0;

    // INTERFACE DESIGN PRINCIPLES:
    // 
    // 1. INTERFACE SEGREGATION PRINCIPLE (ISP):
    //    - Sadece publisher'a özgü metodlar var
    //    - Client'lar kullanmadıkları interface'lere depend etmez
    //    - Focused, cohesive interface
    // 
    // 2. DEPENDENCY INVERSION PRINCIPLE (DIP):
    //    - High-level modules (Domain Logic) low-level modules'a (ZeroMQ) depend etmez
    //    - Her ikisi de abstraction'lara (bu interface) depend eder
    //    - Testability ve flexibility artırır
    // 
    // 3. SINGLE RESPONSIBILITY PRINCIPLE (SRP):
    //    - Sadece data publishing sorumluluğu
    //    - Data processing, storage vs. başka interface'lerde
    // 
    // 4. OPEN/CLOSED PRINCIPLE (OCP):
    //    - Extension'a açık (yeni implementation'lar eklenebilir)
    //    - Modification'a kapalı (mevcut interface değişmez)
    //    - ZeroMQ, HTTP, File gibi farklı implementation'lar eklenebilir

}; // class TrackDataPublisher sonu

// NAMESPACE CLOSING - Hat projesinin domain katmanı outgoing ports namespace'i kapanışı
} // namespace hat::domain::ports::outgoing

// HEXAGONAL ARCHITECTURE FLOW (OUTGOING):
// 1. TrackDataProcessor (Domain Logic) -> business rules uygular
// 2. TrackDataPublisher interface (Secondary Port) -> dış dünyaya çıkış noktası  
// 3. ZeroMQTrackDataPublisher (Secondary Adapter) -> ZeroMQ implementation
// 4. ZeroMQ Network -> external systems'e mesaj gönderir
// 5. External Consumers -> published data'yı alır ve işler

// DESIGN PATTERNS USED:
// - Port & Adapter Pattern (Hexagonal Architecture)
// - Strategy Pattern (farklı publisher implementation'ları)
// - Observer Pattern (publish-subscribe messaging)
// - Statistics Pattern (performance monitoring)

// BENEFITS OF THIS DESIGN:
// - Testability: Mock implementation'lar kolayca yazılabilir
// - Flexibility: ZeroMQ yerine farklı transport'lar kullanılabilir
// - Maintainability: Interface değişmeden implementation güncellenebilir
// - Separation of Concerns: Domain logic transport detaylarını bilmez