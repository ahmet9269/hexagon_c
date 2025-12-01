#pragma once

#include "domain/ports/incoming/IDelayCalcTrackDataIncomingPort.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <sstream>
#include <optional>
#include <cstring>
#include <chrono>

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;

namespace adapters {
namespace incoming {
namespace zeromq {

/**
 * Dish pattern kullanan ZeroMQ Subscriber Adapter
 * UDP multicast üzerinden grup tabanlı mesaj alma sağlar
 * app2_processor.cpp'deki dish pattern'ini hexagonal architecture'a entegre eder
 */
class TrackDataZeroMQIncomingAdapter {
private:
    std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission_;
    
    // ZeroMQ C++ context ve socket
    zmq::context_t zmq_context_;
    std::unique_ptr<zmq::socket_t> dish_socket_;
    
    // Thread yönetimi
    std::thread subscriber_thread_;
    std::atomic<bool> running_;
    
    // Konfigürasyon
    std::string multicast_endpoint_;  // UDP multicast adresi (örn: udp://239.1.1.1:9001)
    std::string group_name_;          // Dinlenecek grup adı (örn: "SOURCE_DATA")
    
    // Gecikme hesaplama için
    struct LatencyMeasurement {
        std::chrono::steady_clock::time_point receive_time;
        long long send_timestamp_us;  // mikrosaniye
        long long latency_us;         // mikrosaniye
        std::string original_data;
    };
    
    // Hata kontrolü için yardımcı fonksiyon (C++ wrapper ile artık gereksiz)
    // void check_rc(int rc, const std::string& context_msg);

public:
    /**
     * Constructor - Default UDP multicast configuration
     * @param track_data_submission Domain katmanına veri göndermek için port
     */
    TrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission);

    /**
     * Constructor with custom configuration
     * @param track_data_submission Domain katmanına veri göndermek için port
     * @param multicast_endpoint UDP multicast endpoint (örn: "udp://239.1.1.1:9001")
     * @param group_name Dinlenecek multicast grup adı (örn: "SOURCE_DATA")
     */
    TrackDataZeroMQIncomingAdapter(
        std::shared_ptr<domain::ports::incoming::IDelayCalcTrackDataIncomingPort> track_data_submission,
        const std::string& multicast_endpoint,
        const std::string& group_name);

    ~TrackDataZeroMQIncomingAdapter();

    /**
     * Dish Subscriber'ı başlatır
     */
  [[nodiscard]]  bool start();

    /**
     * Dish Subscriber'ı durdurur
     */
    void stop();

    /**
     * Subscriber aktif durumu
     */
   [[nodiscard]] bool isRunning() const;

private:
    /**
     * ZeroMQ dish socket'ini initialize eder
     */
    void initializeDishSocket();

    /**
     * Subscriber worker thread - asynchronous message receiving
     */
    void subscriberWorker();

    // Note: processReceivedMessage is not needed for binary DelayCalcTrackData
    // LatencyMeasurement processReceivedMessage(const std::string& received_payload);

    /**
     * Binary veriyi DelayCalcTrackData'ya deserialize eder
     */
    std::optional<domain::ports::DelayCalcTrackData> deserializeDelayCalcTrackData(
        const std::vector<uint8_t>& binary_data);
};

} // namespace zeromq
} // namespace incoming
} // namespace adapters
