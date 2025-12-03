#!/bin/bash
#===============================================================================
# B_Hexagon Dependencies Installer for CentOS/RHEL
# Bu script CentOS/RHEL sistemlerde gerekli bağımlılıkları kurar
#===============================================================================

set -e

echo "=============================================="
echo " B_Hexagon Dependencies Installer"
echo " Target: CentOS/RHEL"
echo "=============================================="

# Renk tanımları
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Root kontrolü
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo -e "${RED}Bu script root olarak çalıştırılmalıdır${NC}"
        echo "Kullanım: sudo ./install_dependencies.sh"
        exit 1
    fi
}

# OS kontrolü
check_os() {
    if [ -f /etc/redhat-release ]; then
        echo -e "${GREEN}✓ CentOS/RHEL tespit edildi${NC}"
        OS_VERSION=$(cat /etc/redhat-release)
        echo "  Versiyon: $OS_VERSION"
    else
        echo -e "${YELLOW}⚠ Bu script CentOS/RHEL için tasarlanmıştır${NC}"
        read -p "Devam etmek istiyor musunuz? (y/n) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
}

# EPEL repository kurulumu
install_epel() {
    echo ""
    echo -e "${YELLOW}[1/6] EPEL Repository kuruluyor...${NC}"
    if yum list installed epel-release &>/dev/null; then
        echo -e "${GREEN}✓ EPEL zaten kurulu${NC}"
    else
        yum install -y epel-release
        echo -e "${GREEN}✓ EPEL kuruldu${NC}"
    fi
}

# Temel derleme araçları
install_build_tools() {
    echo ""
    echo -e "${YELLOW}[2/6] Derleme araçları kuruluyor...${NC}"
    yum groupinstall -y "Development Tools"
    yum install -y cmake cmake3 gcc-c++ make
    echo -e "${GREEN}✓ Derleme araçları kuruldu${NC}"
}

# Doxygen ve Graphviz
install_doxygen() {
    echo ""
    echo -e "${YELLOW}[3/6] Doxygen ve Graphviz kuruluyor...${NC}"
    yum install -y doxygen graphviz
    echo -e "${GREEN}✓ Doxygen $(doxygen --version) kuruldu${NC}"
    echo -e "${GREEN}✓ Graphviz kuruldu${NC}"
}

# ZeroMQ
install_zeromq() {
    echo ""
    echo -e "${YELLOW}[4/6] ZeroMQ kuruluyor...${NC}"
    
    # ZeroMQ bağımlılıkları
    yum install -y libsodium-devel openpgm-devel
    
    # ZeroMQ kaynak koddan derleme (DRAFT API desteği için)
    if [ ! -f /usr/local/lib/libzmq.so ]; then
        echo "  ZeroMQ kaynak koddan derleniyor (DRAFT API desteği için)..."
        
        cd /tmp
        if [ ! -d zeromq-4.3.5 ]; then
            curl -LO https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.tar.gz
            tar xzf zeromq-4.3.5.tar.gz
        fi
        
        cd zeromq-4.3.5
        ./configure --enable-drafts --prefix=/usr/local
        make -j$(nproc)
        make install
        ldconfig
        
        echo -e "${GREEN}✓ ZeroMQ 4.3.5 (DRAFT API) derlendi ve kuruldu${NC}"
    else
        echo -e "${GREEN}✓ ZeroMQ zaten kurulu${NC}"
    fi
}

# spdlog
install_spdlog() {
    echo ""
    echo -e "${YELLOW}[5/6] spdlog kuruluyor...${NC}"
    
    if [ ! -f /usr/local/lib/libspdlog.so ]; then
        cd /tmp
        if [ ! -d spdlog ]; then
            git clone --depth 1 --branch v1.12.0 https://github.com/gabime/spdlog.git
        fi
        
        cd spdlog
        mkdir -p build && cd build
        cmake .. -DSPDLOG_BUILD_SHARED=ON -DCMAKE_INSTALL_PREFIX=/usr/local
        make -j$(nproc)
        make install
        ldconfig
        
        echo -e "${GREEN}✓ spdlog 1.12.0 derlendi ve kuruldu${NC}"
    else
        echo -e "${GREEN}✓ spdlog zaten kurulu${NC}"
    fi
}

# Google Test
install_gtest() {
    echo ""
    echo -e "${YELLOW}[6/6] Google Test kuruluyor...${NC}"
    
    if [ ! -f /usr/local/lib/libgtest.so ]; then
        cd /tmp
        if [ ! -d googletest ]; then
            git clone --depth 1 --branch v1.15.2 https://github.com/google/googletest.git
        fi
        
        cd googletest
        mkdir -p build && cd build
        cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/usr/local
        make -j$(nproc)
        make install
        ldconfig
        
        echo -e "${GREEN}✓ Google Test 1.15.2 derlendi ve kuruldu${NC}"
    else
        echo -e "${GREEN}✓ Google Test zaten kurulu${NC}"
    fi
}

# ldconfig güncelleme
update_ldconfig() {
    echo ""
    echo -e "${YELLOW}Kütüphane önbelleği güncelleniyor...${NC}"
    echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf
    ldconfig
    echo -e "${GREEN}✓ ldconfig güncellendi${NC}"
}

# Kurulum özeti
print_summary() {
    echo ""
    echo "=============================================="
    echo -e "${GREEN} KURULUM TAMAMLANDI${NC}"
    echo "=============================================="
    echo ""
    echo "Kurulu bileşenler:"
    echo "  • Doxygen      : $(doxygen --version 2>/dev/null || echo 'N/A')"
    echo "  • Graphviz     : $(dot -V 2>&1 | head -1 || echo 'N/A')"
    echo "  • ZeroMQ       : /usr/local/lib/libzmq.so"
    echo "  • spdlog       : /usr/local/lib/libspdlog.so"
    echo "  • Google Test  : /usr/local/lib/libgtest.so"
    echo ""
    echo "Ortam değişkenleri (gerekirse .bashrc'ye ekleyin):"
    echo "  export LD_LIBRARY_PATH=/usr/local/lib:\$LD_LIBRARY_PATH"
    echo "  export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:\$PKG_CONFIG_PATH"
    echo ""
}

# Ana fonksiyon
main() {
    check_root
    check_os
    install_epel
    install_build_tools
    install_doxygen
    install_zeromq
    install_spdlog
    install_gtest
    update_ldconfig
    print_summary
}

# Script'i çalıştır
main "$@"
