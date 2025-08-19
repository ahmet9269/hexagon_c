// Header dosyasını dahil et - Bu class'ın declaration'ını içerir
#include "ZeroMQTrackDataSubscriber.hpp"
// C string işlemleri için (strlen fonksiyonu kullanılacak)
#include <cstring>

// Hat projesi namespace'i altında, adapters->incoming->zeromq alt namespace'i
// :: operatörü namespace'leri birbirine bağlar
namespace hat::adapters::incoming::zeromq {

// CONSTRUCTOR - Class'ın yeni bir instance'ını oluşturur
// Bu fonksiyon class'ın bir objesi yaratıldığında otomatik çalışır
ZeroMQTrackDataSubscriber::ZeroMQTrackDataSubscriber(
    // std::shared_ptr<T> = Akıllı pointer, otomatik memory yönetimi yapar
    // domain::ports::incoming::TrackDataSubmission = Interface type
    // track_data_submission = Parametre adı, domain katmanına veri gönderecek
    std::shared_ptr<domain::ports::incoming::TrackDataSubmission> track_data_submission,
    // const std::string& = String referansı (const = değiştirilemez, & = kopyalama yapmaz)
    // delay_calc_endpoint = ZeroMQ bağlantı adresi (örn: tcp://localhost:5555)
    const std::string& delay_calc_endpoint,
    // topic_filter = Hangi mesajları dinleyeceğimizi belirler
    const std::string& topic_filter)
    // MEMBER INITIALIZER LIST başlangıcı - : operatörü ile başlar
    // Bu kısım member variable'ları constructor çalışmadan önce initialize eder
    : track_data_submission_(track_data_submission)  // Domain interface'ini kaydet
    , context_(1)                                   // ZeroMQ context'i 1 thread ile oluştur
    , running_(false)                               // Başlangıçta sistem durmuş durumda
    , delay_calc_endpoint_(delay_calc_endpoint)     // Endpoint adresini kaydet
    , topic_filter_(topic_filter)                  // Topic filter'ını kaydet
    , total_received_(0)                           // Alınan toplam mesaj sayısı = 0
    , successful_processed_(0)                     // Başarıyla işlenen mesaj sayısı = 0
    , failed_processed_(0) {                       // Hatalı mesaj sayısı = 0
    
    // Constructor body - Member'lar initialize edildikten sonra çalışır
    // ZeroMQ subscriber'larını kurmak için private fonksiyonu çağır
    initializeSubscribers();
}

// DESTRUCTOR - Class'ın instance'ı yok edilirken otomatik çalışır
// ~ operatörü destructor olduğunu belirtir
ZeroMQTrackDataSubscriber::~ZeroMQTrackDataSubscriber() {
    // Sistem durdurmak için stop() fonksiyonunu çağır
    // Bu thread'leri güvenli şekilde kapatır
    stop();
}

// PUBLIC METHOD - Subscriber'ı başlatır
// bool return type = true/false döner (başarı durumu)
bool ZeroMQTrackDataSubscriber::start() {
    // running_ atomic variable'ı thread-safe şekilde oku
    // .load() = atomic variable'dan değeri güvenli şekilde al
    if (running_.load()) {
        // return = fonksiyondan çık ve değer döndür
        return false; // Zaten çalışıyor, başlatma başarısız
    }

    // -> operatörü = pointer üzerinden member'a erişim
    // ! operatörü = logical NOT (tersini al)
    // Domain katmanı hazır mı kontrol et
    if (!track_data_submission_->isReadyToReceive()) {
        return false; // Domain hazır değil, başlatma başarısız
    }

    // .store() = atomic variable'a thread-safe şekilde değer yaz
    // Sistem çalışır duruma geçir
    running_.store(true);

    // std::thread = Yeni bir thread (işlem parçacığı) oluştur
    // Lambda expression = [this]() { ... } anonim fonksiyon
    // [this] = capture list, bu class'ın instance'ına erişim ver
    // () = parametre listesi (boş)
    // {} = lambda fonksiyonun gövdesi
    delay_calc_thread_ = std::thread([this]() {
        // Lambda içinde bu class'ın subscribeDelayCalcTrackData() metodunu çağır
        subscribeDelayCalcTrackData();
    });

    return true; // Başlatma başarılı
}

// PUBLIC METHOD - Subscriber'ı durdur
// void = hiçbir değer döndürme
void ZeroMQTrackDataSubscriber::stop() {
    // Atomic variable'a false yaz = sistem durduruluyor sinyali
    running_.store(false);

    // .joinable() = thread hala çalışıyor mu kontrol et
    // Thread'in güvenli şekilde kapanmasını bekle
    if (delay_calc_thread_.joinable()) {
        // .join() = ana thread'i beklet, bu thread bitene kadar dur
        // Bu olmadan program crash olabilir
        delay_calc_thread_.join();
    }
}

// PUBLIC METHOD - Sistem çalışıyor mu kontrol et
// const = bu fonksiyon class'ı değiştirmez (read-only)
bool ZeroMQTrackDataSubscriber::isRunning() const {
    // Atomic variable'dan güvenli şekilde değer oku
    return running_.load();
}

// PUBLIC METHOD - İstatistikleri döndür
// SubscriberStats = nested struct (class içinde tanımlanmış)
// :: = scope resolution operator, class içindeki type'a erişim
ZeroMQTrackDataSubscriber::SubscriberStats ZeroMQTrackDataSubscriber::getStats() const {
    // Local variable oluştur (stack memory'de)
    SubscriberStats stats;
    
    // Atomic variable'lardan thread-safe şekilde değerleri oku
    stats.total_received = total_received_.load();
    stats.successful_processed = successful_processed_.load();
    stats.failed_processed = failed_processed_.load();
    
    // Ternary operator: condition ? true_value : false_value
    // > operatörü = büyüktür karşılaştırması
    // static_cast<double> = explicit type conversion (int'i double'a çevir)
    // / operatörü = bölme işlemi
    stats.success_rate = stats.total_received > 0 ? 
        (double)stats.successful_processed / stats.total_received : 0.0;
    
    // Struct'ı value olarak döndür (copy semantics)
    return stats;
}

// PRIVATE METHOD - ZeroMQ subscriber'ları kurar
void ZeroMQTrackDataSubscriber::initializeSubscribers() {
    // std::make_unique<T> = unique_ptr oluştur, heap'te T tipinde obje yarat
    // zmq::socket_t = ZeroMQ socket class'ı
    // ZMQ_SUB = subscriber socket type (mesaj alıcı)
    // context_ = ZeroMQ context referansı
    delay_calc_subscriber_ = std::make_unique<zmq::socket_t>(context_, ZMQ_SUB);
    
    // -> = pointer üzerinden method çağırma
    // .connect() = ZeroMQ endpoint'ine bağlan
    // .c_str() = std::string'i C-style char* array'e çevir
    delay_calc_subscriber_->connect(delay_calc_endpoint_.c_str());
    
    // .empty() = string boş mu kontrol et
    if (!topic_filter_.empty()) {
        // ZMQ_SUBSCRIBE = hangi topic'leri dinleyeceğini ayarla
        // .setsockopt() = socket option ayarla
        // .length() = string uzunluğunu al
        delay_calc_subscriber_->setsockopt(ZMQ_SUBSCRIBE, topic_filter_.c_str(), topic_filter_.length());
    } else {
        // const char* = C-style string pointer
        // Varsayılan topic prefix tanımla
        const char* default_prefix = "DELAY_CALC_DATA"; // B tarafındaki default prefix ile eşleşir
        
        // strlen() = C string uzunluğunu hesapla (\0 karakterine kadar)
        // static_cast<int> = explicit type conversion
        delay_calc_subscriber_->setsockopt(ZMQ_SUBSCRIBE, default_prefix, (int)strlen(default_prefix));
    }

    // PERFORMANS OPTİMİZASYONU BAŞLANGIÇ
    // int = 32-bit integer type
    int timeout = 10; // Çok kısa timeout - 10ms, düşük gecikme için
    
    // ZMQ_RCVTIMEO = receive timeout option
    // &timeout = timeout variable'ının memory adresini ver
    // sizeof(timeout) = timeout variable'ının byte cinsinden boyutu
    delay_calc_subscriber_->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    
    // Receive High Water Mark - düşük buffer
    // HWM = kaç mesajın buffer'da bekleyebileceğini belirler
    int rcv_hwm = 10;
    // ZMQ_RCVHWM = receive high water mark option
    delay_calc_subscriber_->setsockopt(ZMQ_RCVHWM, &rcv_hwm, sizeof(rcv_hwm));
    
    // Linger time - hızlı kapanma
    // Socket kapanırken pending mesajları ne kadar beklesin
    int linger = 0; // 0 = hiç bekleme, hemen kapat
    // ZMQ_LINGER = linger time option
    delay_calc_subscriber_->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
}

// PRIVATE METHOD - Ana mesaj dinleme döngüsü
// Bu fonksiyon ayrı bir thread'de çalışır
void ZeroMQTrackDataSubscriber::subscribeDelayCalcTrackData() {
    // while = döngü, condition true olduğu sürece devam et
    // .load() = atomic variable'ı thread-safe oku
    while (running_.load()) {
        // try-catch = exception handling (hata yakalama)
        try {
            // ZeroMQ multipart message handling
            // zmq::message_t = ZeroMQ mesaj objesi
            zmq::message_t topic_msg; // İlk part: topic bilgisi
            
            // auto = type inference, compiler type'ı otomatik belirler
            // .recv() = mesaj al, return type: std::optional<size_t>
            // zmq::recv_flags::dontwait = non-blocking mode, beklemeden dön
            auto r1 = delay_calc_subscriber_->recv(topic_msg, zmq::recv_flags::dontwait);
            
            // if (r1) = r1 true mu kontrol et (mesaj alındı mı)
            // std::optional<T> boolean context'te has_value() gibi davranır
            if (r1) {
                // İkinci part: actual data
                zmq::message_t data_msg;
                
                // zmq::recv_flags::none = blocking mode, mesaj gelene kadar bekle
                auto r2 = delay_calc_subscriber_->recv(data_msg, zmq::recv_flags::none);
                
                // ! = logical NOT operator
                if (!r2) {
                    // ++ = increment operator, değeri 1 artır
                    // Atomic increment (thread-safe)
                    failed_processed_++;
                    // continue = döngünün başına dön, alttaki kodları atla
                    continue;
                }
                
                // Başarıyla alınan mesaj sayısını artır
                total_received_++;

                // Mesajı domain model'e çevir
                // auto = type inference: std::optional<domain::model::DelayCalcTrackData>
                auto data = deserializeDelayCalcTrackData(data_msg);
                
                // .has_value() = optional içinde değer var mı
                if (data.has_value()) {
                    // Domain katmanına veri gönder
                    // .value() = optional içindeki değeri al
                    // -> = pointer üzerinden method çağırma
                    if (track_data_submission_->submitDelayCalcTrackData(data.value())) {
                        // İşlem başarılı
                        successful_processed_++;
                    } else {
                        // İşlem başarısız
                        failed_processed_++;
                    }
                } else {
                    // Deserialization başarısız
                    failed_processed_++;
                }
            }
            
            // CPU'yu rahatlatmak için kısa bekleme
            // std::this_thread::sleep_for = mevcut thread'i belirtilen süre durdur
            // std::chrono::microseconds(100) = 100 mikrosaniye duration objesi
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            
        // catch = exception yakalama
        // const std::exception& = exception referansı (const = değiştirilmez)
        // & = reference, copy yapmaz
        } catch (const std::exception& e) {
            // Herhangi bir hata durumunda failed counter'ı artır
            failed_processed_++;
            // Hata loglaması yapılabilir (şu an boş)
        }
    } // while döngüsü sonu
}

// PRIVATE METHOD - ZeroMQ mesajını DelayCalcTrackData'ya çevir
// std::optional<T> = değer olabilir veya olmayabilir (nullable type)
// const zmq::message_t& = ZeroMQ mesajı referansı (const = değiştirilemez)
std::optional<domain::model::DelayCalcTrackData> ZeroMQTrackDataSubscriber::deserializeDelayCalcTrackData(const zmq::message_t& message) {
    // try-catch = hata yakalama mekanizması
    try {
        // static_cast<const char*> = explicit type conversion
        // .data() = mesajın raw data pointer'ını al
        // void* -> const char* conversion
        const char* begin = static_cast<const char*>(message.data());
        
        // std::string constructor: (başlangıç_pointer, bitiş_pointer)
        // .size() = mesajın byte cinsinden boyutu
        // + operatörü = pointer arithmetic, begin + size = end pointer
        std::string s(begin, begin + message.size());

        // std::istringstream = string'den okuma yapmak için stream
        // Input string stream - string'i parçalara ayırmak için kullanılır
        std::istringstream iss(s);
        
        // .imbue() = locale ayarla
        // std::locale::classic() = "C" locale, sayıların . ile ayrılması için
        iss.imbue(std::locale::classic());
        
        // Token'ları saklamak için değişkenler
        std::string token;                    // Geçici token
        std::vector<std::string> parts;       // Tüm parçalar

        // while döngüsü + std::getline = string'i delimiter ile ayır
        // std::getline(stream, output_string, delimiter)
        // '|' = pipe character, delimiter olarak kullanılıyor
        while (std::getline(iss, token, '|')) {
            // .push_back() = vector'ün sonuna eleman ekle
            parts.push_back(token);
        }
        
        // .size() = vector'daki eleman sayısı
        // != = not equal operator
        // DelayCalcTrackData 16 alan bekliyor
        if (parts.size() != 16) {
            // std::nullopt = optional için "değer yok" anlamına gelir
            return std::nullopt;
        }

        // Lambda expressions - anonim fonksiyonlar
        // auto = type inference, compiler type'ı belirler
        // [](parametre) { return değer; } = lambda syntax
        // std::stoi = string to integer conversion
        auto to_i = [](const std::string& v) { return std::stoi(v); };
        
        // std::stod = string to double conversion
        auto to_d = [](const std::string& v) { return std::stod(v); };
        
        // std::stoll = string to long long conversion
        // static_cast<int64_t> = long long'u int64_t'ye çevir
        auto to_i64 = [](const std::string& v) { return static_cast<int64_t>(std::stoll(v)); };

        // DelayCalcTrackData constructor çağrısı
        // parts[index] = vector'den index'teki elemanı al
        // Lambda fonksiyonları çağırarak string'leri uygun type'lara çevir
        domain::model::DelayCalcTrackData data(
            to_i(parts[0]),        // id - integer
            to_d(parts[1]),        // velocity - double  
            to_d(parts[2]),        // position - double
            to_i64(parts[3]),      // time_field - int64_t
            to_i(parts[4]),        // track_id - integer
            to_d(parts[5]),        // x_vel_ecef - double
            to_d(parts[6]),        // y_vel_ecef - double
            to_d(parts[7]),        // z_vel_ecef - double
            to_d(parts[8]),        // x_pos_ecef - double
            to_d(parts[9]),        // y_pos_ecef - double
            to_d(parts[10]),       // z_pos_ecef - double
            to_i64(parts[11]),     // update_time - int64_t
            to_i64(parts[12]),     // original_update_time - int64_t
            to_i64(parts[13]),     // first_hop_sent - int64_t
            to_i64(parts[14]),     // first_hop_delay - int64_t
            to_i64(parts[15])      // second_hop_sent - int64_t
        );

        // std::optional'a değer koyarak döndür
        return data;
        
    // catch (...) = tüm exception türlerini yakala
    } catch (...) {
        // Herhangi bir hata durumunda null döndür
        return std::nullopt;
    }
}

// Namespace kapatma
} // namespace hat::adapters::incoming::zeromq