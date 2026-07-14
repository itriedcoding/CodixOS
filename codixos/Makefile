# CodixOS - Lightweight Terminal Operating System
# Build System Makefile

CC = gcc
CXX = g++
NASM = nasm
LD = ld

CFLAGS = -Wall -Wextra -O2 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS = -T linker.ld -nostdlib

BUILD_DIR = build
SRC_DIR = .
KERNEL_SRC = kernel
SHELL_SRC = shell
UTILS_SRC = utils
INIT_SRC = init
TERM_SRC = terminal
PKG_SRC = pkgmanager

.PHONY: all clean install iso initrd shell utils terminal pkgmanager init

all: clean initrd shell utils terminal pkgmanager init kernel

kernel:
	@echo "[BUILD] Building CodixOS Kernel..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/kernel.c -o $(BUILD_DIR)/kernel.o
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/memory.c -o $(BUILD_DIR)/memory.o
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/io.c -o $(BUILD_DIR)/io.o
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/interrupt.c -o $(BUILD_DIR)/interrupt.o
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/process.c -o $(BUILD_DIR)/process.o
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/filesystem.c -o $(BUILD_DIR)/filesystem.o
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/security/secure_boot.c -o $(BUILD_DIR)/secure_boot.o
	$(CC) $(CFLAGS) -c $(KERNEL_SRC)/security/disk_encrypt.c -o $(BUILD_DIR)/disk_encrypt.o
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/codixos.bin $(BUILD_DIR)/*.o
	@echo "[BUILD] Kernel compiled successfully!"

shell:
	@echo "[BUILD] Building CodixOS Shell..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SHELL_SRC)/src/codix_shell.c -o $(BUILD_DIR)/codix_shell.o
	$(CC) $(CFLAGS) -c $(SHELL_SRC)/src/commands.c -o $(BUILD_DIR)/commands.o
	$(CC) $(CFLAGS) -c $(SHELL_SRC)/src/parser.c -o $(BUILD_DIR)/parser.o
	$(CC) $(CFLAGS) -c $(SHELL_SRC)/src/builtins.c -o $(BUILD_DIR)/builtins.o

utils:
	@echo "[BUILD] Building System Utilities..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/ls.c -o $(BUILD_DIR)/ls.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/cat.c -o $(BUILD_DIR)/cat.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/cp.c -o $(BUILD_DIR)/cp.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/mv.c -o $(BUILD_DIR)/mv.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/rm.c -o $(BUILD_DIR)/rm.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/mkdir.c -o $(BUILD_DIR)/mkdir.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/echo.c -o $(BUILD_DIR)/echo.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/grep.c -o $(BUILD_DIR)/grep.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/chmod.c -o $(BUILD_DIR)/chmod.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/ps.c -o $(BUILD_DIR)/ps.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/kill.c -o $(BUILD_DIR)/kill.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/df.c -o $(BUILD_DIR)/df.o
	$(CC) $(CFLAGS) -c $(UTILS_SRC)/src/free.c -o $(BUILD_DIR)/free.o

terminal:
	@echo "[BUILD] Building Terminal Emulator..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(TERM_SRC)/src/terminal.c -o $(BUILD_DIR)/terminal.o

pkgmanager:
	@echo "[BUILD] Building Package Manager..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(PKG_SRC)/src/codix_pkg.c -o $(BUILD_DIR)/codix_pkg.o

init:
	@echo "[BUILD] Building Init System..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(INIT_SRC)/src/init.c -o $(BUILD_DIR)/init.o
	$(CC) $(CFLAGS) -c $(INIT_SRC)/src/service.c -o $(BUILD_DIR)/service.o

initrd:
	@echo "[BUILD] Creating Initial RAM Disk..."
	@mkdir -p $(BUILD_DIR)/initrd
	@cp filesystem/* $(BUILD_DIR)/initrd/ -r
	@echo "CodixOS initrd created"

iso:
	@echo "[BUILD] Creating Bootable ISO..."
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	@cp $(BUILD_DIR)/codixos.bin $(BUILD_DIR)/iso/boot/
	@cp boot/grub.cfg $(BUILD_DIR)/iso/boot/grub/
	@grub-mkrescue -o $(BUILD_DIR)/codixos.iso $(BUILD_DIR)/iso
	@echo "[BUILD] ISO created: $(BUILD_DIR)/codixos.iso"

clean:
	@echo "[BUILD] Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "[BUILD] Clean complete"
