#!/bin/bash
# CodixOS QEMU Run Script
# Runs CodixOS in QEMU emulator

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
KERNEL="$BUILD_DIR/codixos.bin"
INITRD="$BUILD_DIR/initrd.img"
ISO="$BUILD_DIR/codixos.iso"

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
    echo -e "${GREEN}  CodixOS QEMU Runner v1.0.0${NC}"
    echo ""
}

# Check if QEMU is installed
check_qemu() {
    if ! command -v qemu-system-i386 &> /dev/null && ! command -v qemu-system-x86_64 &> /dev/null; then
        echo -e "${RED}Error: QEMU is not installed${NC}"
        echo "Please install QEMU first:"
        echo "  Ubuntu/Debian: sudo apt-get install qemu-system-x86"
        echo "  Fedora: sudo dnf install qemu-system-x86"
        echo "  Arch: sudo pacman -S qemu-system-x86"
        exit 1
    fi
}

# Check if kernel exists
check_kernel() {
    if [ ! -f "$KERNEL" ]; then
        echo -e "${YELLOW}Kernel not found. Building...${NC}"
        ./scripts/build.sh kernel
    fi
}

# Run kernel directly
run_kernel() {
    echo -e "${YELLOW}Running CodixOS kernel...${NC}"
    
    if command -v qemu-system-x86_64 &> /dev/null; then
        qemu-system-x86_64 \
            -kernel "$KERNEL" \
            -append "console=ttyS0" \
            -nographic \
            -m 256M \
            -cpu qemu64 \
            -smp 1 \
            -no-reboot \
            -serial mon:stdio
    else
        qemu-system-i386 \
            -kernel "$KERNEL" \
            -append "console=ttyS0" \
            -nographic \
            -m 256M \
            -cpu qemu32 \
            -smp 1 \
            -no-reboot \
            -serial mon:stdio
    fi
}

# Run with initrd
run_initrd() {
    echo -e "${YELLOW}Running CodixOS with initrd...${NC}"
    
    if [ ! -f "$INITRD" ]; then
        echo -e "${YELLOW}Creating initrd...${NC}"
        make initrd
    fi
    
    if command -v qemu-system-x86_64 &> /dev/null; then
        qemu-system-x86_64 \
            -kernel "$KERNEL" \
            -initrd "$INITRD" \
            -append "console=ttyS0" \
            -nographic \
            -m 256M \
            -cpu qemu64 \
            -smp 1 \
            -no-reboot \
            -serial mon:stdio
    else
        qemu-system-i386 \
            -kernel "$KERNEL" \
            -initrd "$INITRD" \
            -append "console=ttyS0" \
            -nographic \
            -m 256M \
            -cpu qemu32 \
            -smp 1 \
            -no-reboot \
            -serial mon:stdio
    fi
}

# Run from ISO
run_iso() {
    echo -e "${YELLOW}Running CodixOS from ISO...${NC}"
    
    if [ ! -f "$ISO" ]; then
        echo -e "${YELLOW}Creating ISO...${NC}"
        make iso
    fi
    
    if command -v qemu-system-x86_64 &> /dev/null; then
        qemu-system-x86_64 \
            -cdrom "$ISO" \
            -boot d \
            -m 512M \
            -cpu qemu64 \
            -smp 1 \
            -nographic \
            -serial mon:stdio
    else
        qemu-system-i386 \
            -cdrom "$ISO" \
            -boot d \
            -m 512M \
            -cpu qemu32 \
            -smp 1 \
            -nographic \
            -serial mon:stdio
    fi
}

# Run with GUI
run_gui() {
    echo -e "${YELLOW}Running CodixOS with GUI...${NC}"
    
    if command -v qemu-system-x86_64 &> /dev/null; then
        qemu-system-x86_64 \
            -kernel "$KERNEL" \
            -append "console=ttyS0" \
            -m 256M \
            -cpu qemu64 \
            -smp 1 \
            -display sdl \
            -vga std \
            -no-reboot
    else
        qemu-system-i386 \
            -kernel "$KERNEL" \
            -append "console=ttyS0" \
            -m 256M \
            -cpu qemu32 \
            -smp 1 \
            -display sdl \
            -vga std \
            -no-reboot
    fi
}

# Print help
print_help() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  -k, --kernel    Run kernel directly"
    echo "  -i, --initrd    Run with initrd"
    echo "  -r, --iso       Run from ISO"
    echo "  -g, --gui       Run with GUI"
    echo "  -h, --help      Show this help"
    echo ""
}

# Main function
main() {
    print_banner
    
    case "${1:-kernel}" in
        -k|--kernel)
            check_qemu
            check_kernel
            run_kernel
            ;;
        -i|--initrd)
            check_qemu
            check_kernel
            run_initrd
            ;;
        -r|--iso)
            check_qemu
            run_iso
            ;;
        -g|--gui)
            check_qemu
            check_kernel
            run_gui
            ;;
        -h|--help)
            print_help
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_help
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
