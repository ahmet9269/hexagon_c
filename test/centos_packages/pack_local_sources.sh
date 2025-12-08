#!/bin/bash
set -e

# Dizin tanımları
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(dirname "$SCRIPT_DIR")" # test/ directory
BUNDLE_DIR="$SCRIPT_DIR/local_offline_bundle"
SOURCES_DIR="$BUNDLE_DIR/sources"

echo "========================================================"
echo " Local Source Packager for Offline Installation"
echo "========================================================"
echo "Workspace: $WORKSPACE_DIR"
echo "Bundle Dir: $BUNDLE_DIR"

# Temizlik ve Hazırlık
rm -rf "$BUNDLE_DIR"
mkdir -p "$SOURCES_DIR"

# 1. Local spdlog paketle
echo "[1/4] Local spdlog paketleniyor..."
if [ -d "$WORKSPACE_DIR/spdlog" ]; then
    tar -czf "$SOURCES_DIR/spdlog.tar.gz" -C "$WORKSPACE_DIR" spdlog
    echo "✓ spdlog paketlendi"
else
    echo "HATA: $WORKSPACE_DIR/spdlog bulunamadı!"
    exit 1
fi

# 2. Local googletest paketle
echo "[2/4] Local googletest paketleniyor..."
if [ -d "$WORKSPACE_DIR/googletest" ]; then
    tar -czf "$SOURCES_DIR/googletest.tar.gz" -C "$WORKSPACE_DIR" googletest
    echo "✓ googletest paketlendi"
else
    echo "HATA: $WORKSPACE_DIR/googletest bulunamadı!"
    exit 1
fi

# 3. Diğer bağımlılıkları kopyala (ZeroMQ vb.)
echo "[3/4] Diğer bağımlılıklar kopyalanıyor..."
# Eğer sources klasöründe varsa oradan al, yoksa uyar
if [ -f "$SCRIPT_DIR/sources/zeromq-4.3.5.tar.gz" ]; then
    cp "$SCRIPT_DIR/sources/zeromq-4.3.5.tar.gz" "$SOURCES_DIR/"
    echo "✓ ZeroMQ kopyalandı"
else
    echo "UYARI: ZeroMQ tarball bulunamadı ($SCRIPT_DIR/sources/zeromq-4.3.5.tar.gz)"
fi

if [ -f "$SCRIPT_DIR/sources/zmq.hpp" ]; then
    cp "$SCRIPT_DIR/sources/zmq.hpp" "$SOURCES_DIR/"
    cp "$SCRIPT_DIR/sources/zmq_addon.hpp" "$SOURCES_DIR/"
    echo "✓ cppzmq headers kopyalandı"
fi

if [ -f "$SCRIPT_DIR/sources/doxygen-1.9.8.tar.gz" ]; then
    cp "$SCRIPT_DIR/sources/doxygen-1.9.8.tar.gz" "$SOURCES_DIR/"
    echo "✓ Doxygen kopyalandı"
fi

# 4. Kurulum scriptini oluştur ve özelleştir
echo "[4/4] Kurulum scripti oluşturuluyor..."
cat > "$BUNDLE_DIR/install.sh" << 'EOF'
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCES_DIR="$SCRIPT_DIR/sources"
BUILD_DIR="/tmp/hexagon_local_build"

echo "========================================================"
echo " Offline Installer (Local Sources)"
echo "========================================================"

if [ "$EUID" -ne 0 ]; then
    echo "Lütfen root yetkisiyle çalıştırın: sudo ./install.sh"
    exit 1
fi

mkdir -p $BUILD_DIR

# --- ZeroMQ Kurulumu ---
if [ -f "$SOURCES_DIR/zeromq-4.3.5.tar.gz" ]; then
    echo "ZeroMQ kuruluyor..."
    cd $BUILD_DIR
    tar xzf "$SOURCES_DIR/zeromq-4.3.5.tar.gz"
    cd zeromq-4.3.5
    ./configure --enable-drafts --prefix=/usr/local --without-docs
    make -j$(nproc)
    make install
    echo "✓ ZeroMQ tamamlandı"
fi

# --- spdlog Kurulumu (Local Source) ---
echo "spdlog kuruluyor..."
cd $BUILD_DIR
rm -rf spdlog # Temizlik
tar xzf "$SOURCES_DIR/spdlog.tar.gz"
cd spdlog
mkdir -p build && cd build
cmake .. -DSPDLOG_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
make install
echo "✓ spdlog tamamlandı"

# --- Google Test Kurulumu (Local Source) ---
echo "Google Test kuruluyor..."
cd $BUILD_DIR
rm -rf googletest # Temizlik
tar xzf "$SOURCES_DIR/googletest.tar.gz"
cd googletest
mkdir -p build && cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
make install
echo "✓ Google Test tamamlandı"

# --- cppzmq ---
if [ -f "$SOURCES_DIR/zmq.hpp" ]; then
    cp "$SOURCES_DIR/zmq.hpp" /usr/local/include/
    cp "$SOURCES_DIR/zmq_addon.hpp" /usr/local/include/
    echo "✓ cppzmq headers kopyalandı"
fi

# --- Doxygen ---
if [ -f "$SOURCES_DIR/doxygen-1.9.8.tar.gz" ]; then
    echo "Doxygen kuruluyor..."
    cd $BUILD_DIR
    tar xzf "$SOURCES_DIR/doxygen-1.9.8.tar.gz"
    cd doxygen-1.9.8
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
    make -j$(nproc)
    make install
    echo "✓ Doxygen tamamlandı"
fi

# --- Yapılandırma ---
echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf
ldconfig
rm -rf $BUILD_DIR

echo ""
echo "KURULUM BAŞARIYLA TAMAMLANDI!"
EOF

chmod +x "$BUNDLE_DIR/install.sh"

echo ""
echo "Paketleme tamamlandı: $BUNDLE_DIR"
echo "Bu klasörü hedef CentOS makinesine taşıyın ve './install.sh' çalıştırın."
