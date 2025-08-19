#pragma once

#include "../../model/DelayCalcTrackData.hpp"
#include "../../model/FinalCalcDelayData.hpp"
#include <vector>
#include <optional>
#include <memory>

namespace hat::domain::ports::outgoing {

/**
 * Secondary port: Track verilerini kalıcı olarak saklayan arayüz
 * Bu port domain logic tarafından çağrılır ve persistence adapter'ı tarafından implement edilir
 */
class TrackDataRepository {
public:
    virtual ~TrackDataRepository() = default;

    // DelayCalcTrackData operations
    
    /**
     * DelayCalcTrackData'yı saklar
     * @param data Saklanacak veri
     * @return Saklama başarılı mı?
     */
    virtual bool saveDelayCalcTrackData(const model::DelayCalcTrackData& data) = 0;

    /**
     * ID ile DelayCalcTrackData bulur
     * @param id Aranacak ID
     * @return Bulunan veri (varsa)
     */
    virtual std::optional<model::DelayCalcTrackData> findDelayCalcTrackDataById(int id) = 0;

    /**
     * Track ID ile DelayCalcTrackData bulur
     * @param track_id Aranacak track ID
     * @return Bulunan veri listesi
     */
    virtual std::vector<model::DelayCalcTrackData> findDelayCalcTrackDataByTrackId(int track_id) = 0;

    // FinalCalcDelayData operations

    /**
     * FinalCalcDelayData'yı saklar
     * @param data Saklanacak final veri
     * @return Saklama başarılı mı?
     */
    virtual bool saveFinalCalcDelayData(const model::FinalCalcDelayData& data) = 0;

    /**
     * Repository istatistiklerini döner
     */
    struct RepositoryStats {
        size_t total_delay_calc_records;
        size_t total_final_calc_records;
        size_t active_tracks;
        int64_t oldest_record_time;
        int64_t newest_record_time;
    };

    virtual RepositoryStats getRepositoryStats() = 0;
};

} // namespace hat::domain::ports::outgoing