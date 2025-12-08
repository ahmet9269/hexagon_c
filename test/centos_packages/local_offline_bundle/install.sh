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
