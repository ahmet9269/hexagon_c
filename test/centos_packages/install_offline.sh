#!/bin/bash
#===============================================================================
# Offline Installer for CentOS/RHEL
# Bu script internet bağlantısı olmayan sistemde çalıştırılır
# 
# Kullanım:
#   sudo ./install_offline.sh
#
# Gereksinimler:
#   - CentOS 7/8 veya RHEL 7/8
#   - Root yetkisi
#   - gcc, g++, make, cmake kurulu olmalı
#===============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCES_DIR="$SCRIPT_DIR/sources"
BUILD_DIR="/tmp/b_hexagon_build"

# Renk tanımları
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo ""
echo -e "${BLUE}==============================================================${NC}"
echo -e "${BLUE} B_Hexagon Offline Installer - CentOS/RHEL${NC}"
echo -e "${BLUE}==============================================================${NC}"
echo ""

# Root kontrolü
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}✗ Bu script root olarak çalıştırılmalıdır${NC}"
    echo "  Kullanım: sudo ./install_offline.sh"
    exit 1
fi

# Derleme araçları kontrolü
check_build_tools() {
    echo -e "${YELLOW}Derleme araçları kontrol ediliyor...${NC}"
    
    local missing_tools=()
    
    command -v gcc >/dev/null 2>&1 || missing_tools+=("gcc")
    command -v g++ >/dev/null 2>&1 || missing_tools+=("g++")
    command -v make >/dev/null 2>&1 || missing_tools+=("make")
    command -v cmake >/dev/null 2>&1 || missing_tools+=("cmake")
    
    if [ ${#missing_tools[@]} -gt 0 ]; then
        echo -e "${RED}✗ Eksik araçlar: ${missing_tools[*]}${NC}"
        echo ""
        echo "Lütfen önce şu komutu çalıştırın:"
        echo "  yum groupinstall -y 'Development Tools'"
        echo "  yum install -y cmake cmake3"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Derleme araçları hazır${NC}"
}

# Build dizini oluştur
setup_build_dir() {
    mkdir -p $BUILD_DIR
    echo -e "${GREEN}✓ Build dizini: $BUILD_DIR${NC}"
}

# ZeroMQ kur
install_zeromq() {
    echo ""
    echo -e "${YELLOW}[1/5] ZeroMQ 4.3.5 kuruluyor (DRAFT API)...${NC}"
    
    if [ -f /usr/local/lib/libzmq.so ]; then
        echo -e "${GREEN}✓ ZeroMQ zaten kurulu, atlanıyor${NC}"
        return
    fi
    
    cd $BUILD_DIR
    tar xzf $SOURCES_DIR/zeromq-4.3.5.tar.gz
    cd zeromq-4.3.5
    
    # DRAFT API ile derle
    ./configure --enable-drafts --prefix=/usr/local --without-docs
    make -j$(nproc)
    make install
    
    echo -e "${GREEN}✓ ZeroMQ 4.3.5 kuruldu${NC}"
}

# spdlog kur
install_spdlog() {
    echo ""
    echo -e "${YELLOW}[2/5] spdlog 1.12.0 kuruluyor...${NC}"
    
    if [ -f /usr/local/lib/libspdlog.so ]; then
        echo -e "${GREEN}✓ spdlog zaten kurulu, atlanıyor${NC}"
        return
    fi
    
    cd $BUILD_DIR
    tar xzf $SOURCES_DIR/spdlog-1.12.0.tar.gz
    cd spdlog-1.12.0
    mkdir -p build && cd build
    
    cmake .. \
        -DSPDLOG_BUILD_SHARED=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local
    
    make -j$(nproc)
    make install
    
    echo -e "${GREEN}✓ spdlog 1.12.0 kuruldu${NC}"
}

# Google Test kur
install_gtest() {
    echo ""
    echo -e "${YELLOW}[3/5] Google Test 1.15.2 kuruluyor...${NC}"
    
    if [ -f /usr/local/lib/libgtest.so ]; then
        echo -e "${GREEN}✓ Google Test zaten kurulu, atlanıyor${NC}"
        return
    fi
    
    cd $BUILD_DIR
    tar xzf $SOURCES_DIR/googletest-1.15.2.tar.gz
    cd googletest-1.15.2
    mkdir -p build && cd build
    
    cmake .. \
        -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local
    
    make -j$(nproc)
    make install
    
    echo -e "${GREEN}✓ Google Test 1.15.2 kuruldu${NC}"
}

# cppzmq headers kur
install_cppzmq() {
    echo ""
    echo -e "${YELLOW}[4/5] cppzmq headers kuruluyor...${NC}"
    
    cp $SOURCES_DIR/zmq.hpp /usr/local/include/
    cp $SOURCES_DIR/zmq_addon.hpp /usr/local/include/
    
    echo -e "${GREEN}✓ cppzmq headers kuruldu${NC}"
}

# Doxygen kur
install_doxygen() {
    echo ""
    echo -e "${YELLOW}[5/5] Doxygen 1.9.8 kuruluyor...${NC}"
    
    if command -v doxygen &>/dev/null; then
        local version=$(doxygen --version 2>/dev/null)
        echo -e "${GREEN}✓ Doxygen $version zaten kurulu${NC}"
        return
    fi
    
    cd $BUILD_DIR
    tar xzf $SOURCES_DIR/doxygen-1.9.8.tar.gz
    cd doxygen-1.9.8
    mkdir -p build && cd build
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local
    
    make -j$(nproc)
    make install
    
    echo -e "${GREEN}✓ Doxygen 1.9.8 kuruldu${NC}"
}

# ldconfig güncelle
update_ldconfig() {
    echo ""
    echo -e "${YELLOW}Kütüphane önbelleği güncelleniyor...${NC}"
    
    # /usr/local/lib'i ldconfig'e ekle
    if [ ! -f /etc/ld.so.conf.d/usr-local.conf ]; then
        echo "/usr/local/lib" > /etc/ld.so.conf.d/usr-local.conf
        echo "/usr/local/lib64" >> /etc/ld.so.conf.d/usr-local.conf
    fi
    
    ldconfig
    
    echo -e "${GREEN}✓ ldconfig güncellendi${NC}"
}

# Temizlik
cleanup() {
    echo ""
    echo -e "${YELLOW}Geçici dosyalar temizleniyor...${NC}"
    rm -rf $BUILD_DIR
    echo -e "${GREEN}✓ Temizlik tamamlandı${NC}"
}

# Kurulum özeti
print_summary() {
    echo ""
    echo -e "${BLUE}==============================================================${NC}"
    echo -e "${GREEN} KURULUM TAMAMLANDI${NC}"
    echo -e "${BLUE}==============================================================${NC}"
    echo ""
    echo "Kurulu bileşenler:"
    echo "  • ZeroMQ       : $(ls /usr/local/lib/libzmq.so* 2>/dev/null | head -1 || echo 'N/A')"
    echo "  • spdlog       : $(ls /usr/local/lib/libspdlog.so* 2>/dev/null | head -1 || echo 'N/A')"
    echo "  • Google Test  : $(ls /usr/local/lib/libgtest.so* 2>/dev/null | head -1 || echo 'N/A')"
    echo "  • cppzmq       : /usr/local/include/zmq.hpp"
    echo "  • Doxygen      : $(doxygen --version 2>/dev/null || echo 'N/A')"
    echo ""
    echo -e "${YELLOW}ÖNEMLİ: Aşağıdaki satırları ~/.bashrc dosyanıza ekleyin:${NC}"
    echo ""
    echo "  export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:\$LD_LIBRARY_PATH"
    echo "  export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:\$PKG_CONFIG_PATH"
    echo "  export PATH=/usr/local/bin:\$PATH"
    echo ""
    echo "Veya şu komutu çalıştırın:"
    echo "  source /etc/profile.d/b_hexagon.sh"
    echo ""
}

# Profil scripti oluştur
create_profile_script() {
    cat > /etc/profile.d/b_hexagon.sh << 'EOF'
# B_Hexagon environment variables
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
export PATH=/usr/local/bin:$PATH
EOF
    chmod +x /etc/profile.d/b_hexagon.sh
}

# Ana fonksiyon
main() {
    check_build_tools
    setup_build_dir
    install_zeromq
    install_spdlog
    install_gtest
    install_cppzmq
    install_doxygen
    update_ldconfig
    create_profile_script
    cleanup
    print_summary
}

# Script'i çalıştır
main "$@"
