#!/bin/bash
# CodixOS Build Script for Linux/WSL
# Builds shell and utilities

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
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
    
    if ! command -v gcc &> /dev/null; then
        echo -e "${RED}Error: GCC is not installed${NC}"
        echo "Please install build tools: sudo apt-get install build-essential"
        exit 1
    fi
    
    echo -e "${GREEN}All dependencies found${NC}"
}

# Build shell
build_shell() {
    echo -e "${YELLOW}Building CodixOS Shell...${NC}"
    mkdir -p build
    
    gcc -Wall -Wextra -O2 -I shell/include -I utils/include -c shell/src/codix_shell.c -o build/codix_shell.o
    gcc -Wall -Wextra -O2 -I shell/include -I utils/include -c shell/src/commands.c -o build/commands.o
    gcc -Wall -Wextra -O2 -I shell/include -I utils/include -c shell/src/parser.c -o build/parser.o
    gcc -Wall -Wextra -O2 -I shell/include -I utils/include -c shell/src/builtins.c -o build/builtins.o
    gcc -o build/codix-sh build/codix_shell.o build/commands.o build/parser.o build/builtins.o
    
    echo -e "${GREEN}Shell built: build/codix-sh${NC}"
}

# Build utilities
build_utils() {
    echo -e "${YELLOW}Building System Utilities...${NC}"
    mkdir -p build/bin
    
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-ls utils/src/ls.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-cat utils/src/cat.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-cp utils/src/cp.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-mv utils/src/mv.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-rm utils/src/rm.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-mkdir utils/src/mkdir.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-echo utils/src/echo.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-grep utils/src/grep.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-chmod utils/src/chmod.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-ps utils/src/ps.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-kill utils/src/kill.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-df utils/src/df.c
    gcc -Wall -Wextra -O2 -I utils/include -o build/bin/codix-free utils/src/free.c
    
    echo -e "${GREEN}Utilities built in build/bin/${NC}"
}

# Build terminal
build_terminal() {
    echo -e "${YELLOW}Building Terminal Emulator...${NC}"
    mkdir -p build
    
    gcc -Wall -Wextra -O2 -I terminal/include -o build/codix-term terminal/src/terminal.c
    
    echo -e "${GREEN}Terminal built: build/codix-term${NC}"
}

# Build package manager
build_pkgmanager() {
    echo -e "${YELLOW}Building Package Manager...${NC}"
    mkdir -p build
    
    gcc -Wall -Wextra -O2 -I pkgmanager/include -o build/codix-pkg pkgmanager/src/codix_pkg.c
    
    echo -e "${GREEN}Package manager built: build/codix-pkg${NC}"
}

# Build desktop environment
build_desktop() {
    echo -e "${YELLOW}Building Desktop Environment...${NC}"
    
    # Check for SDL2
    if command -v sdl2-config &> /dev/null; then
        SDL2_CFLAGS=$(sdl2-config --cflags)
        SDL2_LIBS=$(sdl2-config --libs)
    else
        echo -e "${YELLOW}SDL2 not found, building without GUI support${NC}"
        SDL2_CFLAGS=""
        SDL2_LIBS=""
    fi
    
    mkdir -p build
    mkdir -p build/bin
    mkdir -p build/share
    mkdir -p build/share/codixos
    
    # Build window manager
    echo -e "  Building Window Manager..."
    gcc -Wall -Wextra -O2 -I desktop/wm/include $SDL2_CFLAGS -c desktop/wm/src/windowmanager.c -o build/windowmanager.o 2>/dev/null || true
    
    # Build desktop shell
    echo -e "  Building Desktop Shell..."
    gcc -Wall -Wextra -O2 -I desktop/shell/include $SDL2_CFLAGS -c desktop/shell/src/desktopshell.c -o build/desktopshell.o 2>/dev/null || true
    
    # Build GUI terminal
    echo -e "  Building GUI Terminal..."
    gcc -Wall -Wextra -O2 -I desktop/apps/terminal/include $SDL2_CFLAGS -c desktop/apps/terminal/src/guiterminal.c -o build/guiterminal.o 2>/dev/null || true
    
    # Build file manager
    echo -e "  Building File Manager..."
    gcc -Wall -Wextra -O2 -I desktop/apps/filemanager/include $SDL2_CFLAGS -c desktop/apps/filemanager/src/filemanager.c -o build/filemanager.o 2>/dev/null || true
    
    # Build text editor
    echo -e "  Building Text Editor..."
    gcc -Wall -Wextra -O2 -I desktop/apps/texteditor/include $SDL2_CFLAGS -c desktop/apps/texteditor/src/texteditor.c -o build/texteditor.o 2>/dev/null || true
    
    # Build settings
    echo -e "  Building Settings Panel..."
    gcc -Wall -Wextra -O2 -I desktop/apps/settings/include $SDL2_CFLAGS -c desktop/apps/settings/src/settings.c -o build/settings.o 2>/dev/null || true
    
    # Build system monitor
    echo -e "  Building System Monitor..."
    gcc -Wall -Wextra -O2 -I desktop/apps/monitor/include $SDL2_CFLAGS -c desktop/apps/monitor/src/systemmonitor.c -o build/systemmonitor.o 2>/dev/null || true
    
    # Build calculator
    echo -e "  Building Calculator..."
    gcc -Wall -Wextra -O2 -I desktop/apps/calculator/include $SDL2_CFLAGS -c desktop/apps/calculator/src/calculator.c -o build/calculator.o 2>/dev/null || true
    
    # Copy configuration files
    cp -r desktop/config/* build/share/codixos/ 2>/dev/null || true
    cp -r desktop/theme/* build/share/codixos/ 2>/dev/null || true
    
    echo -e "${GREEN}Desktop Environment components built${NC}"
    echo -e "${YELLOW}Note: Full GUI requires SDL2 and X11/Wayland${NC}"
}

# Clean build directory
clean() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf build
    echo -e "${GREEN}Clean complete${NC}"
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
    echo -e "To run CodixOS Shell:"
    echo -e "  ${CYAN}./build/codix-sh${NC}"
    echo ""
    echo -e "To install system-wide:"
    echo -e "  ${CYAN}sudo cp build/bin/* /usr/local/bin/${NC}"
    echo ""
}

# Main function
main() {
    print_banner
    
    case "${1:-all}" in
        clean)
            clean
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
        desktop)
            check_dependencies
            build_desktop
            ;;
        all)
            check_dependencies
            clean
            build_shell
            build_utils
            build_terminal
            build_pkgmanager
            build_desktop
            print_summary
            ;;
        *)
            echo -e "${RED}Unknown command: $1${NC}"
            echo "Usage: $0 {clean|shell|utils|terminal|pkgmanager|desktop|all}"
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
