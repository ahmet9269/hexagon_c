#!/bin/bash
#===============================================================================
# Offline Package Downloader for CentOS/RHEL
# Bu script internet bağlantısı olan bir sistemde çalıştırılarak
# gerekli paketleri offline kurulum için indirir
#===============================================================================

set -e

DOWNLOAD_DIR="./offline_packages"

echo "=============================================="
echo " Offline Paket İndirici - CentOS/RHEL"
echo "=============================================="

mkdir -p $DOWNLOAD_DIR/{rpms,sources}

# RPM paketlerini indir (yumdownloader ile)
download_rpms() {
    echo ""
    echo "RPM paketleri indiriliyor..."
    
    # Gerekli araçlar
    yum install -y yum-utils
    
    # Paketleri indir (bağımlılıklarıyla birlikte)
    yumdownloader --resolve --destdir=$DOWNLOAD_DIR/rpms \
        doxygen \
        graphviz \
        gcc-c++ \
        make \
        cmake3 \
        libsodium-devel \
        openpgm-devel \
        git
    
    echo "✓ RPM paketleri indirildi: $DOWNLOAD_DIR/rpms/"
}

# Kaynak kodları indir
download_sources() {
    echo ""
    echo "Kaynak kodlar indiriliyor..."
    
    cd $DOWNLOAD_DIR/sources
    
    # ZeroMQ
    echo "  ZeroMQ indiriliyor..."
    curl -LO https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.tar.gz
    
    # spdlog
    echo "  spdlog indiriliyor..."
    curl -LO https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.tar.gz
    mv v1.12.0.tar.gz spdlog-1.12.0.tar.gz
    
    # Google Test
    echo "  Google Test indiriliyor..."
    curl -LO https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    mv v1.15.2.tar.gz googletest-1.15.2.tar.gz
    
    # cppzmq headers
    echo "  cppzmq headers indiriliyor..."
    curl -LO https://raw.githubusercontent.com/zeromq/cppzmq/master/zmq.hpp
    curl -LO https://raw.githubusercontent.com/zeromq/cppzmq/master/zmq_addon.hpp
    
    cd ../..
    echo "✓ Kaynak kodlar indirildi: $DOWNLOAD_DIR/sources/"
}

# Offline kurulum scripti oluştur
create_offline_installer() {
    cat > $DOWNLOAD_DIR/install_offline.sh << 'OFFLINE_SCRIPT'
#!/bin/bash
#===============================================================================
# Offline Installer for CentOS/RHEL
# Bu script internet bağlantısı olmayan sistemde çalıştırılır
#===============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=============================================="
echo " Offline Kurulum - CentOS/RHEL"
echo "=============================================="

# Root kontrolü
if [ "$EUID" -ne 0 ]; then
    echo "Bu script root olarak çalıştırılmalıdır"
    echo "Kullanım: sudo ./install_offline.sh"
    exit 1
fi

# RPM paketlerini kur
echo ""
echo "[1/4] RPM paketleri kuruluyor..."
rpm -Uvh --force $SCRIPT_DIR/rpms/*.rpm 2>/dev/null || yum localinstall -y $SCRIPT_DIR/rpms/*.rpm
echo "✓ RPM paketleri kuruldu"

# ZeroMQ derle ve kur
echo ""
echo "[2/4] ZeroMQ derleniyor..."
cd /tmp
tar xzf $SCRIPT_DIR/sources/zeromq-4.3.5.tar.gz
cd zeromq-4.3.5
./configure --enable-drafts --prefix=/usr/local
make -j$(nproc)
make install
echo "✓ ZeroMQ kuruldu"

# spdlog derle ve kur
echo ""
echo "[3/4] spdlog derleniyor..."
cd /tmp
tar xzf $SCRIPT_DIR/sources/spdlog-1.12.0.tar.gz
cd spdlog-1.12.0
mkdir -p build && cd build
cmake .. -DSPDLOG_BUILD_SHARED=ON -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
make install
echo "✓ spdlog kuruldu"

# Google Test derle ve kur
echo ""
echo "[4/4] Google Test derleniyor..."
cd /tmp
tar xzf $SCRIPT_DIR/sources/googletest-1.15.2.tar.gz
cd googletest-1.15.2
mkdir -p build && cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
make install
echo "✓ Google Test kuruldu"

# cppzmq headers kopyala
echo ""
echo "cppzmq headers kopyalanıyor..."
cp $SCRIPT_DIR/sources/zmq.hpp /usr/local/include/
cp $SCRIPT_DIR/sources/zmq_addon.hpp /usr/local/include/
echo "✓ cppzmq headers kuruldu"

# ldconfig güncelle
echo ""
echo "Kütüphane önbelleği güncelleniyor..."
echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf
ldconfig
echo "✓ ldconfig güncellendi"

# Temizlik
rm -rf /tmp/zeromq-4.3.5 /tmp/spdlog-1.12.0 /tmp/googletest-1.15.2

echo ""
echo "=============================================="
echo " KURULUM TAMAMLANDI"
echo "=============================================="
echo ""
echo "Ortam değişkenleri (.bashrc'ye ekleyin):"
echo "  export LD_LIBRARY_PATH=/usr/local/lib:\$LD_LIBRARY_PATH"
echo ""
OFFLINE_SCRIPT

    chmod +x $DOWNLOAD_DIR/install_offline.sh
    echo "✓ Offline installer oluşturuldu: $DOWNLOAD_DIR/install_offline.sh"
}

# Ana fonksiyon
main() {
    if [ -f /etc/redhat-release ]; then
        download_rpms
    else
        echo "⚠ CentOS/RHEL değil, sadece kaynak kodlar indirilecek"
    fi
    
    download_sources
    create_offline_installer
    
    echo ""
    echo "=============================================="
    echo " İNDİRME TAMAMLANDI"
    echo "=============================================="
    echo ""
    echo "Paketler: $DOWNLOAD_DIR/"
    echo ""
    echo "Kullanım:"
    echo "  1. $DOWNLOAD_DIR klasörünü hedef sisteme kopyalayın"
    echo "  2. sudo ./install_offline.sh çalıştırın"
    echo ""
}

main "$@"
