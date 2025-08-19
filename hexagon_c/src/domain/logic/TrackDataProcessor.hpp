#pragma once

#include "../ports/incoming/TrackDataSubmission.hpp"
#include "../ports/outgoing/TrackDataPublisher.hpp"
#include "../ports/outgoing/TrackDataRepository.hpp"
#include "../model/DelayCalcTrackData.hpp"
#include "../model/FinalCalcDelayData.hpp"
#include <memory>

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
        const ProcessorConfig& config = ProcessorConfig{});

    // TrackDataSubmission interface implementation
    
    /**
     * DelayCalcTrackData işleme - Ana iş mantığı
     */
    bool submitDelayCalcTrackData(const model::DelayCalcTrackData& data) override;

    /**
     * FinalCalcDelayData üretir ve gönderir
     * DelayCalcTrackData'dan hesaplanır
     */
    bool generateAndPublishFinalCalcDelayData(const model::DelayCalcTrackData& input_data);

    /**
     * Sistem hazır durumu kontrolü
     */
    bool isReadyToReceive() const override;

private:
    /**
     * Normal DelayCalcTrackData işleme
     */
    bool processNormalDelayCalcData(const model::DelayCalcTrackData& data);

    /**
     * Yüksek hız verisi işleme - Öncelikli
     */
    bool processHighVelocityData(const model::DelayCalcTrackData& data);

    /**
     * Normal FinalCalcDelayData işleme
     */
    bool processNormalFinalCalcData(const model::FinalCalcDelayData& data);

    /**
     * Kritik performans verisi işleme
     */
    bool processCriticalPerformanceData(const model::FinalCalcDelayData& data);

    /**
     * Zayıf performans verisi işleme
     */
    bool processPoorPerformanceData(const model::FinalCalcDelayData& data);

    /**
     * DelayCalcTrackData'dan FinalCalcDelayData üretir
     * GERÇEK GECİKME HESAPLAMALARI - User Specified Formulas
     */
    model::FinalCalcDelayData createFinalCalcDelayData(const model::DelayCalcTrackData& input_data);
};

} // namespace hat::domain::logic