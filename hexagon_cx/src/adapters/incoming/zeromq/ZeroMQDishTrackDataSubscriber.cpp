// DRAFT API'leri etkinleştirmek için
#define ZMQ_BUILD_DRAFT_API

#include "ZeroMQDishTrackDataSubscriber.hpp"
#include <iostream>
#include <cassert>

namespace hat::adapters::incoming::zeromq {

// Hata kontrolü yardımcı fonksiyonu (app2_processor.cpp'den uyarlandı)
void ZeroMQDishTrackDataSubscriber::check_rc(int rc, const std::string& context_msg) {
    if (rc != 0) {
        std::cerr << context_msg << " - ZMQ Error #" << zmq_errno() << ": " << zmq_strerror(zmq_errno()) << std::endl;
        throw std::runtime_error("ZMQ Error: " + context_msg);
    }
}

ZeroMQDishTrackDataSubscriber::ZeroMQDishTrackDataSubscriber(
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
    const std::string& multicast_endpoint,
    const std::string& group_name)
    : track_data_submission_(track_data_submission)
    , running_(false)
    , multicast_endpoint_(multicast_endpoint)
    , group_name_(group_name)
    , zmq_context_(nullptr)
    , dish_socket_(nullptr) {
    
    initializeDishSocket();
}

ZeroMQDishTrackDataSubscriber::~ZeroMQDishTrackDataSubscriber() {
    stop();
    
    // Socket'leri temizle
    if (dish_socket_) {
        zmq_close(dish_socket_);
    }
    if (zmq_context_) {
        zmq_ctx_term(zmq_context_);
    }
}

void ZeroMQDishTrackDataSubscriber::initializeDishSocket() {
    try {
        // ZeroMQ context oluştur
        zmq_context_ = zmq_ctx_new();
        assert(zmq_context_);

        // DISH socket oluştur (app2_processor.cpp pattern'i)
        dish_socket_ = zmq_socket(zmq_context_, ZMQ_DISH);
        assert(dish_socket_);

        std::cout << "[DishSubscriber] Dish socket oluşturuldu." << std::endl;
        std::cout << "[DishSubscriber] Multicast endpoint: " << multicast_endpoint_ << std::endl;
        std::cout << "[DishSubscriber] Grup adı: " << group_name_ << std::endl;

        // Multicast endpoint'e bind
        check_rc(zmq_bind(dish_socket_, multicast_endpoint_.c_str()), "DISH zmq_bind");
        
        // Gruba katıl (app2_processor.cpp pattern'i)
        check_rc(zmq_join(dish_socket_, group_name_.c_str()), "DISH zmq_join");
        
        std::cout << "[DishSubscriber] Dish socket başarıyla bağlandı ve gruba katıldı." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] Initialize hatası: " << e.what() << std::endl;
        throw;
    }
}

bool ZeroMQDishTrackDataSubscriber::start() {
    if (running_.load()) {
        return false; // Zaten çalışıyor
    }

    running_.store(true);

    // Subscriber worker thread'ini başlat
    subscriber_thread_ = std::thread([this]() {
        subscriberWorker();
    });

    std::cout << "[DishSubscriber] Subscriber başlatıldı." << std::endl;
    return true;
}

void ZeroMQDishTrackDataSubscriber::stop() {
    running_.store(false);

    if (subscriber_thread_.joinable()) {
        subscriber_thread_.join();
    }

    std::cout << "[DishSubscriber] Subscriber durduruldu." << std::endl;
}

bool ZeroMQDishTrackDataSubscriber::isRunning() const {
    return running_.load();
}

void ZeroMQDishTrackDataSubscriber::subscriberWorker() {
    std::cout << "[DishSubscriber] Worker thread başlatıldı." << std::endl;
    std::cout << "[DishSubscriber] Mesajlar bekleniyor..." << std::endl;

    while (running_.load()) {
        try {
            // app2_processor.cpp pattern'ini takip et - mesaj alma
            zmq_msg_t received_msg;
            zmq_msg_init(&received_msg);
            
            // Non-blocking receive (timeout ile)
            int bytes = zmq_msg_recv(&received_msg, dish_socket_, ZMQ_DONTWAIT);
            
            if (bytes == -1) {
                int error = zmq_errno();
                if (error == EAGAIN) {
                    // Mesaj yok, kısa bekle
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    zmq_msg_close(&received_msg);
                    continue;
                } else {
                    std::cerr << "[DishSubscriber] Receive hatası: " << zmq_strerror(error) << std::endl;
                    zmq_msg_close(&received_msg);
                    continue;
                }
            }

            // Mesajı string'e çevir
            std::string received_payload(
                static_cast<char*>(zmq_msg_data(&received_msg)), 
                zmq_msg_size(&received_msg)
            );
            zmq_msg_close(&received_msg);

            // app2_processor.cpp pattern'i - gecikme hesaplama
            auto latency_info = processReceivedMessage(received_payload);

            // Veriyi deserialize et
            auto track_data = deserializeDelayCalcTrackData(latency_info.original_data, latency_info);

            if (track_data.has_value() && track_data_submission_) {
                // Domain katmanına gönder
                track_data_submission_->submitDelayCalcTrackData(track_data.value());
                
                std::cout << "[DishSubscriber] Veri domain katmanına gönderildi - Track ID: " 
                          << track_data->track_id << ", Gecikme: " 
                          << latency_info.latency_ns / 1000.0 << " mikrosaniye" << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "[DishSubscriber] Worker thread hatası: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::cout << "[DishSubscriber] Worker thread sonlandırıldı." << std::endl;
}

ZeroMQDishTrackDataSubscriber::LatencyMeasurement 
ZeroMQDishTrackDataSubscriber::processReceivedMessage(const std::string& received_payload) {
    
    LatencyMeasurement result;
    
    // app2_processor.cpp pattern'ini takip et
    // 1. Mesajı alır almaz mevcut zamanı kaydet
    result.receive_time = std::chrono::high_resolution_clock::now();

    // 2. Mesajı '|' karakterinden ayırarak orijinal metni ve gönderim zamanını bul
    size_t separator_pos = received_payload.find_last_of('|');
    if (separator_pos == std::string::npos) {
        std::cerr << "[DishSubscriber] Hata: Alınan mesajda zaman damgası ayıracı ('|') bulunamadı." << std::endl;
        result.original_data = received_payload;
        result.send_timestamp_ns = 0;
        result.latency_ns = 0;
        return result;
    }

    result.original_data = received_payload.substr(0, separator_pos);
    try {
        result.send_timestamp_ns = std::stoll(received_payload.substr(separator_pos + 1));
    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] Zaman damgası parse hatası: " << e.what() << std::endl;
        result.send_timestamp_ns = 0;
        result.latency_ns = 0;
        return result;
    }

    // 3. Gecikmeyi hesapla
    auto receive_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        result.receive_time.time_since_epoch()).count();
    result.latency_ns = receive_time_ns - result.send_timestamp_ns;

    std::cout << "[DishSubscriber] -> Mesaj alındı: \"" << result.original_data << "\"" << std::endl;
    std::cout << "[DishSubscriber]    Gecikme: " << result.latency_ns / 1000.0 << " mikrosaniye" << std::endl;

    return result;
}

std::optional<domain::model::DelayCalcTrackData> 
ZeroMQDishTrackDataSubscriber::deserializeDelayCalcTrackData(
    const std::string& original_data, 
    const LatencyMeasurement& latency_info) {
    
    try {
        // Basit JSON-like parsing (gerçek uygulamada JSON library kullanılabilir)
        // Format: {"type":"DelayCalcTrackData","track_id":123,"x":1.5,"y":2.5,"timestamp":...}
        
        domain::model::DelayCalcTrackData data;
        
        // track_id parse et
        size_t track_id_pos = original_data.find("\"track_id\":");
        if (track_id_pos != std::string::npos) {
            size_t start = track_id_pos + 11; // "track_id": sonrası
            size_t end = original_data.find(",", start);
            if (end == std::string::npos) end = original_data.find("}", start);
            
            if (end != std::string::npos) {
                std::string track_id_str = original_data.substr(start, end - start);
                data.track_id = std::stoi(track_id_str);
            }
        }
        
        // x koordinatını parse et
        size_t x_pos = original_data.find("\"x\":");
        if (x_pos != std::string::npos) {
            size_t start = x_pos + 4; // "x": sonrası
            size_t end = original_data.find(",", start);
            if (end == std::string::npos) end = original_data.find("}", start);
            
            if (end != std::string::npos) {
                std::string x_str = original_data.substr(start, end - start);
                data.x = std::stod(x_str);
            }
        }
        
        // y koordinatını parse et
        size_t y_pos = original_data.find("\"y\":");
        if (y_pos != std::string::npos) {
            size_t start = y_pos + 4; // "y": sonrası
            size_t end = original_data.find(",", start);
            if (end == std::string::npos) end = original_data.find("}", start);
            
            if (end != std::string::npos) {
                std::string y_str = original_data.substr(start, end - start);
                data.y = std::stod(y_str);
            }
        }

        // Gecikme bilgisini ekle (domain model'inde varsa)
        // data.receive_latency_ns = latency_info.latency_ns; // Eğer model bunu destekliyorsa

        return data;

    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] Deserialization hatası: " << e.what() << std::endl;
        return std::nullopt;
    }
}

} // namespace hat::adapters::incoming::zeromq
