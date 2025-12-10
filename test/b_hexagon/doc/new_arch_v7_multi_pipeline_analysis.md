# New Arch v7 - Çoklu Pipeline Analizi

**Doküman Versiyonu**: v7  
**Tarih**: 2024  
**Konu**: 20 Incoming + 20 Domain + 20 Outgoing Adapter Senaryosu

---

## 1. Sistem Topolojisi

### 1.1 Pipeline Mimarisi

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         20 BAĞIMSIZ PİPELINE                                    │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  Pipeline 1:  [IN-1] ──▶ [Queue] ──▶ [Domain-1] ──▶ [Queue] ──▶ [OUT-1]         │
│  Pipeline 2:  [IN-2] ──▶ [Queue] ──▶ [Domain-2] ──▶ [Queue] ──▶ [OUT-2]         │
│  Pipeline 3:  [IN-3] ──▶ [Queue] ──▶ [Domain-3] ──▶ [Queue] ──▶ [OUT-3]         │
│  ...                                                                            │
│  Pipeline 20: [IN-20] ──▶ [Queue] ──▶ [Domain-20] ──▶ [Queue] ──▶ [OUT-20]      │
│                                                                                 │
│  Thread Sayısı: 20 + 20 + 20 = 60 Thread                                        │
│  Queue Sayısı: 20 + 20 = 40 Queue                                               │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Temel Parametreler

| Parametre | Değer |
|-----------|-------|
| Pipeline Sayısı | 20 |
| Thread Sayısı | 60 (20 IN + 20 Domain + 20 OUT) |
| Queue Sayısı | 40 (20 incoming + 20 outgoing) |
| Frekans (per pipeline) | 200 Hz |
| Toplam Mesaj Hızı | 4000 msg/sec |
| Veri Boyutu | 8 KB (ortalama) |

---

## 2. CPU Kullanım Analizi

### 2.1 Thread Bazlı CPU Hesabı

```
┌─────────────────────────────────────────────────────────────┐
│              CPU KULLANIMI - 60 THREAD                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Her Pipeline (200 Hz @ 8 KB):                               │
│ ├── IncomingAdapter:  40 μs × 200 = 8,000 μs/s              │
│ ├── DomainService:   130 μs × 200 = 26,000 μs/s             │
│ └── OutgoingAdapter:  40 μs × 200 = 8,000 μs/s              │
│ Pipeline başına: 42,000 μs/s = %4.2                         │
│                                                             │
│ 20 Pipeline Toplamı:                                        │
│ ├── 20 × IN:     20 × 8,000 = 160,000 μs/s = %16            │
│ ├── 20 × Domain: 20 × 26,000 = 520,000 μs/s = %52           │
│ ├── 20 × OUT:    20 × 8,000 = 160,000 μs/s = %16            │
│ └── TOPLAM: 840,000 μs/s = %84                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 Bileşen Dağılımı

| Bileşen | Thread Sayısı | CPU Kullanımı | Oran |
|---------|---------------|---------------|------|
| IncomingAdapter | 20 | %16 | %19 |
| DomainService | 20 | %52 | %62 |
| OutgoingAdapter | 20 | %16 | %19 |
| **TOPLAM** | **60** | **%84** | **%100** |

---

## 3. Tek CPU Senaryosu (60 Thread)

### 3.1 Context Switch Analizi

```
┌─────────────────────────────────────────────────────────────┐
│              CONTEXT SWITCH - 60 THREAD                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Thread sayısı: 60                                           │
│ Aktif mesaj işleme: 20 pipeline × 200 Hz = 4000 msg/sec     │
│                                                             │
│ Her mesaj için context switch:                              │
│ ├── IN → Domain: 1 switch                                   │
│ ├── Domain → OUT: 1 switch                                  │
│ └── Mesaj başına: 2 switch minimum                          │
│                                                             │
│ Toplam switch: 4000 × 2 = 8000 switch/sec                   │
│ Switch overhead: 8000 × 2 μs = 16,000 μs/s = %1.6           │
│                                                             │
│ Güncellenmiş CPU kullanımı: %84 + %1.6 = %85.6              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Scheduling Latency Riski

```
┌─────────────────────────────────────────────────────────────┐
│              SCHEDULING LATENCY RİSKİ                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ 60 thread, tek CPU:                                         │
│                                                             │
│ En kötü durum (tüm thread'ler aynı anda çalışmak isterse):  │
│ ├── Her thread 10ms time slice                              │
│ ├── Full round: 60 × 10ms = 600ms!                          │
│ └── ⚠️ Bir thread 600ms bekleyebilir                        │
│                                                             │
│ Gerçekçi durum (thread'ler çoğunlukla sleep):               │
│ ├── Aynı anda aktif: ~5-10 thread                           │
│ ├── Bekleme: 50-100 μs                                      │
│ └── Kabul edilebilir                                        │
│                                                             │
│ SORUN: Burst anında tüm pipeline'lar aktif olabilir!        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.3 Tek CPU Performans Özeti

```
┌─────────────────────────────────────────────────────────────┐
│              TEK CPU - PERFORMANS ÖZETİ                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Toplam mesaj hızı: 4000 msg/sec                             │
│ CPU kullanımı: %85.6                                        │
│ Headroom: %14.4                                             │
│                                                             │
│ Throughput:                                                 │
│ ├── Max throughput: ~4700 msg/sec                           │
│ ├── Hedef: 4000 msg/sec                                     │
│ └── Margin: 1.17x ⚠️ ÇOK DAR                                │
│                                                             │
│ Latency Dağılımı:                                           │
│ ├── P50:  ~300 μs                                           │
│ ├── P95:  ~600 μs                                           │
│ ├── P99:  ~1000 μs                                          │
│ └── P99.9: ~3000 μs ❌                                      │
│                                                             │
│ SONUÇ: ❌ TEK CPU ÖNERİLMEZ                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.4 Burst Durumu Analizi

```
┌─────────────────────────────────────────────────────────────┐
│              BURST DURUMU - TEK CPU                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Senaryo: 20 pipeline aynı anda burst (400 Hz each)          │
│                                                             │
│ Gelen hız: 20 × 400 = 8000 msg/sec                          │
│ İşleme kapasitesi: ~4700 msg/sec (tek CPU limit)            │
│                                                             │
│ Sonuç: ❌ YETERSİZ                                          │
│ ├── Queue'lar hızla dolar                                   │
│ ├── Latency patlama yapar                                   │
│ └── Mesaj kaybı olabilir                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. Çoklu CPU Senaryoları

### 4.1 Seçenek A: 2 İzole CPU

```
┌─────────────────────────────────────────────────────────────┐
│              SEÇENEK A: 2 İZOLE CPU                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ CPU 2: I/O Thread'ler (40 thread)                           │
│ ├── 20 × IncomingAdapter                                    │
│ └── 20 × OutgoingAdapter                                    │
│ CPU Kullanımı: %32                                          │
│                                                             │
│ CPU 3: Domain Thread'ler (20 thread)                        │
│ └── 20 × DomainService                                      │
│ CPU Kullanımı: %52                                          │
│                                                             │
│ Throughput Margin: 2.35x                                    │
│                                                             │
│ Latency:                                                    │
│ ├── P50:  ~250 μs                                           │
│ ├── P99:  ~400 μs                                           │
│ └── P99.9: ~600 μs                                          │
│                                                             │
│ Değerlendirme: ⚠️ MİNİMUM KABULEDİLEBİLİR                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Seçenek B: 3 İzole CPU (Önerilen)

```
┌─────────────────────────────────────────────────────────────┐
│              SEÇENEK B: 3 İZOLE CPU (ÖNERİLEN)              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ CPU 2: Incoming Thread'ler (20 thread)                      │
│ └── 20 × IncomingAdapter                                    │
│ CPU Kullanımı: %16                                          │
│                                                             │
│ CPU 3: Domain Thread'ler (20 thread)                        │
│ └── 20 × DomainService                                      │
│ CPU Kullanımı: %52                                          │
│                                                             │
│ CPU 4: Outgoing Thread'ler (20 thread)                      │
│ └── 20 × OutgoingAdapter                                    │
│ CPU Kullanımı: %16                                          │
│                                                             │
│ Throughput Margin: 3.5x                                     │
│                                                             │
│ Latency:                                                    │
│ ├── P50:  ~220 μs                                           │
│ ├── P99:  ~280 μs                                           │
│ └── P99.9: ~350 μs                                          │
│                                                             │
│ Değerlendirme: ✅ ÖNERİLEN                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.3 Seçenek C: 4 İzole CPU (İdeal)

```
┌─────────────────────────────────────────────────────────────┐
│              SEÇENEK C: 4 İZOLE CPU (İDEAL)                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ CPU 2: Incoming (20 thread)          - %16                  │
│ CPU 3: Domain 1-10 (10 thread)       - %26                  │
│ CPU 4: Domain 11-20 (10 thread)      - %26                  │
│ CPU 5: Outgoing (20 thread)          - %16                  │
│                                                             │
│ Domain 2 CPU'ya dağıtılmış:                                 │
│ ├── Her CPU'da 10 domain thread                             │
│ ├── Daha düşük contention                                   │
│ └── Daha iyi cache locality                                 │
│                                                             │
│ Throughput Margin: 4.5x                                     │
│                                                             │
│ Latency:                                                    │
│ ├── P50:  ~215 μs                                           │
│ ├── P99:  ~250 μs                                           │
│ └── P99.9: ~300 μs                                          │
│                                                             │
│ Değerlendirme: ✅✅ İDEAL                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 5. Karşılaştırma Tablosu

### 5.1 CPU Seçenekleri Karşılaştırması

| Metrik | 1 CPU | 2 CPU | 3 CPU | 4 CPU |
|--------|-------|-------|-------|-------|
| Thread Sayısı | 60 | 60 | 60 | 60 |
| CPU Kullanımı | %85.6 | %42 avg | %28 avg | %21 avg |
| Throughput Margin | 1.17x ⚠️ | 2.35x | 3.5x | 4.5x |
| P50 Latency | ~300 μs | ~250 μs | ~220 μs | ~215 μs |
| P99 Latency | ~1000 μs | ~400 μs | ~280 μs | ~250 μs |
| P99.9 Latency | ~3000 μs | ~600 μs | ~350 μs | ~300 μs |
| Burst Tolerance | Çok Düşük | Orta | İyi | Çok İyi |
| Öneri | ❌ | ⚠️ Minimum | ✅ Önerilen | ✅✅ İdeal |

### 5.2 Throughput Karşılaştırması

```
┌─────────────────────────────────────────────────────────────┐
│              THROUGHPUT KARŞILAŞTIRMASI                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Hedef: 20 × 200 Hz = 4000 msg/sec                           │
│                                                             │
│ 1 CPU:                                                      │
│ ├── Max throughput: ~4700 msg/sec                           │
│ ├── Margin: 4700/4000 = 1.17x                               │
│ └── ⚠️ ÇOK DAR MARGIN                                       │
│                                                             │
│ 2 CPU:                                                      │
│ ├── Max throughput: ~9400 msg/sec                           │
│ ├── Margin: 9400/4000 = 2.35x                               │
│ └── ✅ Kabul edilebilir                                     │
│                                                             │
│ 3 CPU:                                                      │
│ ├── Max throughput: ~14000 msg/sec                          │
│ ├── Margin: 14000/4000 = 3.5x                               │
│ └── ✅ İyi                                                  │
│                                                             │
│ 4 CPU:                                                      │
│ ├── Max throughput: ~18000 msg/sec                          │
│ ├── Margin: 18000/4000 = 4.5x                               │
│ └── ✅ Çok iyi                                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. Paralel Pipeline Avantajı

### 6.1 Bağımsız Pipeline'ların Faydaları

```
┌─────────────────────────────────────────────────────────────┐
│              PARALEL PİPELINE AVANTAJI                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Her pipeline bağımsız:                                      │
│ ├── Pipeline-1 yavaşlasa bile Pipeline-2 etkilenmez         │
│ ├── Hata izolasyonu mükemmel                                │
│ └── Yatay ölçekleme kolay                                   │
│                                                             │
│ Throughput (çoklu CPU'da):                                  │
│ ├── Tek pipeline: 4700 msg/sec (tek CPU limit)              │
│ ├── 20 pipeline paralel: 20 × 4700 = 94,000 msg/sec!        │
│ └── Gerçek limit: CPU sayısına bağlı                        │
│                                                             │
│ Önemli: Bu mimaride her pipeline'ın kendi domain'i var      │
│         → Merkezi darboğaz YOK!                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 Hata İzolasyonu

| Senaryo | Tek Domain | 20 Domain |
|---------|------------|-----------|
| Domain-5 crash | Tüm sistem durur | Sadece Pipeline-5 durur |
| Domain-5 yavaş | Tüm mesajlar gecikmeli | Sadece Pipeline-5 gecikmeli |
| Queue-5 dolu | Tüm sistem etkilenir | Sadece Pipeline-5 etkilenir |

---

## 7. Bellek Kullanımı

### 7.1 Memory Footprint

```
┌─────────────────────────────────────────────────────────────┐
│              BELLEK KULLANIMI - 60 THREAD                   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Thread Stack (60 thread × 1MB default):                     │
│ └── 60 MB                                                   │
│                                                             │
│ Queue Buffers (40 queue × 100 × 16KB):                      │
│ └── 64 MB                                                   │
│                                                             │
│ ZeroMQ Buffers (40 socket):                                 │
│ └── ~20 MB                                                  │
│                                                             │
│ Application Code + Heap:                                    │
│ └── ~20 MB                                                  │
│                                                             │
│ TOPLAM: ~164 MB                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 Bellek Optimizasyonu

| Optimizasyon | Tasarruf | Yeni Toplam |
|--------------|----------|-------------|
| Thread stack 256KB | -45 MB | 119 MB |
| Queue capacity 50 | -32 MB | 87 MB |
| ZMQ buffer tuning | -10 MB | 77 MB |
| **Toplam Optimizasyon** | **-87 MB** | **77 MB** |

---

## 8. Konfigürasyon

### 8.1 GRUB Konfigürasyonu

```bash
# 3 İzole CPU için (Önerilen)
GRUB_CMDLINE_LINUX="isolcpus=2,3,4 nohz_full=2,3,4 rcu_nocbs=2,3,4"

# 4 İzole CPU için (İdeal)
GRUB_CMDLINE_LINUX="isolcpus=2,3,4,5 nohz_full=2,3,4,5 rcu_nocbs=2,3,4,5"

# Uygula
sudo update-grub
sudo reboot
```

### 8.2 Thread Affinity Kodu

```cpp
#include <pthread.h>
#include <sched.h>
#include <thread>
#include <vector>

void setThreadAffinity(std::thread& t, int cpuCore) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpuCore, &cpuset);
    
    int rc = pthread_setaffinity_np(
        t.native_handle(),
        sizeof(cpu_set_t),
        &cpuset
    );
    
    if (rc != 0) {
        spdlog::error("Failed to set CPU affinity for core {}", cpuCore);
    }
}

// 3 CPU Konfigürasyonu
void configure3CPU(
    std::vector<std::thread>& incomingThreads,
    std::vector<std::thread>& domainThreads,
    std::vector<std::thread>& outgoingThreads
) {
    // Incoming Adapters → CPU 2
    for (auto& t : incomingThreads) {
        setThreadAffinity(t, 2);
    }
    
    // Domain Services → CPU 3
    for (auto& t : domainThreads) {
        setThreadAffinity(t, 3);
    }
    
    // Outgoing Adapters → CPU 4
    for (auto& t : outgoingThreads) {
        setThreadAffinity(t, 4);
    }
}

// 4 CPU Konfigürasyonu (Domain Split)
void configure4CPU(
    std::vector<std::thread>& incomingThreads,
    std::vector<std::thread>& domainThreads,
    std::vector<std::thread>& outgoingThreads
) {
    // Incoming Adapters → CPU 2
    for (auto& t : incomingThreads) {
        setThreadAffinity(t, 2);
    }
    
    // Domain Services 1-10 → CPU 3
    for (int i = 0; i < 10; i++) {
        setThreadAffinity(domainThreads[i], 3);
    }
    
    // Domain Services 11-20 → CPU 4
    for (int i = 10; i < 20; i++) {
        setThreadAffinity(domainThreads[i], 4);
    }
    
    // Outgoing Adapters → CPU 5
    for (auto& t : outgoingThreads) {
        setThreadAffinity(t, 5);
    }
}
```

### 8.3 Systemd Service

```ini
# /etc/systemd/system/hexagon-multi.service
[Unit]
Description=Hexagon Multi-Pipeline Application
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/bin/hexagon_multi_app
CPUAffinity=2 3 4 5
Nice=-10
LimitRTPRIO=99
LimitMEMLOCK=infinity
LimitNOFILE=65535

# Thread stack size optimizasyonu
Environment="MALLOC_ARENA_MAX=2"
Environment="PTHREAD_STACK_MIN=262144"

[Install]
WantedBy=multi-user.target
```

---

## 9. Risk Analizi

### 9.1 Risk Matrisi

| Risk | 1 CPU | 2 CPU | 3 CPU | 4 CPU |
|------|-------|-------|-------|-------|
| CPU Doygunluğu | ❌ Yüksek | ⚠️ Orta | ✅ Düşük | ✅ Çok Düşük |
| Latency Spike | ❌ Yüksek | ⚠️ Orta | ✅ Düşük | ✅ Çok Düşük |
| Burst Failure | ❌ Yüksek | ⚠️ Orta | ✅ Düşük | ✅ Çok Düşük |
| Thread Starvation | ❌ Yüksek | ⚠️ Orta | ✅ Düşük | ✅ Çok Düşük |
| Memory Pressure | ✅ Düşük | ✅ Düşük | ✅ Düşük | ✅ Düşük |

### 9.2 Azaltma Stratejileri

| Risk | Azaltma Stratejisi |
|------|---------------------|
| CPU Doygunluğu | Daha fazla izole CPU tahsis et |
| Latency Spike | Domain thread'leri ayrı CPU'ya taşı |
| Burst Failure | Queue kapasitesini artır |
| Thread Starvation | Thread priority ayarla |

---

## 10. Sonuç ve Öneriler

### 10.1 Genel Değerlendirme

```
┌─────────────────────────────────────────────────────────────┐
│                    SONUÇ                                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ 20 IN + 20 Domain + 20 OUT = 60 thread senaryosu için:      │
│                                                             │
│ ❌ TEK CPU: ÖNERİLMEZ                                       │
│    └── %85.6 kullanım, 1.17x margin, P99.9 = 3ms            │
│                                                             │
│ ⚠️ 2 CPU: MİNİMUM                                           │
│    └── %42 kullanım, 2.35x margin, P99.9 = 600μs            │
│                                                             │
│ ✅ 3 CPU: ÖNERİLEN                                          │
│    └── %28 kullanım, 3.5x margin, P99.9 = 350μs             │
│                                                             │
│ ✅✅ 4 CPU: İDEAL                                           │
│    └── %21 kullanım, 4.5x margin, P99.9 = 300μs             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 10.2 Önerilen Konfigürasyon

```
┌─────────────────────────────────────────────────────────────┐
│              ÖNERİLEN KONFİGÜRASYON                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ MİNİMUM: 3 İZOLE CPU                                        │
│ ├── CPU 2: 20 × IncomingAdapter                             │
│ ├── CPU 3: 20 × DomainService                               │
│ └── CPU 4: 20 × OutgoingAdapter                             │
│                                                             │
│ İDEAL: 4 İZOLE CPU                                          │
│ ├── CPU 2: 20 × IncomingAdapter                             │
│ ├── CPU 3: 10 × DomainService (1-10)                        │
│ ├── CPU 4: 10 × DomainService (11-20)                       │
│ └── CPU 5: 20 × OutgoingAdapter                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 10.3 Uygulama Checklist

- [ ] GRUB konfigürasyonu güncelle (isolcpus, nohz_full, rcu_nocbs)
- [ ] Sistem yeniden başlat
- [ ] Thread affinity kodunu implement et
- [ ] Systemd service dosyası oluştur
- [ ] Thread stack size optimize et (256KB)
- [ ] Queue kapasitelerini ayarla
- [ ] IRQ affinity ayarla
- [ ] Performans testi yap
- [ ] Latency dağılımını doğrula

---

## Referanslar

- [new_arch_v4.md](new_arch_v4.md) - Temel mimari tasarım
- [new_arch_v4_performance_analysis.md](new_arch_v4_performance_analysis.md) - Performans analizi
- [new_arch_v6_workload_analysis.md](new_arch_v6_workload_analysis.md) - 200 Hz iş yükü analizi

---

*Bu doküman, 20 bağımsız pipeline (60 thread) senaryosu için CPU tahsis stratejilerini ve performans analizini içermektedir.*
