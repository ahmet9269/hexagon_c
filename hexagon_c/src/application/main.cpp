// STL (Standard Template Library) header'ları - C++ standart kütüphanesi
#include <iostream>    // Console I/O için (std::cout, std::cin, std::cerr)
#include <memory>      // Smart pointer'lar için (std::shared_ptr, std::unique_ptr, std::make_shared)
#include <thread>      // Thread işlemleri için (std::thread, std::this_thread)
#include <chrono>      // Zaman işlemleri için (std::chrono::steady_clock, duration)
#include <csignal>     // Signal handling için (SIGINT, SIGTERM, signal())
#include <atomic>      // Thread-safe değişkenler için (std::atomic<T>)
#include <map>         // Anahtar-değer çiftleri için (std::map<K,V>)
#include <set>         // Unique değer koleksiyonu için (std::set<T>)
#include <mutex>       // Thread synchronization için (std::mutex, std::lock_guard)
#include <limits>      // Numeric limits için (std::numeric_limits<T>::max())
#include <optional>    // Nullable değerler için (std::optional<T>, std::nullopt)

// HEXAGONAL ARCHITECTURE LAYER IMPORTS
// Domain katmanı - İş mantığının kalbi, dış dünyadan bağımsız
#include "../domain/logic/TrackDataProcessor.hpp"

// Adapter katmanı - Dış dünya ile interface'ler
#include "../adapters/incoming/zeromq/ZeroMQTrackDataSubscriber.hpp"  // Veri alma
#include "../adapters/outgoing/zeromq/ZeroMQTrackDataPublisher.hpp"   // Veri gönderme

// using namespace = namespace'i otomatik kullan, hat:: yazmaya gerek kalmasın
using namespace hat;

// GLOBAL VARIABLES - Tüm fonksiyonlardan erişilebilir
// std::atomic<bool> = Thread-safe boolean, birden fazla thread güvenli erişebilir
// running = Sistem çalışır durumda mı flag'i
std::atomic<bool> running(true);  // () = constructor call, başlangıç değeri true

// SIGNAL HANDLER FUNCTION - İşletim sistemi sinyallerini yakalar
// void = hiçbir değer döndürme
// int signum = signal numarası (SIGINT=2, SIGTERM=15)
void signalHandler(int signum) {
    // std::cout = console output stream
    // << = stream insertion operator, veriyi stream'e yaz
    // \n = newline character (satır sonu)
    // std::endl = newline + flush buffer (buffer'ı temizle)
    std::cout << "\nShutdown signal received (" << signum << "). Stopping gracefully..." << std::endl;
    
    // .store() = atomic variable'a thread-safe şekilde değer yaz
    // false = sistem durduruluyor sinyali
    running.store(false);
}

/**
 * IN-MEMORY REPOSITORY IMPLEMENTATION
 * Repository Pattern implementasyonu - Veri saklama abstraction'ı
 * Gerçek uygulamada database, file system vs. kullanılacak
 * : public = public inheritance (is-a relationship)
 * domain::ports::outgoing::TrackDataRepository = Interface (abstract base class)
 */
class InMemoryTrackDataRepository : public domain::ports::outgoing::TrackDataRepository {
private:  // Private members - sadece bu class içinden erişilebilir
    // std::map<Key, Value> = Balanced binary search tree (Red-Black tree)
    // Key = int (ID), Value = domain model
    // Otomatik sıralama ve O(log n) arama performansı
    std::map<int, domain::model::DelayCalcTrackData> delay_calc_data_;
    std::map<int, domain::model::FinalCalcDelayData> final_calc_data_;
    
    // mutable = const fonksiyonlarda bile değiştirilebilir
    // std::mutex = mutual exclusion, thread synchronization primitive
    // Aynı anda sadece bir thread erişebilir
    mutable std::mutex data_mutex_;

public:   // Public members - dışarıdan erişilebilir
    // OVERRIDE METHODS - Base class'daki virtual fonksiyonları implement et
    
    // DelayCalcTrackData CRUD operations
    
    // override = compiler'a bu fonksiyonun base class'ı override ettiğini söyle
    // const domain::model::DelayCalcTrackData& = referans (kopyalama yapmaz)
    bool saveDelayCalcTrackData(const domain::model::DelayCalcTrackData& data) override {
        // std::lock_guard<std::mutex> = RAII lock pattern
        // Constructor'da lock, destructor'da unlock (scope bitiminde otomatik)
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        // [] = subscript operator, map'te key ile erişim
        // Eğer key yoksa yeni entry oluşturur
        // = assignment operator, değer atama
        delay_calc_data_[data.getId()] = data;
        
        return true;  // Başarılı
    }

    // std::optional<T> = nullable type, değer olabilir veya olmayabilir
    // Null pointer yerine type-safe alternatif
    std::optional<domain::model::DelayCalcTrackData> findDelayCalcTrackDataById(int id) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        // auto = type inference, compiler type'ı otomatik belirler
        // .find() = map'te key arama, iterator döner
        auto it = delay_calc_data_.find(id);
        
        // != = not equal operator
        // .end() = map'in bitiş iterator'ü (geçersiz pozisyon)
        // Iterator comparison: bulundu mu kontrol et
        if (it != delay_calc_data_.end()) {
            // -> = pointer member access (iterator pointer gibi davranır)
            // .second = map pair'inin değer kısmı (.first = key, .second = value)
            return it->second;
        }
        
        // std::nullopt = optional için "değer yok" literal'i
        return std::nullopt;
    }

    // std::vector<T> = dynamic array, boyutu runtime'da değişebilir
    std::vector<domain::model::DelayCalcTrackData> findDelayCalcTrackDataByTrackId(int track_id) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        // Local variable oluştur (stack memory'de)
        std::vector<domain::model::DelayCalcTrackData> result;
        
        // Range-based for loop (C++11) - koleksiyonun tüm elemanlarını dolaş
        // const auto& = referans ile erişim (kopyalama yapmaz), const = değiştirme
        // pair = map'teki her eleman std::pair<Key, Value> türünde
        for (const auto& pair : delay_calc_data_) {
            // == = equality comparison
            if (pair.second.getTrackId() == track_id) {
                // .push_back() = vector'ün sonuna eleman ekle
                result.push_back(pair.second);
            }
        }
        
        // Return by value - vector kopyalanır (RVO optimization ile optimize edilebilir)
        return result;
    }

    // FinalCalcDelayData operations - Yukarıdaki ile benzer mantık
    bool saveFinalCalcDelayData(const domain::model::FinalCalcDelayData& data) override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        // Track ID'yi key olarak kullan (ID yerine)
        final_calc_data_[data.getTrackId()] = data;
        return true;
    }

    // RepositoryStats = nested struct, base class'da tanımlanmış
    RepositoryStats getRepositoryStats() override {
        std::lock_guard<std::mutex> lock(data_mutex_);
        
        // Local struct instance oluştur
        RepositoryStats stats;
        
        // .size() = container'daki eleman sayısı
        stats.total_delay_calc_records = delay_calc_data_.size();
        stats.total_final_calc_records = final_calc_data_.size();
        
        // ACTIVE TRACKS CALCULATION
        // std::set<int> = unique integer'ların sorted collection'ı
        // Duplicate track ID'leri otomatik olarak filtreler
        std::set<int> track_ids;
        
        // DelayCalc data'larından track ID'leri topla
        for (const auto& pair : delay_calc_data_) {
            // .insert() = set'e eleman ekle (duplicate'ler otomatik ignore edilir)
            track_ids.insert(pair.second.getTrackId());
        }
        
        // FinalCalc data'larından da track ID'leri topla
        for (const auto& pair : final_calc_data_) {
            track_ids.insert(pair.second.getTrackId());
        }
        
        // Set'in boyutu = unique track sayısı
        stats.active_tracks = track_ids.size();
        
        // TIME RANGE CALCULATION - En eski ve en yeni record zamanları
        // Default değerler
        stats.oldest_record_time = 0;
        stats.newest_record_time = 0;
        
        // || = logical OR operator
        // .empty() = container boş mu kontrol et
        // En az bir map'te veri varsa zaman hesapla
        if (!delay_calc_data_.empty() || !final_calc_data_.empty()) {
            // int64_t = 64-bit signed integer
            // std::numeric_limits<int64_t>::max() = int64_t'nin maximum değeri
            int64_t min_time = std::numeric_limits<int64_t>::max();
            int64_t max_time = 0;
            
            // DelayCalc data'larından min/max zamanları bul
            for (const auto& pair : delay_calc_data_) {
                // std::min() = iki değerden küçük olanı döner
                min_time = std::min(min_time, pair.second.getUpdateTime());
                // std::max() = iki değerden büyük olanı döner
                max_time = std::max(max_time, pair.second.getUpdateTime());
            }
            
            // FinalCalc data'larından da min/max zamanları bul
            for (const auto& pair : final_calc_data_) {
                min_time = std::min(min_time, pair.second.getUpdateTime());
                max_time = std::max(max_time, pair.second.getUpdateTime());
            }
            
            // Sonuçları stats'e kaydet
            stats.oldest_record_time = min_time;
            stats.newest_record_time = max_time;
        }
        
        // Struct'ı value olarak döndür
        return stats;
    }
};

// MAIN FUNCTION - Program'ın giriş noktası
// int = return type (exit code: 0=başarılı, diğer=hata)
// argc = argument count (komut satırı argüman sayısı)
// char* argv[] = argument vector (komut satırı argümanları string array'i)
int main(int argc, char* argv[]) {
    // STARTUP BANNER - Uygulama başlangıç mesajları
    std::cout << "=== HaT - Hexagonal Architecture Tracking System ===" << std::endl;
    std::cout << "C++ Implementation with ZeroMQ" << std::endl;
    std::cout << "=====================================================" << std::endl;

    // SIGNAL HANDLER SETUP - İşletim sistemi sinyallerini yakala
    // signal() = C function, signal handler register eder
    // SIGINT = Ctrl+C signal (interrupt)
    // SIGTERM = termination signal (kill command)
    // signalHandler = yukarıda tanımladığımız fonksiyon pointer'ı
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // EXCEPTION HANDLING - Tüm main fonksiyonunu try-catch ile sar
    try {
        // CONFIGURATION - Sistem konfigürasyonu
        // std::string = C++ string class (dynamic, UTF-8 destekli)
        // TCP endpoint'leri tanımla
        std::string delay_calc_sub_endpoint = "tcp://localhost:7777";  // hexagon_b'den alır
        std::string delay_calc_pub_endpoint = "tcp://*:5557";          // Dışarıya yayınlar
        std::string final_calc_pub_endpoint = "tcp://*:5558";          // Dışarıya yayınlar

        // COMMAND LINE ARGUMENT PROCESSING
        // argc > 1 = en az bir argüman var mı (argv[0] = program adı)
        if (argc > 1) {
            // for loop = klasik C-style döngü
            // int i = 1 = başlangıç (argv[0]'ı atla)
            // i < argc = koşul
            // ++i = increment (pre-increment, daha verimli)
            for (int i = 1; i < argc; ++i) {
                // char* to std::string conversion
                std::string arg = argv[i];
                
                // == = string equality comparison
                // || = logical OR
                if (arg == "--help" || arg == "-h") {
                    // HELP MESSAGE - Kullanım kılavuzu
                    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                    std::cout << "Options:" << std::endl;
                    std::cout << "  --help, -h     Show this help message" << std::endl;
                    std::cout << "  --sub-port P   DelayCalc subscriber port (default: 5555)" << std::endl;
                    std::cout << "  --pub-port1 P  DelayCalc publisher port (default: 5557)" << std::endl;
                    std::cout << "  --pub-port2 P  FinalCalc publisher port (default: 5558)" << std::endl;
                    
                    // return = main fonksiyonundan çık
                    // 0 = başarılı exit code
                    return 0;
                }
            }
        }

        // CONFIGURATION DISPLAY - Mevcut ayarları göster
        std::cout << "Configuration:" << std::endl;
        std::cout << "  DelayCalc Subscriber: " << delay_calc_sub_endpoint << " (from hexagon_b)" << std::endl;
        std::cout << "  DelayCalc Publisher:  " << delay_calc_pub_endpoint << std::endl;
        std::cout << "  FinalCalc Publisher:  " << final_calc_pub_endpoint << std::endl;
        std::cout << "  Note: Receives data from hexagon_b, generates FinalCalc internally" << std::endl;

        // HEXAGONAL ARCHITECTURE COMPONENT CREATION
        // Dependency Injection pattern ile bileşenleri oluştur

        // 1. REPOSITORY (Secondary Port) - Veri saklama katmanı
        // auto = type inference: std::shared_ptr<InMemoryTrackDataRepository>
        // std::make_shared<T> = shared_ptr oluştur, heap'te T tipinde obje yarat
        // Referans sayacı ile otomatik memory management
        auto repository = std::make_shared<InMemoryTrackDataRepository>();

        // 2. PUBLISHER (Secondary Port) - Veri gönderme katmanı
        // std::shared_ptr = birden fazla owner olabilir, referans sayacı kullanır
        auto publisher = std::make_shared<adapters::outgoing::zeromq::ZeroMQTrackDataPublisher>(
            delay_calc_pub_endpoint, final_calc_pub_endpoint);

        // 3. DOMAIN LOGIC (Hexagon'un kalbi) - İş mantığı
        // Constructor'a publisher ve repository'yi dependency injection ile ver
        auto track_data_processor = std::make_shared<domain::logic::TrackDataProcessor>(
            publisher, repository);

        // 4. SUBSCRIBER (Primary Adapter) - Veri alma katmanı
        // std::unique_ptr = tek owner, move semantics ile transfer edilebilir
        // std::make_unique<T> = unique_ptr oluştur
        auto subscriber = std::make_unique<adapters::incoming::zeromq::ZeroMQTrackDataSubscriber>(
            track_data_processor, delay_calc_sub_endpoint);

        // COMPONENT STARTUP - Bileşenleri sırasıyla başlat
        std::cout << "\nStarting components..." << std::endl;

        // Publisher'ı başlat - Önce yayın kanallarını aç
        // ! = logical NOT operator
        // -> = pointer member access (smart pointer'dan method çağırma)
        if (!publisher->start()) {
            // std::cerr = console error stream (stderr)
            std::cerr << "Failed to start publisher!" << std::endl;
            return 1;  // Error exit code
        }
        std::cout << "✓ Publisher started" << std::endl;

        // Subscriber'ı başlat - Veri dinlemeye başla
        if (!subscriber->start()) {
            std::cerr << "Failed to start subscriber!" << std::endl;
            return 1;
        }
        std::cout << "✓ Subscriber started" << std::endl;

        // SYSTEM READY MESSAGE
        std::cout << "\n=== System is running ===\n" << std::endl;
        std::cout << "Listening for DelayCalcTrackData on ZeroMQ..." << std::endl;
        std::cout << "Generating FinalCalcDelayData internally and publishing both..." << std::endl;
        std::cout << "Press Ctrl+C to stop gracefully." << std::endl;

        // MAIN LOOP - Ana uygulama döngüsü
        // İstatistik gösterimi için zaman takibi
        // std::chrono::steady_clock = monotonic clock (sistem zamanı değişiminden etkilenmez)
        // ::now() = şu anki zaman damgası
        auto last_stats_time = std::chrono::steady_clock::now();
        
        // Ana döngü - running flag true olduğu sürece devam et
        // .load() = atomic variable'dan thread-safe okuma
        while (running.load()) {
            // std::this_thread::sleep_for = mevcut thread'i belirtilen süre durdur
            // std::chrono::seconds(1) = 1 saniye duration objesi
            // CPU'yu meşgul etmemek için bekle
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // STATISTICS DISPLAY - Her 10 saniyede bir istatistikleri göster
            auto now = std::chrono::steady_clock::now();
            
            // std::chrono::duration_cast<T> = duration type conversion
            // now - last_stats_time = zaman farkı
            // .count() = duration'ın numeric değerini al
            // >= = büyük eşit karşılaştırma
            if (std::chrono::duration_cast<std::chrono::seconds>(now - last_stats_time).count() >= 10) {
                
                std::cout << "\n--- Statistics ---" << std::endl;
                
                // SUBSCRIBER STATISTICS - Veri alma istatistikleri
                // auto = type inference: ZeroMQTrackDataSubscriber::SubscriberStats
                auto sub_stats = subscriber->getStats();
                std::cout << "Subscriber: Received=" << sub_stats.total_received 
                         << ", Processed=" << sub_stats.successful_processed
                         << ", Failed=" << sub_stats.failed_processed
                         << ", Success Rate=" << (sub_stats.success_rate * 100) << "%" << std::endl;

                // PUBLISHER STATISTICS - Veri gönderme istatistikleri
                auto pub_stats = publisher->getPublisherStats();
                std::cout << "Publisher: Published=" << pub_stats.total_published
                         << ", Failed=" << pub_stats.failed_publications
                         << ", Avg Latency=" << pub_stats.average_latency_ms << "ms" << std::endl;

                // REPOSITORY STATISTICS - Veri saklama istatistikleri
                auto repo_stats = repository->getRepositoryStats();
                std::cout << "Repository: DelayCalc=" << repo_stats.total_delay_calc_records
                         << ", FinalCalc=" << repo_stats.total_final_calc_records
                         << ", Active Tracks=" << repo_stats.active_tracks << std::endl;

                std::cout << "---" << std::endl;
                
                // Zaman damgasını güncelle
                last_stats_time = now;
            }
        } // while loop sonu

        // GRACEFUL SHUTDOWN - Sistem temiz kapatma
        std::cout << "\nShutting down components..." << std::endl;

        // Subscriber'ı önce durdur - Yeni veri gelmesin
        subscriber->stop();
        std::cout << "✓ Subscriber stopped" << std::endl;

        // Publisher'ı durdur - Pending mesajları göndersin
        publisher->stop();
        std::cout << "✓ Publisher stopped" << std::endl;

        // FINAL STATISTICS - Son istatistikleri göster
        std::cout << "\n=== Final Statistics ===" << std::endl;
        
        // Final stats'leri al
        auto final_sub_stats = subscriber->getStats();
        auto final_pub_stats = publisher->getPublisherStats();
        auto final_repo_stats = repository->getRepositoryStats();

        // Özet bilgileri göster
        std::cout << "Total Messages Received: " << final_sub_stats.total_received << std::endl;
        std::cout << "Total Messages Published: " << final_pub_stats.total_published << std::endl;
        std::cout << "Final Success Rate: " << (final_sub_stats.success_rate * 100) << "%" << std::endl;
        
        // + = addition operator (integer addition)
        std::cout << "Total Records in Repository: " 
                 << (final_repo_stats.total_delay_calc_records + final_repo_stats.total_final_calc_records) << std::endl;

        // SUCCESS SHUTDOWN MESSAGE
        std::cout << "\n=== HaT System Shutdown Complete ===" << std::endl;

    // EXCEPTION HANDLING - Herhangi bir hata durumunda
    // const std::exception& = standart exception referansı
    } catch (const std::exception& e) {
        // .what() = exception'ın hata mesajını döner
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;  // Error exit code
    }

    // SUCCESS EXIT - Normal sonlanma
    return 0;  // Success exit code
}