#!/bin/bash
# CodixOS WSL Installation Script
# Installs CodixOS components in WSL environment

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Print banner
print_banner() {
    echo -e "${CYAN}"
    echo "        _____      _               ____   _____ "
    echo "       / ____|    | |             |  _ \ / ____|"
    echo "      | |     ___ | | ___  _ __   | |_) | (___  "
    echo "      | |    / _ \| |/ _ \| '__|  |  _ < \___ \ "
    echo "      | |___| (_) | | (_) | |     | |_) |____) |"
    echo "       \_____\___/|_|\___/|_|     |____/|_____/ "
    echo -e "${NC}"
    echo -e "${GREEN}  CodixOS WSL Installer v1.0.0${NC}"
    echo ""
}

# Check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo -e "${RED}Error: This script must be run as root${NC}"
        echo "Please run: sudo ./install.sh"
        exit 1
    fi
}

# Install dependencies
install_dependencies() {
    echo -e "${YELLOW}Installing dependencies...${NC}"
    
    # Update package list
    apt-get update
    
    # Install required packages
    apt-get install -y \
        build-essential \
        gcc \
        g++ \
        make \
        nasm \
        grub-pc-bin \
        grub-common \
        xorriso \
        mtools \
        git \
        wget \
        curl \
        vim \
        nano \
        htop \
        tmux \
        screen
    
    echo -e "${GREEN}Dependencies installed successfully${NC}"
}

# Install CodixOS
install_codixos() {
    echo -e "${YELLOW}Installing CodixOS...${NC}"
    
    # Create installation directory
    INSTALL_DIR="/opt/codixos"
    mkdir -p "$INSTALL_DIR"
    
    # Copy files
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    cp -r "$SCRIPT_DIR/../../"* "$INSTALL_DIR/"
    
    # Build CodixOS
    cd "$INSTALL_DIR"
    make all
    
    echo -e "${GREEN}CodixOS installed successfully${NC}"
}

# Create desktop entry
create_desktop_entry() {
    echo -e "${YELLOW}Creating desktop entry...${NC}"
    
    DESKTOP_DIR="$HOME/.local/share/applications"
    mkdir -p "$DESKTOP_DIR"
    
    cat > "$DESKTOP_DIR/codixos.desktop" << EOF
[Desktop Entry]
Name=CodixOS
Comment=CodixOS Terminal Operating System
Exec=/opt/codixos/bin/codixos
Icon=/opt/codixos/share/icons/codixos.png
Terminal=true
Type=Application
Categories=System;Terminal;
EOF
    
    echo -e "${GREEN}Desktop entry created${NC}"
}

# Create symlink
create_symlink() {
    echo -e "${YELLOW}Creating symlink...${NC}"
    
    ln -sf /opt/codixos/bin/codixos /usr/local/bin/codixos
    
    echo -e "${GREEN}Symlink created: /usr/local/bin/codixos${NC}"
}

# Print success message
print_success() {
    echo ""
    echo -e "${GREEN}=====================================${NC}"
    echo -e "${GREEN}   CodixOS Installation Complete!${NC}"
    echo -e "${GREEN}=====================================${NC}"
    echo ""
    echo -e "You can now run CodixOS by typing:"
    echo -e "  ${CYAN}codixos${NC}"
    echo ""
    echo -e "Or run the terminal emulator:"
    echo -e "  ${CYAN}codix-term${NC}"
    echo ""
    echo -e "For help, type:"
    echo -e "  ${CYAN}codixos --help${NC}"
    echo ""
}

# Main function
main() {
    print_banner
    check_root
    install_dependencies
    install_codixos
    create_desktop_entry
    create_symlink
    print_success
}

# Run main function
main "$@"
