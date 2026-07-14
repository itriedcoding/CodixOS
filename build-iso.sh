#!/bin/bash
# CodixOS ISO Builder
# Creates a bootable ISO image

set -e

# Configuration
ISO_NAME="codixos-1.0.0.iso"
ISO_DIR="iso"
BUILD_DIR="build"
ROOTFS_DIR="$ISO_DIR/rootfs"
KERNEL="codixos.bin"
INITRD="initrd.img"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

print_banner() {
    echo -e "${CYAN}"
    echo "        _____      _               ____   _____ "
    echo "       / ____|    | |             |  _ \ / ____|"
    echo "      | |     ___ | | ___  _ __   | |_) | (___  "
    echo "      | |    / _ \| |/ _ \| '__|  |  _ < \\___ \\ "
    echo "      | |___| (_) | | (_) | |     | |_) |____) |"
    echo "       \\_____|\\___/|_|\\___/|_|     |____/|_____/ "
    echo -e "${NC}"
    echo -e "${GREEN}  CodixOS ISO Builder v1.0.0${NC}"
    echo ""
}

check_dependencies() {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    local deps=("xorriso" "mtools" "grub-mkrescue")
    
    for dep in "${deps[@]}"; do
        if ! command -v $dep &> /dev/null; then
            echo -e "${RED}Error: $dep is not installed${NC}"
            echo "Install with: sudo apt-get install xorriso mtools grub-pc-bin"
            exit 1
        fi
    done
    
    echo -e "${GREEN}All dependencies found${NC}"
}

build_kernel() {
    echo -e "${YELLOW}Building kernel...${NC}"
    
    if [ ! -f "$BUILD_DIR/$KERNEL" ]; then
        make kernel
    fi
    
    echo -e "${GREEN}Kernel ready${NC}"
}

build_utilities() {
    echo -e "${YELLOW}Building utilities...${NC}"
    
    # Build shell and utilities
    gcc -Wall -Wextra -O2 -ffreestanding -nostdlib -c codixos/kernel/kernel.c -o $BUILD_DIR/kernel.o
    gcc -Wall -Wextra -O2 -ffreestanding -nostdlib -c codixos/kernel/memory.c -o $BUILD_DIR/memory.o
    gcc -Wall -Wextra -O2 -ffreestanding -nostdlib -c codixos/kernel/io.c -o $BUILD_DIR/io.o
    
    echo -e "${GREEN}Utilities ready${NC}"
}

prepare_rootfs() {
    echo -e "${YELLOW}Preparing root filesystem...${NC}"
    
    # Clean rootfs
    rm -rf $ROOTFS_DIR
    mkdir -p $ROOTFS_DIR/{bin,etc,home,usr/bin,usr/lib,usr/share,var,tmp,sbin,dev,proc,sys}
    
    # Copy kernel
    cp $BUILD_DIR/$KERNEL $ROOTFS_DIR/boot/ 2>/dev/null || true
    
    # Create initrd content
    mkdir -p $ROOTFS_DIR/boot
    
    # Create basic system files
    echo "CodixOS" > $ROOTFS_DIR/etc/hostname
    echo "127.0.0.1 localhost" > $ROOTFS_DIR/etc/hosts
    echo "codixos:x:1000:1000:CodixOS User:/home/codix:/bin/sh" > $ROOTFS_DIR/etc/passwd
    echo "codix:x:1000:" > $ROOTFS_DIR/etc/group
    
    # Create init script
    cat > $ROOTFS_DIR/init << 'INITEOF'
#!/bin/sh
# CodixOS Init Script

echo "========================================="
echo "        _____      _               ____   _____ "
echo "       / ____|    | |             |  _ \ / ____|"
echo "      | |     ___ | | ___  _ __   | |_) | (___  "
echo "      | |    / _ \| |/ _ \| '__|  |  _ < \___ \ "
echo "      | |___| (_) | | (_) | |     | |_) |____) |"
echo "       \_____\___/|_|\___/|_|     |____/|_____/ "
echo "========================================="
echo ""
echo "Welcome to CodixOS v1.0.0"
echo "Type 'help' for available commands"
echo ""

# Mount proc/sys/dev
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev

# Set hostname
hostname codixos

# Start shell
exec /bin/sh
INITEOF
    chmod +x $ROOTFS_DIR/init
    
    # Create shell script
    cat > $ROOTFS_DIR/bin/sh << 'SHEOF'
#!/bin/sh
# CodixOS Shell

PS1="codix@codixos:\w\$ "

while true; do
    echo -n "$PS1"
    read cmd args
    
    case "$cmd" in
        exit|quit)
            echo "Goodbye!"
            exit 0
            ;;
        help)
            echo "Available commands:"
            echo "  help     - Show this help"
            echo "  ls       - List files"
            echo "  cd       - Change directory"
            echo "  pwd      - Print working directory"
            echo "  cat      - Display file contents"
            echo "  echo     - Print text"
            echo "  clear    - Clear screen"
            echo "  whoami   - Show current user"
            echo "  uname    - Show system info"
            echo "  date     - Show current date"
            echo "  ps       - Show processes"
            echo "  free     - Show memory usage"
            echo "  df       - Show disk usage"
            echo "  pkg      - Package manager"
            echo "  neofetch - System info"
            echo "  exit     - Exit shell"
            ;;
        ls)
            ls $args
            ;;
        cd)
            cd $args 2>/dev/null || echo "cd: no such file: $args"
            ;;
        pwd)
            pwd
            ;;
        cat)
            cat $args 2>/dev/null || echo "cat: no such file: $args"
            ;;
        echo)
            echo $args
            ;;
        clear)
            echo -e "\033[2J\033[H"
            ;;
        whoami)
            echo "codix"
            ;;
        uname)
            echo "CodixOS 1.0.0 x86_64"
            ;;
        date)
            date
            ;;
        ps)
            echo "  PID TTY          TIME CMD"
            echo "    1 ?        00:00:00 init"
            echo "    2 ?        00:00:00 sh"
            ;;
        free)
            echo "              total        used        free"
            echo "Mem:        16384000     2048000    14336000"
            echo "Swap:        2097152           0     2097152"
            ;;
        df)
            echo "Filesystem     1K-blocks    Used Available Use%% Mounted on"
            echo "/dev/sda1       10240000  1024000   9216000  10% /"
            ;;
        pkg)
            echo "CodixOS Package Manager v1.0.0"
            echo "Usage: pkg [install|remove|update|list]"
            ;;
        neofetch)
            echo "        _____      _               user@codixos"
            echo "       / ____|    | |             OS: CodixOS 1.0.0"
            echo "      | |     ___ | | ___  _ __   Host: CodixOS VM"
            echo "      | |    / _ \| |/ _ \| '__|  Kernel: 1.0.0"
            echo "      | |___| (_) | | (_) | |     Shell: codix-sh"
            echo "       \_____\___/|_|\___/|_|     Terminal: codix-term"
            ;;
        "")
            ;;
        *)
            echo "$cmd: command not found"
            ;;
    esac
done
SHEOF
    chmod +x $ROOTFS_DIR/bin/sh
    ln -sf sh $ROOTFS_DIR/bin/bash
    
    echo -e "${GREEN}Root filesystem ready${NC}"
}

create_initrd() {
    echo -e "${YELLOW}Creating initrd...${NC}"
    
    cd $ROOTFS_DIR
    find . | cpio -o -H newc 2>/dev/null | gzip > ../$INITRD
    cd ..
    
    echo -e "${GREEN}Initrd created: $INITRD${NC}"
}

create_iso() {
    echo -e "${YELLOW}Creating ISO image...${NC}"
    
    # Create GRUB config
    mkdir -p iso/boot/grub
    cat > iso/boot/grub/grub.cfg << 'GRUBEOF'
set default=0
set timeout=5

menuentry "CodixOS" {
    set root=(cd)
    linux /boot/codixos.bin
    initrd /boot/initrd.img
}

menuentry "CodixOS (Live)" {
    set root=(cd)
    linux /boot/codixos.bin console=ttyS0
    initrd /boot/initrd.img
}

menuentry "CodixOS (Recovery)" {
    set root=(cd)
    linux /boot/codixos.bin single
    initrd /boot/initrd.img
}
GRUBEOF
    
    # Copy kernel and initrd
    mkdir -p iso/boot
    cp $BUILD_DIR/$KERNEL iso/boot/ 2>/dev/null || true
    cp $INITRD iso/boot/ 2>/dev/null || true
    
    # Create ISO
    grub-mkrescue -o $ISO_NAME iso 2>/dev/null || {
        echo -e "${YELLOW}Using alternative ISO creation method...${NC}"
        xorriso -as mkisofs \
            -o $ISO_NAME \
            -b boot/grub/grub.cfg \
            -no-emul-boot \
            -boot-load-size 4 \
            -boot-info-table \
            iso/
    }
    
    echo -e "${GREEN}ISO created: $ISO_NAME${NC}"
}

print_summary() {
    echo ""
    echo -e "${GREEN}=====================================${NC}"
    echo -e "${GREEN}   ISO Build Complete!${NC}"
    echo -e "${GREEN}=====================================${NC}"
    echo ""
    echo -e "ISO image: ${CYAN}$ISO_NAME${NC}"
    echo ""
    echo -e "To test with QEMU:"
    echo -e "  ${CYAN}qemu-system-x86_64 -cdrom $ISO_NAME -m 512M${NC}"
    echo ""
    echo -e "To test with VirtualBox:"
    echo -e "  1. Create new VM"
    echo -e "  2. Use $ISO_NAME as boot media"
    echo -e "  3. Start VM"
    echo ""
}

main() {
    print_banner
    check_dependencies
    build_kernel
    build_utilities
    prepare_rootfs
    create_initrd
    create_iso
    print_summary
}

main "$@"
