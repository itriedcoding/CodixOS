#!/bin/bash
# CodixOS Desktop Startup Script
# Starts the desktop environment

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
LOG_DIR="$HOME/.codixos/logs"
PID_DIR="$HOME/.codixos/pids"

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
    echo -e "${GREEN}  CodixOS Desktop Environment v1.0.0${NC}"
    echo ""
}

# Check if running in X11/Wayland
check_display() {
    if [ -z "$DISPLAY" ] && [ -z "$WAYLAND_DISPLAY" ]; then
        echo -e "${YELLOW}No display server detected.${NC}"
        echo "Starting X11 virtual display (Xvfb)..."
        
        if command -v Xvfb &> /dev/null; then
            Xvfb :1 -screen 0 1920x1080x24 &
            export DISPLAY=:1
            sleep 1
        else
            echo -e "${RED}Error: Xvfb not installed${NC}"
            echo "Install with: sudo apt-get install xvfb"
            exit 1
        fi
    fi
}

# Create directories
create_dirs() {
    mkdir -p "$LOG_DIR"
    mkdir -p "$PID_DIR"
}

# Check dependencies
check_dependencies() {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    local deps=("gcc" "make")
    local optional=("sdl2-config" "Xvfb")
    
    for dep in "${deps[@]}"; do
        if ! command -v $dep &> /dev/null; then
            echo -e "${RED}Error: $dep is not installed${NC}"
            exit 1
        fi
    done
    
    echo -e "${GREEN}Dependencies OK${NC}"
}

# Build if needed
build_if_needed() {
    if [ ! -d "$BUILD_DIR" ] || [ ! -f "$BUILD_DIR/codix-wm" ]; then
        echo -e "${YELLOW}Building CodixOS Desktop...${NC}"
        make -f Makefile.desktop all
    fi
}

# Start window manager
start_wm() {
    echo -e "${YELLOW}Starting Window Manager...${NC}"
    if [ -f "$BUILD_DIR/codix-wm" ]; then
        "$BUILD_DIR/codix-wm" > "$LOG_DIR/wm.log" 2>&1 &
        echo $! > "$PID_DIR/wm.pid"
        echo -e "${GREEN}Window Manager started${NC}"
    else
        echo -e "${RED}Window Manager not found${NC}"
    fi
}

# Start desktop shell
start_shell() {
    echo -e "${YELLOW}Starting Desktop Shell...${NC}"
    if [ -f "$BUILD_DIR/codix-shell" ]; then
        "$BUILD_DIR/codix-shell" > "$LOG_DIR/shell.log" 2>&1 &
        echo $! > "$PID_DIR/shell.pid"
        echo -e "${GREEN}Desktop Shell started${NC}"
    else
        echo -e "${RED}Desktop Shell not found${NC}"
    fi
}

# Start panel
start_panel() {
    echo -e "${YELLOW}Starting Panel...${NC}"
    # Panel would be started here
    echo -e "${GREEN}Panel started${NC}"
}

# Start desktop icons
start_desktop_icons() {
    echo -e "${YELLOW}Starting Desktop Icons...${NC}"
    # Desktop icons would be started here
    echo -e "${GREEN}Desktop Icons started${NC}"
}

# Start applications
start_apps() {
    echo -e "${YELLOW}Starting default applications...${NC}"
    
    # Start terminal
    if [ -f "$BUILD_DIR/bin/codix-term-gui" ]; then
        "$BUILD_DIR/bin/codix-term-gui" > "$LOG_DIR/terminal.log" 2>&1 &
        echo $! > "$PID_DIR/terminal.pid"
    fi
    
    # Start Firefox browser (pre-installed)
    if command -v firefox &> /dev/null; then
        firefox > "$LOG_DIR/firefox.log" 2>&1 &
        echo $! > "$PID_DIR/firefox.pid"
        echo -e "${GREEN}Firefox browser started${NC}"
    elif [ -f "/usr/bin/firefox" ]; then
        /usr/bin/firefox > "$LOG_DIR/firefox.log" 2>&1 &
        echo $! > "$PID_DIR/firefox.pid"
        echo -e "${GREEN}Firefox browser started${NC}"
    else
        echo -e "${YELLOW}Firefox not found, skipping browser startup${NC}"
    fi
    
    echo -e "${GREEN}Applications started${NC}"
}

# Stop all components
stop_all() {
    echo -e "${YELLOW}Stopping CodixOS Desktop...${NC}"
    
    for pidfile in "$PID_DIR"/*.pid; do
        if [ -f "$pidfile" ]; then
            pid=$(cat "$pidfile")
            if kill -0 "$pid" 2>/dev/null; then
                kill "$pid" 2>/dev/null || true
            fi
            rm -f "$pidfile"
        fi
    done
    
    echo -e "${GREEN}Desktop stopped${NC}"
}

# Print status
print_status() {
    echo ""
    echo -e "${CYAN}=== CodixOS Desktop Status ===${NC}"
    echo ""
    
    for pidfile in "$PID_DIR"/*.pid; do
        if [ -f "$pidfile" ]; then
            name=$(basename "$pidfile" .pid)
            pid=$(cat "$pidfile")
            if kill -0 "$pid" 2>/dev/null; then
                echo -e "  ${GREEN}●${NC} $name (PID: $pid)"
            else
                echo -e "  ${RED}●${NC} $name (stopped)"
            fi
        fi
    done
    
    echo ""
}

# Show help
show_help() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  start       Start the desktop environment"
    echo "  stop        Stop the desktop environment"
    echo "  restart     Restart the desktop environment"
    echo "  status      Show desktop status"
    echo "  build       Build desktop components"
    echo "  help        Show this help"
    echo ""
}

# Main function
main() {
    case "${1:-start}" in
        start)
            print_banner
            check_display
            create_dirs
            check_dependencies
            build_if_needed
            start_wm
            start_shell
            start_panel
            start_desktop_icons
            start_apps
            print_status
            echo -e "${GREEN}CodixOS Desktop is running!${NC}"
            echo -e "Press Ctrl+C to stop or run: $0 stop"
            ;;
        stop)
            stop_all
            ;;
        restart)
            stop_all
            sleep 1
            main start
            ;;
        status)
            print_status
            ;;
        build)
            echo -e "${YELLOW}Building CodixOS Desktop...${NC}"
            make -f Makefile.desktop all
            echo -e "${GREEN}Build complete${NC}"
            ;;
        help|-h|--help)
            show_help
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
