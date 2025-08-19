#include "ZeroMQTrackDataPublisher.hpp"
#include <iostream>
#include <cstring>

namespace hat::adapters::outgoing::zeromq {

ZeroMQTrackDataPublisher::ZeroMQTrackDataPublisher(
    const std::string& delay_calc_endpoint,
    const std::string& final_calc_endpoint,
    const std::string& topic_prefix)
    : context_(1)
    , running_(false)
    , delay_calc_endpoint_(delay_calc_endpoint)
    , final_calc_endpoint_(final_calc_endpoint)
    , topic_prefix_(topic_prefix)
    , total_published_(0)
    , failed_publications_(0)
    , active_connections_(0)
    , total_latency_ms_(0.0) {
    
    initializePublishers();
}

ZeroMQTrackDataPublisher::~ZeroMQTrackDataPublisher() {
    stop();
}

bool ZeroMQTrackDataPublisher::start() {
    if (running_.load()) {
        return false; // Zaten çalışıyor
    }

    running_.store(true);

    // Publisher thread'ini başlat
    publisher_thread_ = std::thread([this]() {
        publisherWorker();
    });

    return true;
}

void ZeroMQTrackDataPublisher::stop() {
    running_.store(false);
    queue_cv_.notify_all();

    if (publisher_thread_.joinable()) {
        publisher_thread_.join();
    }
}

bool ZeroMQTrackDataPublisher::publishDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) {
    try {
        auto serialized = serializeDelayCalcTrackData(data);
        if (serialized.empty()) {
            failed_publications_++;
            return false;
        }

        PublishTask task;
        task.type = PublishTask::DELAY_CALC;
        task.serialized_data = std::move(serialized);
        task.track_id = data.getTrackId();
        task.timestamp = std::chrono::steady_clock::now();

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            publish_queue_.push(std::move(task));
        }
        queue_cv_.notify_one();

        return true;

    } catch (const std::exception& e) {
        failed_publications_++;
        return false;
    }
}

bool ZeroMQTrackDataPublisher::publishFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) {
    try {
        auto serialized = serializeFinalCalcDelayData(data);
        if (serialized.empty()) {
            failed_publications_++;
            return false;
        }

        PublishTask task;
        task.type = PublishTask::FINAL_CALC;
        task.serialized_data = std::move(serialized);
        task.track_id = data.getTrackId();
        task.timestamp = std::chrono::steady_clock::now();

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            publish_queue_.push(std::move(task));
        }
        queue_cv_.notify_one();

        return true;

    } catch (const std::exception& e) {
        failed_publications_++;
        return false;
    }
}

bool ZeroMQTrackDataPublisher::isPublisherActive() const {
    return running_.load();
}

ZeroMQTrackDataPublisher::PublisherStats ZeroMQTrackDataPublisher::getPublisherStats() const {
    PublisherStats stats;
    stats.total_published = total_published_.load();
    stats.failed_publications = failed_publications_.load();
    stats.active_connections = active_connections_.load();
    
    auto total = total_published_.load();
    stats.average_latency_ms = total > 0 ? total_latency_ms_.load() / total : 0.0;
    
    return stats;
}

void ZeroMQTrackDataPublisher::initializePublishers() {
    // DelayCalcTrackData publisher
    delay_calc_publisher_ = std::make_unique<zmq::socket_t>(context_, ZMQ_PUB);
    delay_calc_publisher_->bind(delay_calc_endpoint_);

    // FinalCalcDelayData publisher
    final_calc_publisher_ = std::make_unique<zmq::socket_t>(context_, ZMQ_PUB);
    final_calc_publisher_->bind(final_calc_endpoint_);

    // High Water Mark ayarla (memory kontrolü için)
    int hwm = 10000;
    delay_calc_publisher_->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
    final_calc_publisher_->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
}

void ZeroMQTrackDataPublisher::publisherWorker() {
    while (running_.load()) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        // Queue'da task bekle
        queue_cv_.wait(lock, [this]() {
            return !publish_queue_.empty() || !running_.load();
        });

        if (!running_.load()) {
            break;
        }

        // Task'ları işle
        while (!publish_queue_.empty()) {
            auto task = std::move(publish_queue_.front());
            publish_queue_.pop();
            lock.unlock();

            // Task'ı yayınla
            publishTask(task);

            lock.lock();
        }
    }
}

void ZeroMQTrackDataPublisher::publishTask(const PublishTask& task) {
    try {
        auto start_time = std::chrono::steady_clock::now();

        // Topic oluştur
        std::string topic = topic_prefix_ + "." + std::to_string(task.track_id);

        zmq::socket_t* socket = nullptr;
        if (task.type == PublishTask::DELAY_CALC) {
            socket = delay_calc_publisher_.get();
        } else {
            socket = final_calc_publisher_.get();
        }

        // Topic mesajını gönder
        zmq::message_t topic_msg(topic.size());
        memcpy(topic_msg.data(), topic.c_str(), topic.size());
        socket->send(topic_msg, zmq::send_flags::sndmore);

        // Data mesajını gönder
        zmq::message_t data_msg(task.serialized_data.size());
        memcpy(data_msg.data(), task.serialized_data.data(), task.serialized_data.size());
        socket->send(data_msg, zmq::send_flags::dontwait);

        // İstatistikleri güncelle
        total_published_++;
        
        auto end_time = std::chrono::steady_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
        total_latency_ms_ = total_latency_ms_.load() + latency;

    } catch (const std::exception& e) {
        failed_publications_++;
    }
}

std::vector<uint8_t> ZeroMQTrackDataPublisher::serializeDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) {
    try {
        std::ostringstream oss;
        oss.imbue(std::locale::classic());
        oss 
            << data.getId() << '|'
            << data.getVelocity() << '|'
            << data.getPosition() << '|'
            << data.getTimeField() << '|'
            << data.getTrackId() << '|'
            << data.getXVelocityECEF() << '|'
            << data.getYVelocityECEF() << '|'
            << data.getZVelocityECEF() << '|'
            << data.getXPositionECEF() << '|'
            << data.getYPositionECEF() << '|'
            << data.getZPositionECEF() << '|'
            << data.getUpdateTime() << '|'
            << data.getOriginalUpdateTime() << '|'
            << data.getFirstHopSentTime() << '|'
            << data.getFirstHopDelayTime() << '|'
            << data.getSecondHopSentTime();
        const std::string s = oss.str();
        return std::vector<uint8_t>(s.begin(), s.end());
    } catch (...) {
        return {};
    }
}

std::vector<uint8_t> ZeroMQTrackDataPublisher::serializeFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) {
    try {
        std::ostringstream oss;
        oss.imbue(std::locale::classic());
        oss 
            << data.getTrackId() << '|'
            << data.getXVelocityECEF() << '|'
            << data.getYVelocityECEF() << '|'
            << data.getZVelocityECEF() << '|'
            << data.getXPositionECEF() << '|'
            << data.getYPositionECEF() << '|'
            << data.getZPositionECEF() << '|'
            << data.getUpdateTime() << '|'
            << data.getOriginalUpdateTime() << '|'
            << data.getFirstHopSentTime() << '|'
            << data.getFirstHopDelayTime() << '|'
            << data.getSecondHopSentTime() << '|'
            << data.getSecondHopDelayTime() << '|'
            << data.getTotalDelayTime() << '|'
            << data.getThirdHopSentTime();
        const std::string s = oss.str();
        return std::vector<uint8_t>(s.begin(), s.end());
    } catch (...) {
        return {};
    }
}

} // namespace hat::adapters::outgoing::zeromq
