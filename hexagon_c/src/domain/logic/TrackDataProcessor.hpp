#pragma once

#include "../ports/incoming/TrackDataSubmission.hpp"
#include "../ports/outgoing/TrackDataPublisher.hpp"
#include "../ports/outgoing/TrackDataRepository.hpp"
#include "../model/DelayCalcTrackData.hpp"
#include "../model/FinalCalcDelayData.hpp"
#include <memory>
#include <chrono>

namespace hat::domain::logic {

// İş mantığı konfigürasyonu
struct ProcessorConfig {
    int64_t max_acceptable_delay_ms = 5000;
    double high_velocity_threshold = 100.0;
};

/**
 * Core business logic - Hexagon'un kalbi
 * Track data işleme, gecikme analizi ve veri akışı yönetimi
 * Sadece domain modelleri ve port'lara bağımlı, teknik detaylardan bağımsız
 */
class TrackDataProcessor : public ports::incoming::TrackDataSubmission {
private:
    std::shared_ptr<ports::outgoing::TrackDataPublisher> publisher_;
    std::shared_ptr<ports::outgoing::TrackDataRepository> repository_;
    
    ProcessorConfig config_;

public:
    TrackDataProcessor(
        std::shared_ptr<ports::outgoing::TrackDataPublisher> publisher,
        std::shared_ptr<ports::outgoing::TrackDataRepository> repository,
        const ProcessorConfig& config = ProcessorConfig{})
        : publisher_(publisher)
        , repository_(repository)
        , config_(config) {}

    // TrackDataSubmission interface implementation
    
    /**
     * DelayCalcTrackData işleme - Ana iş mantığı
     */
    bool submitDelayCalcTrackData(const model::DelayCalcTrackData& data) override {
        try {
            // İş kuralı: Veri validasyonu
            if (!data.isValid()) {
                return false;
            }

            // İş kuralı: Veri yaşı kontrolü
            if (!data.isDataFresh(config_.max_acceptable_delay_ms)) {
                // Eski veri uyarısı - yine de işle ama logla
                // Gerçek uygulamada logging yapılacak
            }

            // İş kuralı: Yüksek hız kontrolü
            if (data.isHighVelocity(config_.high_velocity_threshold)) {
                // Yüksek hız uyarısı - öncelikli işlem
                return processHighVelocityData(data);
            }

            // Normal işlem akışı
            return processNormalDelayCalcData(data);

        } catch (const std::exception& e) {
            // Hata yönetimi
            return false;
        }
    }

    /**
     * FinalCalcDelayData üretir ve gönderir
     * DelayCalcTrackData'dan hesaplanır
     */
    bool generateAndPublishFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
        try {
            // FinalCalcDelayData'yı DelayCalcTrackData'dan üret
            auto final_data = createFinalCalcDelayData(input_data);
            
            if (!final_data.isValid()) {
                return false;
            }

            // İş kuralı: Performans analizi
            auto performance_status = final_data.getPerformanceStatus();
            
            switch (performance_status) {
                case model::FinalCalcDelayData::PerformanceStatus::CRITICAL:
                    return processCriticalPerformanceData(final_data);
                
                case model::FinalCalcDelayData::PerformanceStatus::POOR:
                    return processPoorPerformanceData(final_data);
                
                default:
                    return processNormalFinalCalcData(final_data);
            }

        } catch (const std::exception& e) {
            return false;
        }
    }



    /**
     * Sistem hazır durumu kontrolü
     */
    bool isReadyToReceive() const override {
        return publisher_ && publisher_->isPublisherActive() && 
               repository_;
    }

private:
    /**
     * Normal DelayCalcTrackData işleme
     */
    bool processNormalDelayCalcData(const model::DelayCalcTrackData& data) {
        // 1. Veriyi sakla
        if (!repository_->saveDelayCalcTrackData(data)) {
            return false;
        }



        // 3. FinalCalcDelayData üret ve gönder
        if (!generateAndPublishFinalCalcDelayData(data)) {
            return false;
        }

        // 4. Orijinal DelayCalcTrackData'yı da yayınla (opsiyonel)
        return publisher_->publishDelayCalcTrackData(data);
    }

    /**
     * Yüksek hız verisi işleme - Öncelikli
     */
    bool processHighVelocityData(const model::DelayCalcTrackData& data) {
        // Yüksek hızlı veriler için özel işlem
        
        // 1. Öncelikli saklama
        if (!repository_->saveDelayCalcTrackData(data)) {
            return false;
        }

        // 2. Acil yayınlama
        if (!publisher_->publishDelayCalcTrackData(data)) {
            return false;
        }


        
        return true;
    }

    /**
     * Normal FinalCalcDelayData işleme
     */
    bool processNormalFinalCalcData(const model::FinalCalcDelayData& data) {
        // 1. Final veriyi sakla
        if (!repository_->saveFinalCalcDelayData(data)) {
            return false;
        }

        // 2. Performans metrikleri hesapla
        auto metrics = data.calculatePerformanceMetrics();

        // 3. Final veriyi yayınla
        return publisher_->publishFinalCalcDelayData(data);
    }

    /**
     * Kritik performans verisi işleme
     */
    bool processCriticalPerformanceData(const model::FinalCalcDelayData& data) {
        // Kritik durum - acil müdahale
        
        // 1. Acil saklama
        repository_->saveFinalCalcDelayData(data);

        // 2. Acil yayınlama
        publisher_->publishFinalCalcDelayData(data);



        return true;
    }

    /**
     * Zayıf performans verisi işleme
     */
    bool processPoorPerformanceData(const model::FinalCalcDelayData& data) {
        // Performans düşüklüğü - izleme artır
        
        repository_->saveFinalCalcDelayData(data);
        publisher_->publishFinalCalcDelayData(data);



        return true;
    }

    /**
     * DelayCalcTrackData'dan FinalCalcDelayData üretir
     * GERÇEK GECİKME HESAPLAMALARI - User Specified Formulas
     */
    model::FinalCalcDelayData createFinalCalcDelayData(const model::DelayCalcTrackData& input_data) {
        // KRITIK: Alım zamanını al (hexagon_c'de işleme zamanı)
        auto receive_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // GERÇEK GECİKME HESAPLAMALARI:
        // hexagon_b'den gelen gerçek gönderim zamanı
        int64_t sent_time = input_data.getFirstHopSentTime();
        
        // User Specified Formulas:
        // SecondHopDelayTime = ReceivedTime - FirstHopSentTime
        int64_t second_hop_delay_time = receive_time - sent_time;
        
        // TotalDelayTime = ReceivedTime - FirstHopSentTime  
        int64_t total_delay_time = receive_time - sent_time;
        
        // ThirdHopSentTime = CurrentTime (işleme zamanı)
        int64_t third_hop_sent_time = receive_time;
        
        // FinalCalcDelayData oluştur
        model::FinalCalcDelayData final_data(
            input_data.getTrackId(),                    // track_id
            input_data.getXVelocityECEF(),             // x_vel_ecef
            input_data.getYVelocityECEF(),             // y_vel_ecef  
            input_data.getZVelocityECEF(),             // z_vel_ecef
            input_data.getXPositionECEF(),             // x_pos_ecef
            input_data.getYPositionECEF(),             // y_pos_ecef
            input_data.getZPositionECEF(),             // z_pos_ecef
            input_data.getUpdateTime(),                 // update_time
            input_data.getOriginalUpdateTime(),         // original_update_time
            input_data.getFirstHopSentTime(),          // first_hop_sent
            input_data.getFirstHopDelayTime(),         // first_hop_delay
            input_data.getSecondHopSentTime(),         // second_hop_sent
            second_hop_delay_time,                      // second_hop_delay (HESAPLANAN)
            total_delay_time,                           // total_delay (HESAPLANAN)
            third_hop_sent_time                         // third_hop_sent (HESAPLANAN)
        );
        
        // GERÇEK GECİKME CONSOLE ÇIKTISI
        std::cout << "📨 Received Track " << input_data.getTrackId() 
                  << " - 📤 Sent: " << sent_time << " ms"
                  << " - 📨 Received: " << receive_time << " ms"
                  << " - ⏱️  DELAY: " << total_delay_time << " ms";
        
        // Gecikme kategorisi
        static constexpr int64_t EXCELLENT_THRESHOLD = 10;
        static constexpr int64_t GOOD_THRESHOLD = 50;
        static constexpr int64_t ACCEPTABLE_THRESHOLD = 100;
        static constexpr int64_t POOR_THRESHOLD = 500;
        
        if (total_delay_time < EXCELLENT_THRESHOLD) std::cout << " 🟢 EXCELLENT";
        else if (total_delay_time < GOOD_THRESHOLD) std::cout << " 🟢 GOOD";
        else if (total_delay_time < ACCEPTABLE_THRESHOLD) std::cout << " 🟡 ACCEPTABLE";
        else if (total_delay_time < POOR_THRESHOLD) std::cout << " 🟠 POOR";
        else std::cout << " 🔴 CRITICAL";
        std::cout << std::endl;
        
        return final_data;
    }


};

} // namespace hat::domain::logic