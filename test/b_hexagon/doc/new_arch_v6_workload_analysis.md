# New Arch v6 - Belirli İş Yükü Analizi

**Doküman Versiyonu**: v6  
**Tarih**: 2024  
**Konu**: 200 Hz / 1-16 KB Senaryo Bazlı Performans Değerlendirmesi

---

## 1. Analiz Parametreleri

### 1.1 Hedef İş Yükü Spesifikasyonu

| Parametre | Değer | Açıklama |
|-----------|-------|----------|
| **Frekans** | 200 Hz | Saniyede 200 mesaj |
| **Periyot** | 5 ms | Mesajlar arası süre |
| **Min Veri Boyutu** | 1 KB | Küçük mesajlar |
| **Orta Veri Boyutu** | 8 KB | Tipik mesajlar |
| **Max Veri Boyutu** | 16 KB | Büyük mesajlar |
| **Ortalama Veri Boyutu** | ~8 KB | Ağırlıklı ortalama |

### 1.2 Zaman Bütçesi

```
Toplam Bütçe per Mesaj: 5.000 μs (5 ms)
├── Alım (Incoming)     : max 500 μs (%10)
├── İşleme (Domain)     : max 3.500 μs (%70)  ← Ana işlem süresi
├── Gönderim (Outgoing) : max 500 μs (%10)
└── Yedek (Headroom)    : 500 μs (%10)
```

---

## 2. Veri Boyutuna Göre Latency Analizi

### 2.1 Küçük Mesajlar (1 KB)

```
┌─────────────────────────────────────────────────────────────┐
│                    1 KB Mesaj İşleme                        │
├─────────────────────────────────────────────────────────────┤
│ ZeroMQ Receive      : ~10 μs                                │
│ Queue Push (Incoming): ~5 μs                                │
│ Queue Pop (Service)  : ~5 μs                                │
│ Domain Processing    : ~50 μs (hesaplama)                   │
│ Queue Push (Outgoing): ~5 μs                                │
│ Queue Pop (Publisher): ~5 μs                                │
│ ZeroMQ Send          : ~10 μs                               │
├─────────────────────────────────────────────────────────────┤
│ TOPLAM LATENCY       : ~90 μs                               │
│ Bütçe Kullanımı      : 90 / 5000 = %1.8                     │
└─────────────────────────────────────────────────────────────┘
```

**Sonuç**: ✅ Çok Uygun - Bütçenin sadece %1.8'i kullanılıyor

### 2.2 Orta Boyut Mesajlar (8 KB)

```
┌─────────────────────────────────────────────────────────────┐
│                    8 KB Mesaj İşleme                        │
├─────────────────────────────────────────────────────────────┤
│ ZeroMQ Receive      : ~25 μs                                │
│ Queue Push (Incoming): ~15 μs (copy overhead)               │
│ Queue Pop (Service)  : ~15 μs                               │
│ Domain Processing    : ~100 μs (daha fazla veri)            │
│ Queue Push (Outgoing): ~15 μs                               │
│ Queue Pop (Publisher): ~15 μs                               │
│ ZeroMQ Send          : ~25 μs                               │
├─────────────────────────────────────────────────────────────┤
│ TOPLAM LATENCY       : ~210 μs                              │
│ Bütçe Kullanımı      : 210 / 5000 = %4.2                    │
└─────────────────────────────────────────────────────────────┘
```

**Sonuç**: ✅ Uygun - Bütçenin sadece %4.2'si kullanılıyor

### 2.3 Büyük Mesajlar (16 KB)

```
┌─────────────────────────────────────────────────────────────┐
│                   16 KB Mesaj İşleme                        │
├─────────────────────────────────────────────────────────────┤
│ ZeroMQ Receive      : ~40 μs                                │
│ Queue Push (Incoming): ~25 μs (copy overhead)               │
│ Queue Pop (Service)  : ~25 μs                               │
│ Domain Processing    : ~150 μs (en fazla veri)              │
│ Queue Push (Outgoing): ~25 μs                               │
│ Queue Pop (Publisher): ~25 μs                               │
│ ZeroMQ Send          : ~45 μs                               │
├─────────────────────────────────────────────────────────────┤
│ TOPLAM LATENCY       : ~335 μs                              │
│ Bütçe Kullanımı      : 335 / 5000 = %6.7                    │
└─────────────────────────────────────────────────────────────┘
```

**Sonuç**: ✅ Uygun - Bütçenin sadece %6.7'si kullanılıyor

---

## 3. Karşılaştırmalı Özet Tablosu

| Veri Boyutu | Toplam Latency | Bütçe (5ms) | Kullanım | Yedek Süre | Durum |
|-------------|----------------|-------------|----------|------------|-------|
| 1 KB | 90 μs | 5000 μs | %1.8 | 4910 μs | ✅ Mükemmel |
| 8 KB | 210 μs | 5000 μs | %4.2 | 4790 μs | ✅ Çok İyi |
| 16 KB | 335 μs | 5000 μs | %6.7 | 4665 μs | ✅ İyi |

---

## 4. Throughput Analizi

### 4.1 Teorik Maksimum Throughput

```
┌─────────────────────────────────────────────────────────────┐
│              Maksimum Throughput Hesaplaması                │
├─────────────────────────────────────────────────────────────┤
│ 1 KB Mesajlar:                                              │
│   Max Rate = 1,000,000 / 90 = ~11,111 msg/sec               │
│   Hedef Rate = 200 msg/sec                                  │
│   Margin = 11,111 / 200 = 55x                               │
├─────────────────────────────────────────────────────────────┤
│ 8 KB Mesajlar:                                              │
│   Max Rate = 1,000,000 / 210 = ~4,762 msg/sec               │
│   Hedef Rate = 200 msg/sec                                  │
│   Margin = 4,762 / 200 = 24x                                │
├─────────────────────────────────────────────────────────────┤
│ 16 KB Mesajlar:                                             │
│   Max Rate = 1,000,000 / 335 = ~2,985 msg/sec               │
│   Hedef Rate = 200 msg/sec                                  │
│   Margin = 2,985 / 200 = 15x                                │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Bandwidth Analizi

```
Veri Hızı Hesaplaması (200 Hz):
├── 1 KB  @ 200 Hz = 200 KB/s = 1.6 Mbps
├── 8 KB  @ 200 Hz = 1.6 MB/s = 12.8 Mbps
└── 16 KB @ 200 Hz = 3.2 MB/s = 25.6 Mbps

Tipik Ethernet Kapasitesi: 1 Gbps
En Kötü Durum Kullanımı: 25.6 / 1000 = %2.56
```

**Sonuç**: Network bandwidth kesinlikle yeterli

---

## 5. Bellek Kullanım Analizi

### 5.1 Queue Buffer Boyutlandırması

```
┌─────────────────────────────────────────────────────────────┐
│              Queue Buffer Hesaplaması                       │
├─────────────────────────────────────────────────────────────┤
│ Önerilen Queue Kapasitesi: 100 mesaj (0.5 saniye buffer)    │
│                                                             │
│ Incoming Queue (16 KB max):                                 │
│   100 × 16 KB = 1.6 MB                                      │
│                                                             │
│ Outgoing Queue (16 KB max):                                 │
│   100 × 16 KB = 1.6 MB                                      │
│                                                             │
│ Toplam Queue Belleği: ~3.2 MB                               │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 Toplam Bellek Tahmini

| Bileşen | Bellek | Açıklama |
|---------|--------|----------|
| Base Process | ~5 MB | Kod + static data |
| Stack (3 thread) | ~3 MB | 1 MB × 3 thread |
| Heap (genel) | ~10 MB | Runtime allocations |
| Queue Buffers | ~3.2 MB | 2 queue × 100 × 16 KB |
| ZeroMQ Buffers | ~5 MB | Internal buffers |
| spdlog | ~2 MB | Log buffers |
| **TOPLAM** | **~28 MB** | |

**Sonuç**: Modern sistemler için çok düşük bellek kullanımı

---

## 6. CPU Kullanım Analizi

### 6.1 Thread Bazlı CPU Dağılımı

```
┌─────────────────────────────────────────────────────────────┐
│                CPU Kullanım Tahmini                         │
├─────────────────────────────────────────────────────────────┤
│ Senaryo: 200 Hz, 8 KB ortalama                              │
│                                                             │
│ IncomingAdapter Thread:                                     │
│   Aktif: 40 μs/mesaj × 200 = 8,000 μs/s = 0.8%              │
│   + Wait/Sleep: ~99.2% idle                                 │
│                                                             │
│ DomainService Thread:                                       │
│   Aktif: 130 μs/mesaj × 200 = 26,000 μs/s = 2.6%            │
│   + Wait/Sleep: ~97.4% idle                                 │
│                                                             │
│ OutgoingAdapter Thread:                                     │
│   Aktif: 40 μs/mesaj × 200 = 8,000 μs/s = 0.8%              │
│   + Wait/Sleep: ~99.2% idle                                 │
│                                                             │
│ TOPLAM CPU KULLANIMI: ~4.2%                                 │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 Çok Çekirdekli Avantaj

```
4 Core CPU Senaryosu:
├── Core 0: Main thread (minimal)
├── Core 1: IncomingAdapter (~0.8%)
├── Core 2: DomainService (~2.6%)
└── Core 3: OutgoingAdapter (~0.8%)

Toplam Sistem CPU: 4.2% / 4 = ~1% average
```

---

## 7. Burst Handling Kapasitesi

### 7.1 Kısa Süreli Yük Artışları

```
┌─────────────────────────────────────────────────────────────┐
│              Burst Handling Analizi                         │
├─────────────────────────────────────────────────────────────┤
│ Normal Rate: 200 Hz                                         │
│ Burst Rate: 2000 Hz (10x artış)                             │
│ Burst Süresi: 100 ms                                        │
│                                                             │
│ Burst Süresinde Gelen Mesaj: 200 mesaj                      │
│ Normal Sürede İşlenecek: 20 mesaj                           │
│ Queue'da Birikecek: 180 mesaj                               │
│                                                             │
│ Queue Kapasitesi: 100 mesaj                                 │
│ Sonuç: 80 mesaj DROP olabilir                               │
│                                                             │
│ Çözüm: Queue kapasitesini 200'e çıkar                       │
│ Ek Bellek: 100 × 16 KB = 1.6 MB                             │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 Önerilen Queue Boyutları

| Senaryo | Queue Kapasitesi | Bellek | Burst Tolerance |
|---------|------------------|--------|-----------------|
| Konservatif | 50 mesaj | 0.8 MB | 250 ms @ 200 Hz |
| Normal | 100 mesaj | 1.6 MB | 500 ms @ 200 Hz |
| Agresif | 200 mesaj | 3.2 MB | 1 sec @ 200 Hz |
| Yüksek Güvenilirlik | 500 mesaj | 8 MB | 2.5 sec @ 200 Hz |

---

## 8. Karşılaştırma: Mevcut vs Yeni Mimari

### 8.1 Latency Karşılaştırması

```
┌─────────────────────────────────────────────────────────────┐
│              Mimari Karşılaştırması (8 KB)                  │
├─────────────────────────────────────────────────────────────┤
│ MEVCUT MİMARİ (Single Thread + Outgoing Queue):             │
│   ZMQ Recv → Domain → Queue → ZMQ Send                      │
│   Latency: ~180 μs (seri işlem)                             │
│   Blocking riski: Domain yavaşlarsa her şey durur           │
│                                                             │
│ YENİ MİMARİ (3 Thread + 2 Queue):                           │
│   [ZMQ Recv → Queue] → [Domain → Queue] → [ZMQ Send]        │
│   Latency: ~210 μs (+30 μs overhead)                        │
│   Blocking riski: Yok, her katman bağımsız                  │
├─────────────────────────────────────────────────────────────┤
│ Latency Farkı: +30 μs (%17 artış)                           │
│ Güvenilirlik: Çok daha yüksek                               │
│ İzolasyon: Tam                                              │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 Esneklik Karşılaştırması

| Özellik | Mevcut | Yeni Arch v4 |
|---------|--------|--------------|
| Thread İzolasyonu | Kısmi | Tam |
| Blocking Riski | Var | Yok |
| Hata Yayılımı | Yüksek | Düşük |
| Ölçeklenebilirlik | Düşük | Yüksek |
| Test Edilebilirlik | Orta | Yüksek |
| CPU Dağılımı | Dengesiz | Dengeli |
| Bakım Kolaylığı | Orta | Yüksek |

---

## 9. Risk Analizi

### 9.1 Düşük Riskler ✅

| Risk | Olasılık | Etki | Azaltma |
|------|----------|------|---------|
| Queue overflow | Düşük | Orta | Dinamik boyutlandırma |
| Latency spike | Düşük | Düşük | Yedek bütçe var |
| Memory pressure | Çok Düşük | Düşük | 28 MB çok az |

### 9.2 Orta Riskler ⚠️

| Risk | Olasılık | Etki | Azaltma |
|------|----------|------|---------|
| Thread starvation | Orta | Orta | Thread priority ayarı |
| Cache thrashing | Düşük | Orta | Thread affinity |

### 9.3 Yüksek Riskler ❌

Belirlenen yüksek risk bulunmamaktadır.

---

## 10. Sonuç ve Öneriler

### 10.1 Genel Değerlendirme

```
┌─────────────────────────────────────────────────────────────┐
│                    SONUÇ: ✅ UYGUN                          │
├─────────────────────────────────────────────────────────────┤
│ Hedef İş Yükü: 200 Hz, 1-16 KB                              │
│                                                             │
│ ✅ Latency: %2-7 bütçe kullanımı (çok düşük)                │
│ ✅ Throughput: 15-55x margin (çok yüksek)                   │
│ ✅ Bellek: ~28 MB (çok düşük)                               │
│ ✅ CPU: ~4% (çok düşük)                                     │
│ ✅ Bandwidth: %2.5 (çok düşük)                              │
│                                                             │
│ Mimari bu iş yükü için fazlasıyla yeterli kapasiteye sahip. │
│ Gelecekte 5-10x büyüme bile sorunsuz karşılanabilir.        │
└─────────────────────────────────────────────────────────────┘
```

### 10.2 Önerilen Konfigürasyon

```cpp
// config.hpp
namespace config {
    // Queue ayarları
    constexpr size_t INCOMING_QUEUE_CAPACITY = 100;  // 0.5 sec buffer
    constexpr size_t OUTGOING_QUEUE_CAPACITY = 100;  // 0.5 sec buffer
    
    // Timeout ayarları
    constexpr auto QUEUE_PUSH_TIMEOUT = std::chrono::milliseconds(10);
    constexpr auto QUEUE_POP_TIMEOUT = std::chrono::milliseconds(50);
    
    // Thread ayarları
    constexpr int INCOMING_THREAD_PRIORITY = 0;   // Normal
    constexpr int DOMAIN_THREAD_PRIORITY = 0;     // Normal
    constexpr int OUTGOING_THREAD_PRIORITY = 0;   // Normal
    
    // Performans ayarları
    constexpr size_t MAX_MESSAGE_SIZE = 16 * 1024;  // 16 KB
    constexpr int TARGET_FREQUENCY_HZ = 200;
}
```

### 10.3 Ölçekleme Senaryoları

| Senaryo | Frekans | Veri Boyutu | Uygunluk |
|---------|---------|-------------|----------|
| Mevcut | 200 Hz | 8 KB avg | ✅ Mükemmel |
| 2x Büyüme | 400 Hz | 8 KB | ✅ Uygun |
| 5x Büyüme | 1000 Hz | 8 KB | ✅ Uygun |
| 10x Büyüme | 2000 Hz | 8 KB | ⚠️ Sınırda |
| Veri Büyümesi | 200 Hz | 64 KB | ✅ Uygun |

---

## 11. Sonraki Adımlar

1. **İmplementasyon** - new_arch_v4.md dokümanına göre kodlama
2. **Benchmark** - Gerçek performans ölçümü
3. **Stress Test** - Burst ve edge case testleri
4. **Fine Tuning** - Queue boyutu ve timeout optimizasyonu
5. **Production Deployment** - Kademeli geçiş

---

## Referanslar

- [new_arch_v4.md](new_arch_v4.md) - Mimari tasarım dokümanı
- [new_arch_v4_performance_analysis.md](new_arch_v4_performance_analysis.md) - Genel performans analizi

---

*Bu doküman, 200 Hz / 1-16 KB iş yükü için New Arch v4 mimarisinin uygunluk analizini içermektedir.*
