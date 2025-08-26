// DRAFT API'ler CMake üzerinden tanımlanıyor; burada yeniden tanımlamaya gerek yok
#ifdef ZMQ_BUILD_DRAFT_API
#else
#define ZMQ_BUILD_DRAFT_API
#endif

#include "ZeroMQRadioTrackDataPublisher.hpp"
#include <iostream>

namespace hat::adapters::outgoing::zeromq {

ZeroMQRadioTrackDataPublisher::ZeroMQRadioTrackDataPublisher(
    const std::string& multicast_endpoint,
    const std::string& group_name)
    : running_(false)
    , multicast_endpoint_(multicast_endpoint)
    , group_name_(group_name)
    , zmq_context_(1)  // 1 I/O thread
    , radio_socket_(nullptr) {
    
    initializeRadioSocket();
}

ZeroMQRadioTrackDataPublisher::~ZeroMQRadioTrackDataPublisher() {
    stop();
    // C++ wrapper RAII ile otomatik cleanup yapar
}

void ZeroMQRadioTrackDataPublisher::initializeRadioSocket() {
    try {
        // PUB socket oluştur (C++ wrapper ile)
        radio_socket_ = std::make_unique<zmq::socket_t>(zmq_context_, ZMQ_PUB);

        std::cout << "[RadioPublisher] PUB socket oluşturuldu." << std::endl;
        std::cout << "[RadioPublisher] Multicast endpoint: " << multicast_endpoint_ << std::endl;
        std::cout << "[RadioPublisher] Grup adı: " << group_name_ << std::endl;

        // Multicast endpoint'e connect (C++ wrapper)
        radio_socket_->connect(multicast_endpoint_);
        
        std::cout << "[RadioPublisher] PUB socket başarıyla bağlandı." << std::endl;

    } catch (const zmq::error_t& e) {
        std::cerr << "[RadioPublisher] ZMQ Initialize hatası: " << e.what() << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] Initialize hatası: " << e.what() << std::endl;
        throw;
    }
}

bool ZeroMQRadioTrackDataPublisher::start() {
    if (running_.load()) {
        return false; // Zaten çalışıyor
    }

    running_.store(true);

    // Publisher worker thread'ini başlat
    publisher_thread_ = std::thread([this]() {
        publisherWorker();
    });

    std::cout << "[RadioPublisher] Publisher başlatıldı." << std::endl;
    return true;
}

void ZeroMQRadioTrackDataPublisher::stop() {
    running_.store(false);
    
    // Worker thread'i uyandır
    queue_cv_.notify_all();

    if (publisher_thread_.joinable()) {
        publisher_thread_.join();
    }

    std::cout << "[RadioPublisher] Publisher durduruldu." << std::endl;
}

bool ZeroMQRadioTrackDataPublisher::publishDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) {
    try {
        // Zaman damgası ile serialize et
        auto serialized = serializeDelayCalcTrackDataWithTimestamp(data);
        
        if (serialized.empty()) {
            return false;
        }

        // Task oluştur ve kuyruğa ekle
        RadioPublishTask task;
        task.type = RadioPublishTask::DELAY_CALC;
        task.serialized_data = std::move(serialized);
        task.timestamp = std::chrono::steady_clock::now();

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            publish_queue_.push(std::move(task));
        }

        // Worker thread'i uyandır
        queue_cv_.notify_one();
        
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] DelayCalcTrackData publish hatası: " << e.what() << std::endl;
        return false;
    }
}

bool ZeroMQRadioTrackDataPublisher::publishFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) {
    try {
        // Zaman damgası ile serialize et
        auto serialized = serializeFinalCalcDelayDataWithTimestamp(data);
        
        if (serialized.empty()) {
            return false;
        }

        // Task oluştur ve kuyruğa ekle
        RadioPublishTask task;
        task.type = RadioPublishTask::FINAL_CALC;
        task.serialized_data = std::move(serialized);
        task.timestamp = std::chrono::steady_clock::now();

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            publish_queue_.push(std::move(task));
        }

        // Worker thread'i uyandır
        queue_cv_.notify_one();
        
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] FinalCalcDelayData publish hatası: " << e.what() << std::endl;
        return false;
    }
}

bool ZeroMQRadioTrackDataPublisher::isPublisherActive() const {
    return running_.load();
}

void ZeroMQRadioTrackDataPublisher::publisherWorker() {
    std::cout << "[RadioPublisher] Worker thread başlatıldı." << std::endl;

    while (running_.load()) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        // Kuyrukta task bekle
        queue_cv_.wait(lock, [this] { 
            return !publish_queue_.empty() || !running_.load(); 
        });

        // Sistem durdurulmuşsa çık
        if (!running_.load()) {
            break;
        }

        // Task'ları işle
        while (!publish_queue_.empty()) {
            auto task = std::move(publish_queue_.front());
            publish_queue_.pop();
            
            lock.unlock(); // Publish sırasında mutex'i bırak
            
            publishRadioTask(task);
            
            lock.lock(); // Tekrar kilitle
        }
    }

    std::cout << "[RadioPublisher] Worker thread sonlandırıldı." << std::endl;
}

void ZeroMQRadioTrackDataPublisher::publishRadioTask(const RadioPublishTask& task) {
    try {
        // Draft API (zmq_msg_set_group) yerine topic prefiksi ekliyoruz
        // Basitçe mesajın başına grup adını ve ayırıcıyı ekleyelim
        std::string grouped_data = group_name_ + ":" + task.serialized_data;

        // C++ wrapper ile mesaj oluştur ve gönder
        zmq::message_t msg(grouped_data.data(), grouped_data.size());

        // C++ wrapper ile mesajı gönder
        auto result = radio_socket_->send(msg, zmq::send_flags::dontwait);
        
        if (result) {
            std::cout << "[RadioPublisher] Mesaj gönderildi - Grup: " << group_name_ 
                      << ", Boyut: " << result.value() << " byte" << std::endl;
        }

    } catch (const zmq::error_t& e) {
        std::cerr << "[RadioPublisher] ZMQ Publish task hatası: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] Publish task hatası: " << e.what() << std::endl;
    }
}

std::string ZeroMQRadioTrackDataPublisher::serializeDelayCalcTrackDataWithTimestamp(
    const domain::model::DelayCalcTrackData& data) {
    
    try {
        // app2_processor.cpp pattern'ini takip et - zaman damgası ekle
        auto forward_time = std::chrono::high_resolution_clock::now();
        auto forward_ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
            forward_time.time_since_epoch()).count();

        // Basit JSON-like serialization (gerçek uygulamada JSON library kullanılabilir)
        std::stringstream ss;
    ss << "{" 
       << "\"type\":\"DelayCalcTrackData\"," 
       << "\"track_id\":" << data.getTrackId() << "," 
       // Orijinal modelde x/y alanları yok; örnek amaçlı ECEF X/Y pozisyonları kullanılıyor
       << "\"x_pos_ecef\":" << data.getXPositionECEF() << "," 
       << "\"y_pos_ecef\":" << data.getYPositionECEF() << "," 
       << "\"timestamp\":" << forward_ns_since_epoch 
       << "}|" << forward_ns_since_epoch;  // app2_processor pattern - zaman damgası sonunda

        return ss.str();

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] DelayCalcTrackData serialization hatası: " << e.what() << std::endl;
        return "";
    }
}

std::string ZeroMQRadioTrackDataPublisher::serializeFinalCalcDelayDataWithTimestamp(
    const domain::model::FinalCalcDelayData& data) {
    
    try {
        // app2_processor.cpp pattern'ini takip et - zaman damgası ekle
        auto forward_time = std::chrono::high_resolution_clock::now();
        auto forward_ns_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
            forward_time.time_since_epoch()).count();

        // Basit JSON-like serialization
        std::stringstream ss;
    ss << "{" 
       << "\"type\":\"FinalCalcDelayData\"," 
       << "\"track_id\":" << data.getTrackId() << "," 
       // Modelde public total/processing latency alanları yok; mevcut getter'ları kullanıyoruz
       << "\"total_delay_time\":" << data.getTotalDelayTime() << "," 
       << "\"first_hop_delay\":" << data.getFirstHopDelayTime() << "," 
       << "\"second_hop_delay\":" << data.getSecondHopDelayTime() << "," 
       << "\"timestamp\":" << forward_ns_since_epoch 
       << "}|" << forward_ns_since_epoch;  // app2_processor pattern

        return ss.str();

    } catch (const std::exception& e) {
        std::cerr << "[RadioPublisher] FinalCalcDelayData serialization hatası: " << e.what() << std::endl;
        return "";
    }
}

} // namespace hat::adapters::outgoing::zeromq
