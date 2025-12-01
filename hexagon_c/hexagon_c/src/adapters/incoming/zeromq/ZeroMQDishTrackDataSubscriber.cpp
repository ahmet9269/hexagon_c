#include "ZeroMQDishTrackDataSubscriber.hpp"
#include <iostream>
#include <zmq.hpp> // C++ wrapper için

namespace adapters {
namespace incoming {
namespace zeromq {

// Default constructor with standard configuration
ZeroMQDishTrackDataSubscriber::ZeroMQDishTrackDataSubscriber(
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission)
    : track_data_submission_(track_data_submission)
    , running_(false)
    , multicast_endpoint_("udp://239.1.1.5:9595")  // Port 9595 for DelayCalcTrackData from B_hexagon (updated to match DelayCalcTrackData constants)
    , group_name_("DelayCalcTrackData")             // Group name matches message type
    , zmq_context_(1)  // 1 I/O thread
    , dish_socket_(nullptr) {
    
    initializeDishSocket();
}

// Custom configuration constructor
ZeroMQDishTrackDataSubscriber::ZeroMQDishTrackDataSubscriber(
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
    const std::string& multicast_endpoint,
    const std::string& group_name)
    : track_data_submission_(track_data_submission)
    , running_(false)
    , multicast_endpoint_(multicast_endpoint)
    , group_name_(group_name)
    , zmq_context_(1)  // 1 I/O thread
    , dish_socket_(nullptr) {
    
    initializeDishSocket();
}

ZeroMQDishTrackDataSubscriber::~ZeroMQDishTrackDataSubscriber() {
    stop();
    // C++ wrapper RAII ile otomatik cleanup yapar
}

void ZeroMQDishTrackDataSubscriber::initializeDishSocket() {
    try {
        std::cout << "🔧 ZeroMQ DISH Configuration:" << std::endl;
        std::cout << "   📡 Endpoint: " << multicast_endpoint_ << std::endl;
        std::cout << "   👥 Group: " << group_name_ << std::endl;
        
        // DISH socket oluştur (C++ wrapper ile) - Draft API gerekli
        dish_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, zmq::socket_type::dish);

        // Performance optimizations
        dish_socket_->set(zmq::sockopt::rcvhwm, 0);       // Unlimited receive buffer
        dish_socket_->set(zmq::sockopt::rcvtimeo, 100);   // 100ms timeout for graceful shutdown
        dish_socket_->set(zmq::sockopt::linger, 0);       // No linger on close
        dish_socket_->set(zmq::sockopt::immediate, 1);    // Process messages immediately
        
        // UDP multicast için DISH socket bind yapar
        dish_socket_->bind(multicast_endpoint_);
        
        // Gruba join ol (DISH için)
        dish_socket_->join(group_name_.c_str());

    } catch (const zmq::error_t& e) {
        std::cerr << "[DishSubscriber] ZMQ Initialize hatası: " << e.what() << std::endl;
        throw;
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
        // Real-time thread priority ayarla
        #ifdef __linux__
        struct sched_param param;
        param.sched_priority = 95; // Max priority
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        
        // CPU affinity - dedicated core
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(1, &cpuset); // Core 1'e bind et
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        #endif
        
        subscriberWorker();
    });

    return true;
}

void ZeroMQDishTrackDataSubscriber::stop() {
    running_.store(false);

    if (subscriber_thread_.joinable()) {
        subscriber_thread_.join();
    }
}

bool ZeroMQDishTrackDataSubscriber::isRunning() const {
    return running_.load();
}

void ZeroMQDishTrackDataSubscriber::subscriberWorker() {
    while (running_.load()) {
        try {
            // ZeroMQ mesaj alma
            zmq::message_t received_msg;
            
            // Non-blocking receive with timeout for graceful shutdown
            auto result = dish_socket_->recv(received_msg, zmq::recv_flags::dontwait);
            
            if (!result || received_msg.size() == 0) {
                // Mesaj yok, kısa bekle
                std::this_thread::sleep_for(std::chrono::microseconds(10));
                continue;
            }

            // Mesaj alındığı anı kaydet (gecikme hesaplaması için)
            auto receive_time = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();

            // Binary mesajı vector'e dönüştür
            const uint8_t* msg_data = static_cast<const uint8_t*>(received_msg.data());
            std::vector<uint8_t> binary_data(msg_data, msg_data + received_msg.size());

            // DelayCalcTrackData'yı deserialize et
            domain::model::DelayCalcTrackData track_data;
            if (track_data.deserialize(binary_data)) {
                if (track_data.isValid() && track_data_submission_) {
                    // Gecikme hesapla: şu anki zaman - ikinci hop gönderim zamanı
                    long second_hop_latency_us = receive_time - track_data.getSecondHopSentTime();
                    
                    std::cout << "📡 DelayCalcTrackData alındı - Track ID: " << track_data.getTrackId() << std::endl;
                    std::cout << "   🕐 İkinci Hop Gecikme: " << second_hop_latency_us << " μs" << std::endl;
                    std::cout << "   🕐 Birinci Hop Gecikme: " << track_data.getFirstHopDelayTime() << " μs" << std::endl;
                    std::cout << "   🕐 Toplam ZMQ Gecikme: " << (track_data.getFirstHopDelayTime() + second_hop_latency_us) << " μs" << std::endl;
                    
                    // Domain katmanına gönder (Hexagonal Architecture)
                    track_data_submission_->submitDelayCalcTrackData(track_data);
                } else {
                    std::cerr << "[DishSubscriber] ❌ Invalid DelayCalcTrackData received" << std::endl;
                }
            } else {
                std::cerr << "[DishSubscriber] ❌ Failed to deserialize DelayCalcTrackData" << std::endl;
                std::cerr << "[DishSubscriber] Message size: " << received_msg.size() << " bytes" << std::endl;
            }

        } catch (const zmq::error_t& e) {
            if (e.num() != EAGAIN) {  // EAGAIN = mesaj yok, normal durum
                std::cerr << "[DishSubscriber] ZMQ Worker thread hatası: " << e.what() << std::endl;
            }
            // Hata durumunda çok kısa bekle
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        } catch (const std::exception& e) {
            std::cerr << "[DishSubscriber] Worker thread hatası: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

// Note: processReceivedMessage is not needed anymore as we handle binary data directly
// Kept for potential future use with string-based messages

std::optional<domain::model::DelayCalcTrackData> 
ZeroMQDishTrackDataSubscriber::deserializeDelayCalcTrackData(
    const std::vector<uint8_t>& binary_data) {
    
    try {
        // DelayCalcTrackData modeli oluştur
        domain::model::DelayCalcTrackData data;
        
        // DelayCalcTrackData'nın kendi binary deserialize metodunu kullan
        if (data.deserialize(binary_data)) {
            // Deserializasyon başarılı - validation kontrolü
            if (data.isValid()) {
                std::cout << "[DishSubscriber] ✅ DelayCalcTrackData successfully deserialized and validated" << std::endl;
                std::cout << "[DishSubscriber] Track ID: " << data.getTrackId() 
                         << ", UpdateTime: " << data.getUpdateTime() << " μs" << std::endl;
                return data;
            } else {
                std::cerr << "[DishSubscriber] ❌ DelayCalcTrackData validation failed after deserialization" << std::endl;
                return std::nullopt;
            }
        } else {
            std::cerr << "[DishSubscriber] ❌ DelayCalcTrackData binary deserialization failed" << std::endl;
            std::cerr << "[DishSubscriber] Expected size: " << data.getSerializedSize() 
                     << " bytes, Received: " << binary_data.size() << " bytes" << std::endl;
            return std::nullopt;
        }

    } catch (const std::exception& e) {
        std::cerr << "[DishSubscriber] ❌ Exception during DelayCalcTrackData deserialization: " << e.what() << std::endl;
        std::cerr << "[DishSubscriber] Binary data size: " << binary_data.size() << " bytes" << std::endl;
        return std::nullopt;
    }
}

} // namespace zeromq
} // namespace incoming
} // namespace adapters
