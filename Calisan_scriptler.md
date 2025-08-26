# Çalışan Scriptler ve Komutlar

Bu dosya, libzmq kütüphanesinin Ubuntu 24.04'te başarıyla derlenmesi için kullanılan çalışan komutları içerir.

## Problem
Ubuntu 24.04'te PIE (Position Independent Executable) güvenlik özelliği nedeniyle `make -j$(nproc)` komutu test dosyalarında şu hatayı veriyor:
```
/usr/bin/ld: ../lib/libunity.a(unity.c.o): relocation R_X86_64_32 against `.rodata' can not be used when making a PIE object; recompile with -fPIE
```

## Çözüm: Testleri Devre Dışı Bırakarak Derleme

### 1. CMake Yapılandırması (ENABLE_DRAFTS=ON ile)
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq
cmake -B build -DENABLE_DRAFTS=ON -DBUILD_TESTS=OFF -DZMQ_BUILD_TESTS=OFF
```

**Açıklama:**
- `DENABLE_DRAFTS=ON`: Draft API özelliklerini aktif eder
- `DBUILD_TESTS=OFF`: CMake test hedeflerini devre dışı bırakır
- `DZMQ_BUILD_TESTS=OFF`: ZMQ özel test hedeflerini devre dışı bırakır

### 2. Derleme
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq/build
make -j$(nproc)
```

**Açıklama:**
- `-j$(nproc)`: Sistemdeki tüm CPU çekirdeklerini kullanarak paralel derleme yapar
- `$(nproc)`: Mevcut CPU çekirdek sayısını otomatik olarak tespit eder

### 3. Alternatif: Sadece Kütüphane Derleme
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq/build
make libzmq -j$(nproc)          # Paylaşımlı kütüphane
make libzmq-static -j$(nproc)   # Statik kütüphane
```

## Oluşturulan Dosyalar

### Kütüphane Dosyaları (build/lib/ dizininde):
- `libzmq.so` - Paylaşımlı kütüphane
- `libzmq.a` - Statik kütüphane
- `libzmq.pc` - pkg-config dosyası

### Aktif Özellikler:
- ✅ Draft API (`DZMQ_BUILD_DRAFT_API=1`)
- ✅ WebSocket desteği
- ✅ WSS (WebSocket Secure) desteği
- ✅ Radix tree subscriptions
- ✅ libbsd desteği
- ✅ EPOLL I/O polling

## Doğrulama Komutları

### Draft API'nin aktif olduğunu kontrol etme:
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq/build
grep -r "DZMQ_BUILD_DRAFT_API" . | head -3
```

### Kütüphane dosyalarını listeleme:
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq/build/lib
ls -la
```

### Kütüphane boyutlarını kontrol etme:
```bash
cd /workspaces/hexagon_c/hexagon_c/libzmq/build/lib
du -h libzmq.*
```

## Kullanım Notları

1. **Draft API**: Bu derleme ile draft (deneysel) API'ler aktif durumda
2. **Testler**: PIE uyumluluk sorunu nedeniyle testler devre dışı bırakıldı
3. **Performans**: Paralel derleme ile hızlı sonuç alınır
4. **Güvenlik**: Modern Ubuntu güvenlik standartlarıyla uyumlu

## Son Güncelleme
Tarih: 26 Ağustos 2025
Platform: Ubuntu 24.04.2 LTS (Dev Container)
ZeroMQ Versiyon: 4.3.6

## C++ Wrapper Dönüşümü
✅ **Tamamlandı**: Tüm ZeroMQ kullanımları C style'dan C++ wrapper'a çevrildi

### Güncellenen Dosyalar:

#### hexagon_c Projesi:
1. `ZeroMQDishTrackDataSubscriber.hpp/.cpp` - C++ wrapper'a çevrildi
2. `ZeroMQTrackDataPublisher.hpp/.cpp` - C++ wrapper'a çevrildi  
3. `ZeroMQRadioTrackDataPublisher.hpp/.cpp` - C++ wrapper'a çevrildi
4. `ZeroMQTrackDataSubscriber.hpp/.cpp` - C++ wrapper'a çevrildi

#### hexagon_bx Projesi:
5. `ZeroMQDataPublisher.hpp` - C++ wrapper'a çevrildi

### C++ Wrapper Avantajları:
- ✅ RAII (Resource Acquisition Is Initialization) - Otomatik kaynak yönetimi
- ✅ Exception safety - Hata güvenliği
- ✅ Type safety - Tip güvenliği
- ✅ Modern C++ idioms
- ✅ Daha temiz ve okunaklı kod
- ✅ Memory leak protection

### Derleme Durumu:
- ✅ hexagon_c projesi - Derleme başarılı
- ✅ hexagon_bx projesi - Derleme başarılı
