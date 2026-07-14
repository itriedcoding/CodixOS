#!/bin/bash
# CodixOS Build Script
# Builds all components of CodixOS

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
    echo -e "${GREEN}  CodixOS Build Script v1.0.0${NC}"
    echo ""
}

# Check dependencies
check_dependencies() {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    # Check for required tools
    for tool in gcc g++ make nasm ld; do
        if ! command -v $tool &> /dev/null; then
            echo -e "${RED}Error: $tool is not installed${NC}"
            echo "Please install build dependencies first."
            exit 1
        fi
    done
    
    echo -e "${GREEN}All dependencies found${NC}"
}

# Clean build directory
clean() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    make clean
    echo -e "${GREEN}Clean complete${NC}"
}

# Build kernel
build_kernel() {
    echo -e "${YELLOW}Building kernel...${NC}"
    make kernel
    echo -e "${GREEN}Kernel built successfully${NC}"
}

# Build shell
build_shell() {
    echo -e "${YELLOW}Building shell...${NC}"
    make shell
    echo -e "${GREEN}Shell built successfully${NC}"
}

# Build utilities
build_utils() {
    echo -e "${YELLOW}Building utilities...${NC}"
    make utils
    echo -e "${GREEN}Utilities built successfully${NC}"
}

# Build terminal
build_terminal() {
    echo -e "${YELLOW}Building terminal...${NC}"
    make terminal
    echo -e "${GREEN}Terminal built successfully${NC}"
}

# Build package manager
build_pkgmanager() {
    echo -e "${YELLOW}Building package manager...${NC}"
    make pkgmanager
    echo -e "${GREEN}Package manager built successfully${NC}"
}

# Build init system
build_init() {
    echo -e "${YELLOW}Building init system...${NC}"
    make init
    echo -e "${GREEN}Init system built successfully${NC}"
}

# Create initrd
create_initrd() {
    echo -e "${YELLOW}Creating initrd...${NC}"
    make initrd
    echo -e "${GREEN}Initrd created successfully${NC}"
}

# Create ISO
create_iso() {
    echo -e "${YELLOW}Creating ISO image...${NC}"
    make iso
    echo -e "${GREEN}ISO image created successfully${NC}"
}

# Print build summary
print_summary() {
    echo ""
    echo -e "${GREEN}=====================================${NC}"
    echo -e "${GREEN}   Build Complete!${NC}"
    echo -e "${GREEN}=====================================${NC}"
    echo ""
    echo -e "Build artifacts are in: ${CYAN}build/${NC}"
    echo ""
    echo -e "To create a bootable ISO, run:"
    echo -e "  ${CYAN}./scripts/build.sh iso${NC}"
    echo ""
    echo -e "To run in QEMU, run:"
    echo -e "  ${CYAN}./scripts/run-qemu.sh${NC}"
    echo ""
}

# Main function
main() {
    print_banner
    
    case "${1:-all}" in
        clean)
            clean
            ;;
        kernel)
            check_dependencies
            clean
            build_kernel
            ;;
        shell)
            check_dependencies
            build_shell
            ;;
        utils)
            check_dependencies
            build_utils
            ;;
        terminal)
            check_dependencies
            build_terminal
            ;;
        pkgmanager)
            check_dependencies
            build_pkgmanager
            ;;
        init)
            check_dependencies
            build_init
            ;;
        initrd)
            create_initrd
            ;;
        iso)
            create_iso
            ;;
        all)
            check_dependencies
            clean
            build_kernel
            build_shell
            build_utils
            build_terminal
            build_pkgmanager
            build_init
            create_initrd
            print_summary
            ;;
        *)
            echo -e "${RED}Unknown command: $1${NC}"
            echo "Usage: $0 {clean|kernel|shell|utils|terminal|pkgmanager|init|initrd|iso|all}"
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
