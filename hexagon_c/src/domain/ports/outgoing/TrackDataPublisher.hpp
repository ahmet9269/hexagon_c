#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include "../../model/FinalCalcDelayData.hpp"
#include <vector>
#include <memory>

namespace hat::domain::ports::outgoing {

/**
 * Secondary port: İşlenen verileri ZeroMQ üzerinden dışarı gönderen arayüz
 * Bu port domain logic tarafından çağrılır ve adapter'lar tarafından implement edilir
 */
class TrackDataPublisher {
public:
    virtual ~TrackDataPublisher() = default;

    /**
     * İşlenen DelayCalcTrackData'yı yayınlar
     * @param data Yayınlanacak veri
     * @return Yayınlama başarılı mı?
     */
    virtual bool publishDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * İşlenen FinalCalcDelayData'yı yayınlar
     * @param data Yayınlanacak final veri
     * @return Yayınlama başarılı mı?
     */
    virtual bool publishFinalCalcDelayData(const model::FinalCalcDelayData& data) = 0;



    /**
     * Publisher durumunu kontrol eder
     * @return Publisher aktif mi?
     */
    virtual bool isPublisherActive() const = 0;

    /**
     * Publisher istatistiklerini döner
     */
    struct PublisherStats {
        size_t total_published;
        size_t failed_publications;
        size_t active_connections;
        double average_latency_ms;
    };

    virtual PublisherStats getPublisherStats() const = 0;
};

} // namespace hat::domain::ports::outgoing