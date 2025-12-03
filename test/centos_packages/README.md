# B_Hexagon CentOS/RHEL Bağımlılık Paketi

Bu dizin, B_Hexagon projesini CentOS/RHEL sistemlerde çalıştırmak için gerekli tüm bağımlılıkları içerir.

## 📦 İçerik

```
centos_packages/
├── README.md                    # Bu dosya
├── install_dependencies.sh      # Online kurulum scripti
├── install_offline.sh           # Offline kurulum scripti
├── download_packages.sh         # Paket indirme scripti
└── sources/                     # Kaynak kodlar
    ├── zeromq-4.3.5.tar.gz     # ZeroMQ (DRAFT API)
    ├── spdlog-1.12.0.tar.gz    # spdlog logging
    ├── googletest-1.15.2.tar.gz # Google Test/Mock
    ├── doxygen-1.9.8.tar.gz    # Doxygen
    ├── zmq.hpp                  # cppzmq header
    └── zmq_addon.hpp            # cppzmq addon header
```

## 🚀 Kurulum Yöntemleri

### Yöntem 1: Online Kurulum (İnternet Bağlantısı Var)

```bash
sudo ./install_dependencies.sh
```

### Yöntem 2: Offline Kurulum (İnternet Bağlantısı Yok)

1. Bu dizini hedef sisteme kopyalayın:
   ```bash
   scp -r centos_packages/ user@hedef-sistem:/tmp/
   ```

2. Hedef sistemde çalıştırın:
   ```bash
   cd /tmp/centos_packages
   sudo ./install_offline.sh
   ```

## 📋 Ön Gereksinimler

Offline kurulum öncesi hedef sistemde şunlar kurulu olmalı:

```bash
# Development Tools grubu
sudo yum groupinstall -y "Development Tools"

# CMake
sudo yum install -y cmake cmake3

# İsteğe bağlı: Graphviz (diyagramlar için)
sudo yum install -y graphviz
```

## 📚 Kurulacak Bileşenler

| Bileşen | Versiyon | Açıklama |
|---------|----------|----------|
| **ZeroMQ** | 4.3.5 | Mesajlaşma kütüphanesi (DRAFT API etkin) |
| **spdlog** | 1.12.0 | Hızlı logging kütüphanesi |
| **Google Test** | 1.15.2 | Unit test framework |
| **Google Mock** | 1.15.2 | Mocking framework |
| **cppzmq** | latest | ZeroMQ C++ bindings |
| **Doxygen** | 1.9.8 | Dokümantasyon oluşturucu |

## 🔧 Kurulum Sonrası

Kurulum tamamlandıktan sonra ortam değişkenlerini ayarlayın:

```bash
# Otomatik (her oturum açılışında)
source /etc/profile.d/b_hexagon.sh

# veya ~/.bashrc'ye ekleyin:
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
export PATH=/usr/local/bin:$PATH
```

## ✅ Kurulum Doğrulama

```bash
# ZeroMQ kontrolü
ls /usr/local/lib/libzmq.so*

# spdlog kontrolü
ls /usr/local/lib/libspdlog.so*

# Google Test kontrolü
ls /usr/local/lib/libgtest.so*

# Doxygen kontrolü
doxygen --version

# Headers kontrolü
ls /usr/local/include/zmq.hpp
ls /usr/local/include/spdlog/
ls /usr/local/include/gtest/
```

## 🔄 Güncelleme

Yeni versiyonlar için `sources/` dizinindeki arşivleri güncelleyin ve `install_offline.sh` scriptini tekrar çalıştırın.

## ⚠️ Sorun Giderme

### "libzmq.so not found" hatası
```bash
sudo ldconfig
# veya
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

### CMake bulunamıyor
```bash
# CentOS 7
sudo yum install -y cmake3
sudo alternatives --install /usr/bin/cmake cmake /usr/bin/cmake3 10

# CentOS 8
sudo dnf install -y cmake
```

### Graphviz (Doxygen diyagramları için)
```bash
sudo yum install -y graphviz
```

## 📞 Destek

Sorunlar için GitHub Issues kullanın.
