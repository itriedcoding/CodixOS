# CodixOS - Build System Makefile

CC = i686-elf-gcc
AS = nasm
LD = i686-elf-gcc

CFLAGS = -Wall -Wextra -O2 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -m32 -I.
ASFLAGS = -f elf32
LDFLAGS = -T kernel/linker.ld -nostdlib -ffreestanding -m32 -lgcc

BUILD_DIR = build

.PHONY: all clean

all: clean kernel iso

boot:
	@echo "[BUILD] Assembling boot stage..."
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) kernel/boot.asm -o $(BUILD_DIR)/boot.o

kernel: boot
	@echo "[BUILD] Building CodixOS Kernel..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c kernel/kernel.c -o $(BUILD_DIR)/kernel.o
	$(CC) $(CFLAGS) -c kernel/memory.c -o $(BUILD_DIR)/memory.o
	$(CC) $(CFLAGS) -c kernel/io.c -o $(BUILD_DIR)/io.o
	$(CC) $(CFLAGS) -c kernel/interrupt.c -o $(BUILD_DIR)/interrupt.o
	$(CC) $(CFLAGS) -c kernel/process.c -o $(BUILD_DIR)/process.o
	$(CC) $(CFLAGS) -c kernel/filesystem.c -o $(BUILD_DIR)/filesystem.o
	$(CC) $(CFLAGS) -c kernel/security/secure_boot.c -o $(BUILD_DIR)/secure_boot.o
	$(CC) $(CFLAGS) -c kernel/security/disk_encrypt.c -o $(BUILD_DIR)/disk_encrypt.o
	$(CC) $(CFLAGS) -c kernel/security/auth.c -o $(BUILD_DIR)/auth.o
	$(CC) $(CFLAGS) -c kernel/security/hardening.c -o $(BUILD_DIR)/hardening.o
	$(CC) $(CFLAGS) -c kernel/security/audit.c -o $(BUILD_DIR)/audit.o
	$(CC) $(CFLAGS) -c kernel/security/sandbox.c -o $(BUILD_DIR)/sandbox.o
	$(CC) $(CFLAGS) -c kernel/security/dataprotect.c -o $(BUILD_DIR)/dataprotect.o
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/codixos.bin $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/io.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/process.o $(BUILD_DIR)/filesystem.o $(BUILD_DIR)/secure_boot.o $(BUILD_DIR)/disk_encrypt.o $(BUILD_DIR)/auth.o $(BUILD_DIR)/hardening.o $(BUILD_DIR)/audit.o $(BUILD_DIR)/sandbox.o $(BUILD_DIR)/dataprotect.o
	@echo "[BUILD] Kernel compiled successfully!"

iso: kernel
	@echo "[BUILD] Creating Bootable ISO..."
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	@cp $(BUILD_DIR)/codixos.bin $(BUILD_DIR)/iso/boot/
	@cp boot/grub.cfg $(BUILD_DIR)/iso/boot/grub/
	grub-mkrescue -o $(BUILD_DIR)/codixos-1.0.0.iso $(BUILD_DIR)/iso
	@echo "[BUILD] ISO created: $(BUILD_DIR)/codixos-1.0.0.iso"

clean:
	@echo "[BUILD] Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "[BUILD] Clean complete"
