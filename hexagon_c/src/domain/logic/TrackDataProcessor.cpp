// Header dosyasını dahil et - Bu class'ın declaration'ını içerir
#include "TrackDataProcessor.hpp"
// Console I/O için (std::cout, std::endl)
#include <iostream>
// Zaman işlemleri için (std::chrono::system_clock, duration_cast)
#include <chrono>

// Hat projesi namespace'i altında, domain->logic alt namespace'i
// Domain Logic = Hexagonal Architecture'ın kalbi, pure business logic
namespace hat::domain::logic {

// CONSTRUCTOR - TrackDataProcessor class'ının constructor'ı
// Dependency Injection pattern ile bağımlılıkları alır
TrackDataProcessor::TrackDataProcessor(
    // std::shared_ptr<T> = Reference counted smart pointer
    // ports::outgoing::TrackDataPublisher = Secondary port interface
    // publisher = Veri yayınlama için kullanılacak
    std::shared_ptr<ports::outgoing::TrackDataPublisher> publisher,
    // ports::outgoing::TrackDataRepository = Secondary port interface  
    // repository = Veri saklama için kullanılacak
    std::shared_ptr<ports::outgoing::TrackDataRepository> repository,
    // const ProcessorConfig& = Configuration struct referansı
    // config = İş mantığı parametreleri (thresholds, limits vs.)
    const ProcessorConfig& config)
    // MEMBER INITIALIZER LIST - Constructor body çalışmadan önce member'ları initialize eder
    // : operatörü ile başlar, , ile devam eder
    : publisher_(publisher)      // Publisher interface'ini kaydet
    , repository_(repository)    // Repository interface'ini kaydet  
    , config_(config) {          // Configuration'ı kaydet
    // Constructor body boş - tüm initialization member initializer list'te yapıldı
    // Bu yaklaşım daha verimli (direct initialization vs assignment)
}

// PRIMARY PORT IMPLEMENTATION - TrackDataSubmission interface'inin implementasyonu
// Bu fonksiyon Hexagonal Architecture'da PRIMARY PORT method'udur
// External adapter'lar (ZeroMQTrackDataSubscriber) bu method'u çağırır
bool TrackDataProcessor::submitDelayCalcTrackData(const model::DelayCalcTrackData& data) {
    // TRY-CATCH BLOCK - Exception safety için tüm business logic'i sar
    try {
        // BUSINESS RULE 1: DATA VALIDATION - Veri validasyonu
        // ! = logical NOT operator
        // Domain model'in kendi validation logic'ini kullan
        if (!data.isValid()) {
            // return = fonksiyondan çık ve false döndür (başarısız)
            return false;
        }

        // BUSINESS RULE 2: DATA FRESHNESS CHECK - Veri yaşı kontrolü
        // config_.max_acceptable_delay_ms = Configuration'dan threshold al
        // Domain-specific business rule: eski veriler kabul edilmeyebilir
        if (!data.isDataFresh(config_.max_acceptable_delay_ms)) {
            // Eski veri uyarısı - yine de işle ama logla
            // Gerçek uygulamada structured logging yapılacak
            // Bu durumda processing devam ediyor (business decision)
        }

        // BUSINESS RULE 3: HIGH VELOCITY DETECTION - Yüksek hız kontrolü
        // config_.high_velocity_threshold = Hız threshold'u
        // Yüksek hızlı objeler özel işlem gerektirebilir (radar tracking domain knowledge)
        if (data.isHighVelocity(config_.high_velocity_threshold)) {
            // Yüksek hız uyarısı - öncelikli işlem
            // Farklı processing pipeline'ı çağır
            return processHighVelocityData(data);
        }

        // NORMAL PROCESSING FLOW - Standart işlem akışı
        // Yukarıdaki special case'ler geçilmişse normal flow
        return processNormalDelayCalcData(data);

    // EXCEPTION HANDLING - Herhangi bir exception durumunda
    // const std::exception& = Standard exception referansı (polymorphic base)
    } catch (const std::exception& e) {
        // Hata yönetimi - Production'da detaylı logging yapılacak
        // Exception details: e.what() ile hata mesajı alınabilir
        return false; // Hata durumunda false döndür
    }
}

// BUSINESS LOGIC METHOD - FinalCalcDelayData üretimi ve yayınlama
// Bu method complex business transformation yapar
bool TrackDataProcessor::generateAndPublishFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
    try {
        // DATA TRANSFORMATION - Domain model transformation
        // auto = type inference: model::FinalCalcDelayData
        // createFinalCalcDelayData = private helper method
        auto final_data = createFinalCalcDelayData(input_data);
        
        // VALIDATION CHECK - Transformed data validation
        if (!final_data.isValid()) {
            return false; // Invalid transformation result
        }

        // BUSINESS RULE: PERFORMANCE ANALYSIS - Performans analizi
        // auto = type inference: model::FinalCalcDelayData::PerformanceStatus (enum)
        // Domain model'in kendi analysis method'unu kullan
        auto performance_status = final_data.getPerformanceStatus();
        
        // STRATEGY PATTERN - Performance status'e göre farklı processing
        // switch-case = compile-time optimized branching
        switch (performance_status) {
            // CRITICAL PERFORMANCE CASE - Kritik performans durumu
            // :: = scope resolution, enum value access
            case model::FinalCalcDelayData::PerformanceStatus::CRITICAL:
                // Kritik durum için özel processing
                return processCriticalPerformanceData(final_data);
            
            // POOR PERFORMANCE CASE - Zayıf performans durumu  
            case model::FinalCalcDelayData::PerformanceStatus::POOR:
                // Zayıf performans için özel processing
                return processPoorPerformanceData(final_data);
            
            // DEFAULT CASE - Normal performans durumu
            default:
                // Normal processing flow
                return processNormalFinalCalcData(final_data);
        }

    } catch (const std::exception& e) {
        // Exception safety - herhangi bir hata durumunda
        return false;
    }
}

// SYSTEM STATUS CHECK - Sistem hazır durumu kontrolü
// const = bu method object state'ini değiştirmez (read-only)
bool TrackDataProcessor::isReadyToReceive() const {
    // DEPENDENCY HEALTH CHECK - Bağımlılık sağlık kontrolü
    // && = logical AND operator, tüm koşullar true olmalı
    // publisher_ = smart pointer, nullptr check (implicit bool conversion)
    // -> = pointer member access operator
    return publisher_ && publisher_->isPublisherActive() && 
           repository_; // repository_ nullptr check
}

// PRIVATE HELPER METHOD - Normal DelayCalcTrackData işleme
// Bu method normal business flow'u implement eder
bool TrackDataProcessor::processNormalDelayCalcData(const model::DelayCalcTrackData& data) {
    // STEP 1: DATA PERSISTENCE - Veriyi sakla
    // -> = pointer member access, secondary port çağrısı
    if (!repository_->saveDelayCalcTrackData(data)) {
        return false; // Storage failure
    }

    // STEP 2: TRANSFORMATION & PUBLISHING - Dönüştür ve yayınla
    // FinalCalcDelayData üret ve gönder
    if (!generateAndPublishFinalCalcDelayData(data)) {
        return false; // Transformation/publishing failure
    }

    // STEP 3: ORIGINAL DATA PUBLISHING - Orijinal veriyi de yayınla (opsiyonel)
    // Business requirement: hem original hem transformed data yayınlanır
    return publisher_->publishDelayCalcTrackData(data);
}

// PRIVATE HELPER METHOD - Yüksek hız verisi özel işleme
// High velocity objects require special handling (domain-specific)
bool TrackDataProcessor::processHighVelocityData(const model::DelayCalcTrackData& data) {
    // Yüksek hızlı veriler için özel işlem
    // Business rule: High velocity = priority processing, skip some steps
    
    // STEP 1: PRIORITY STORAGE - Öncelikli saklama
    if (!repository_->saveDelayCalcTrackData(data)) {
        return false;
    }

    // STEP 2: IMMEDIATE PUBLISHING - Acil yayınlama
    // High velocity data için FinalCalc generation skip edilir (performance)
    if (!publisher_->publishDelayCalcTrackData(data)) {
        return false;
    }

    return true; // Success
}

// PRIVATE HELPER METHOD - Normal FinalCalcDelayData işleme
bool TrackDataProcessor::processNormalFinalCalcData(const model::FinalCalcDelayData& data) {
    // STEP 1: FINAL DATA STORAGE - Final veriyi sakla
    if (!repository_->saveFinalCalcDelayData(data)) {
        return false;
    }

    // STEP 2: PERFORMANCE METRICS - Performans metrikleri hesapla
    // auto = type inference: model::FinalCalcDelayData::PerformanceMetrics (struct)
    // Domain model'in analytics capability'si
    auto metrics = data.calculatePerformanceMetrics();
    // metrics variable kullanılmıyor şu an, ileride logging/monitoring için

    // STEP 3: FINAL DATA PUBLISHING - Final veriyi yayınla
    return publisher_->publishFinalCalcDelayData(data);
}

// PRIVATE HELPER METHOD - Kritik performans verisi işleme
// Critical performance requires immediate action, no error checking
bool TrackDataProcessor::processCriticalPerformanceData(const model::FinalCalcDelayData& data) {
    // Kritik durum - acil müdahale
    // Business rule: Critical = fire-and-forget, no error propagation
    
    // STEP 1: IMMEDIATE STORAGE - Acil saklama
    // Return value ignore edilir (critical durumda hata kontrolü yok)
    repository_->saveFinalCalcDelayData(data);

    // STEP 2: IMMEDIATE PUBLISHING - Acil yayınlama  
    // Return value ignore edilir
    publisher_->publishFinalCalcDelayData(data);

    return true; // Always success for critical data
}

// PRIVATE HELPER METHOD - Zayıf performans verisi işleme
bool TrackDataProcessor::processPoorPerformanceData(const model::FinalCalcDelayData& data) {
    // Performans düşüklüğü - izleme artır
    // Business rule: Poor performance = store and publish, increase monitoring
    
    // Fire-and-forget approach for poor performance
    repository_->saveFinalCalcDelayData(data);
    publisher_->publishFinalCalcDelayData(data);

    // Burada monitoring/alerting logic eklenebilir
    // Örnek: monitoring_system_->increaseMonitoringLevel(data.getTrackId());

    return true;
}

// CORE BUSINESS TRANSFORMATION METHOD - Ana business transformation
// DelayCalcTrackData'dan FinalCalcDelayData'ya dönüşüm
// Bu method domain'in core business logic'ini içerir
model::FinalCalcDelayData TrackDataProcessor::createFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
    // TIME CALCULATION - Kritik zaman hesaplaması
    // KRITIK: Alım zamanını al (hexagon_c'de işleme zamanı)
    // std::chrono::duration_cast<T> = duration type conversion
    // std::chrono::milliseconds = millisecond precision
    // std::chrono::system_clock::now() = current wall clock time
    // .time_since_epoch() = Unix epoch'tan beri geçen süre
    // .count() = numeric value extraction
    auto receive_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // BUSINESS CALCULATIONS - İş mantığı hesaplamaları
    // GERÇEK GECİKME HESAPLAMALARI:
    // hexagon_b'den gelen gerçek gönderim zamanı
    int64_t sent_time = input_data.getFirstHopSentTime();
    
    // USER SPECIFIED FORMULAS - Kullanıcı tanımlı formüller:
    // SecondHopDelayTime = ReceivedTime - FirstHopSentTime
    // - = arithmetic subtraction operator
    int64_t second_hop_delay_time = receive_time - sent_time;
    
    // TotalDelayTime = ReceivedTime - FirstHopSentTime  
    // (Bu örnekte aynı hesaplama, gerçekte daha complex olabilir)
    int64_t total_delay_time = receive_time - sent_time;
    
    // ThirdHopSentTime = CurrentTime (işleme zamanı)
    // Processing timestamp olarak current time kullan
    int64_t third_hop_sent_time = receive_time;
    
    // DOMAIN MODEL CONSTRUCTION - Domain model oluşturma
    // FinalCalcDelayData constructor çağrısı
    // Input data'dan gerekli field'ları extract et, calculated field'ları ekle
    model::FinalCalcDelayData final_data(
        input_data.getTrackId(),                    // track_id - primary key
        input_data.getXVelocityECEF(),             // x_vel_ecef - ECEF coordinate system
        input_data.getYVelocityECEF(),             // y_vel_ecef
        input_data.getZVelocityECEF(),             // z_vel_ecef
        input_data.getXPositionECEF(),             // x_pos_ecef - ECEF coordinate system
        input_data.getYPositionECEF(),             // y_pos_ecef
        input_data.getZPositionECEF(),             // z_pos_ecef
        input_data.getUpdateTime(),                 // update_time - original timestamp
        input_data.getOriginalUpdateTime(),         // original_update_time
        input_data.getFirstHopSentTime(),          // first_hop_sent - from input
        input_data.getFirstHopDelayTime(),         // first_hop_delay - from input
        input_data.getSecondHopSentTime(),         // second_hop_sent - from input
        second_hop_delay_time,                      // second_hop_delay (HESAPLANAN)
        total_delay_time,                           // total_delay (HESAPLANAN)
        third_hop_sent_time                         // third_hop_sent (HESAPLANAN)
    );
    
    // REAL-TIME MONITORING OUTPUT - Gerçek zamanlı izleme çıktısı
    // GERÇEK GECİKME CONSOLE ÇIKTISI
    // std::cout = console output stream
    // << = stream insertion operator
    std::cout << "📨 Received Track " << input_data.getTrackId() 
              << " - 📤 Sent: " << sent_time << " ms"
              << " - 📨 Received: " << receive_time << " ms"
              << " - ⏱️  DELAY: " << total_delay_time << " ms";
    
    // PERFORMANCE CATEGORIZATION - Performans kategorilendirmesi
    // static constexpr = compile-time constant, memory'de yer kaplamaz
    // int64_t = 64-bit signed integer
    // Gecikme kategorisi threshold'ları (domain knowledge)
    static constexpr int64_t EXCELLENT_THRESHOLD = 10;   // < 10ms = mükemmel
    static constexpr int64_t GOOD_THRESHOLD = 50;        // < 50ms = iyi
    static constexpr int64_t ACCEPTABLE_THRESHOLD = 100; // < 100ms = kabul edilebilir
    static constexpr int64_t POOR_THRESHOLD = 500;       // < 500ms = zayıf
    
    // CONDITIONAL LOGIC - Koşullu mantık ile kategori belirleme
    // if-else chain = sequential condition checking
    // < = less than comparison operator
    if (total_delay_time < EXCELLENT_THRESHOLD) std::cout << " 🟢 EXCELLENT";
    else if (total_delay_time < GOOD_THRESHOLD) std::cout << " 🟢 GOOD";
    else if (total_delay_time < ACCEPTABLE_THRESHOLD) std::cout << " 🟡 ACCEPTABLE";
    else if (total_delay_time < POOR_THRESHOLD) std::cout << " 🟠 POOR";
    else std::cout << " 🔴 CRITICAL";
    
    // std::endl = newline + flush buffer (console output'u hemen göster)
    std::cout << std::endl;
    
    // RETURN BY VALUE - Domain model'i value olarak döndür
    // RVO (Return Value Optimization) ile optimize edilebilir
    return final_data;
}

// NAMESPACE CLOSING - Domain logic namespace'i kapanışı
} // namespace hat::domain::logic

// DOMAIN LOGIC DESIGN PRINCIPLES:
//
// 1. PURE BUSINESS LOGIC:
//    - Sadece domain kuralları ve hesaplamalar
//    - Infrastructure concern'leri yok (ZeroMQ, file I/O vs.)
//    - Testable ve maintainable
//
// 2. DEPENDENCY INVERSION:
//    - Concrete class'lara değil, interface'lere depend eder
//    - Publisher ve Repository interface'leri kullanır
//    - Loose coupling, high cohesion
//
// 3. SINGLE RESPONSIBILITY:
//    - Sadece track data processing sorumluluğu
//    - Data transformation ve business rule enforcement
//    - Clear, focused responsibility
//
// 4. EXCEPTION SAFETY:
//    - Tüm public method'lar exception-safe
//    - Resource leak'leri önlenir
//    - Graceful error handling
//
// 5. CONFIGURATION-DRIVEN:
//    - Business rule'lar configuration ile parametrize edilmiş
//    - Runtime'da değiştirilebilir threshold'lar
//    - Flexible business logic

// HEXAGONAL ARCHITECTURE BENEFITS:
//
// - TESTABILITY: Mock interface'ler ile unit test yazılabilir
// - MAINTAINABILITY: Business logic infrastructure'dan izole
// - FLEXIBILITY: Farklı adapter'lar takılabilir
// - PORTABILITY: Platform/technology bağımsız domain logic