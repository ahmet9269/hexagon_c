#pragma once

#include "../../../domain/ports/incoming/TrackDataSubmission.hpp"
#include "../../../domain/model/DelayCalcTrackData.hpp"
#include "../../../domain/model/FinalCalcDelayData.hpp"
#include "zmq/zmq.hpp"
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <functional>
#include <sstream>
#include <locale>
#include <optional>

namespace hat::adapters::incoming::zeromq {

/**
 * Primary adapter: ZeroMQ üzerinden veri alan ve domain'e ileten adapter
 * Hexagonal mimaride dış dünyadan sisteme gelen veriler için giriş noktası
 */
class ZeroMQTrackDataSubscriber {
private:
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission_;
    
    // ZeroMQ context ve socket
    zmq::context_t context_;
    std::unique_ptr<zmq::socket_t> delay_calc_subscriber_;
    
    // Thread yönetimi
    std::thread delay_calc_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string delay_calc_endpoint_;
    std::string topic_filter_;
    
    // İstatistikler
    std::atomic<size_t> total_received_;
    std::atomic<size_t> successful_processed_;
    std::atomic<size_t> failed_processed_;

public:
    /**
     * Constructor
     * @param track_data_submission Domain katmanına veri gönderecek port
     * @param delay_calc_endpoint DelayCalcTrackData için ZeroMQ endpoint
     * @param topic_filter Topic filtresi (opsiyonel)
     */
    ZeroMQTrackDataSubscriber(
        std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
        const std::string& delay_calc_endpoint = "tcp://localhost:5555",
        const std::string& topic_filter = "");

    ~ZeroMQTrackDataSubscriber();

    /**
     * Subscriber'ı başlatır
     */
    bool start();

    /**
     * Subscriber'ı durdurur
     */
    void stop();

    /**
     * Subscriber durumunu kontrol eder
     */
    bool isRunning() const;

    /**
     * İstatistikleri döner
     */
    struct SubscriberStats {
        size_t total_received;
        size_t successful_processed;
        size_t failed_processed;
        double success_rate;
    };

    SubscriberStats getStats() const;

private:
    /**
     * ZeroMQ subscriber'ı initialize eder
     */
    void initializeSubscribers();

    /**
     * DelayCalcTrackData mesajlarını dinler
     */
    void subscribeDelayCalcTrackData();

    /**
     * ZeroMQ mesajını DelayCalcTrackData'ya deserialize eder
     */
    std::optional<domain::model::DelayCalcTrackData> deserializeDelayCalcTrackData(const zmq::message_t& message);
};

} // namespace hat::adapters::incoming::zeromq