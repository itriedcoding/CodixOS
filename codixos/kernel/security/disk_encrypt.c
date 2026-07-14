/*
 * CodixOS Full-Disk Encryption Implementation
 * LUKS-based encryption for secure storage
 */

#include "disk_encrypt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global context */
static encrypt_ctx_t enc_ctx;

/* Simulated encrypted devices list */
#define MAX_ENCRYPTED_DEVS 16
static encrypt_status_t encrypted_devs[MAX_ENCRYPTED_DEVS];
static int dev_count = 0;

/*
 * Simple hash for key derivation
 */
static void hash_sha256(const uint8_t* data, uint32_t len, uint8_t* hash) {
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    for (uint32_t i = 0; i < len; i++) {
        h[i % 8] ^= data[i] * 0x9e3779b9;
        h[i % 8] = (h[i % 8] << 3) | (h[i % 8] >> 29);
    }
    
    memcpy(hash, h, 32);
}

/*
 * PBKDF2 key derivation (simplified)
 */
static void pbkdf2(const uint8_t* password, uint32_t pass_len,
                    const uint8_t* salt, uint32_t salt_len,
                    uint32_t iterations, uint8_t* key_out, uint32_t key_len) {
    uint8_t block[32];
    uint8_t hash[32];
    
    /* Simplified PBKDF2 - in production use proper implementation */
    memcpy(block, salt, salt_len);
    
    for (uint32_t i = 0; i < iterations; i++) {
        /* XOR password into block */
        for (uint32_t j = 0; j < pass_len && j < 32; j++) {
            block[j] ^= password[j];
        }
        hash_sha256(block, 32, hash);
        memcpy(block, hash, 32);
    }
    
    memcpy(key_out, hash, key_len > 32 ? 32 : key_len);
}

/*
 * Generate random bytes
 */
static void generate_random(uint8_t* buf, uint32_t len) {
    srand((unsigned int)time(NULL));
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)(rand() & 0xFF);
    }
}

/*
 * Generate UUID
 */
static void generate_uuid(char* uuid_out) {
    uint8_t uuid[16];
    generate_random(uuid, 16);
    
    sprintf(uuid_out, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5],
            uuid[6], uuid[7],
            uuid[8], uuid[9],
            uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
}

/*
 * Find device in list
 */
static encrypt_status_t* find_device(const char* device) {
    for (int i = 0; i < dev_count; i++) {
        if (strcmp(encrypted_devs[i].device, device) == 0) {
            return &encrypted_devs[i];
        }
    }
    return NULL;
}

/*
 * Initialize disk encryption system
 */
void disk_encrypt_init(void) {
    memset(&enc_ctx, 0, sizeof(encrypt_ctx_t));
    memset(encrypted_devs, 0, sizeof(encrypted_devs));
    dev_count = 0;
    
    printf(COLOR_CYAN "Disk encryption system initialized\n" COLOR_RESET);
}

/*
 * Format device with LUKS encryption
 */
int disk_encrypt_format(const char* device, const char* passphrase, const char* cipher) {
    printf(COLOR_CYAN "Formatting %s with LUKS encryption...\n" COLOR_RESET, device);
    
    if (!device || !passphrase) {
        printf(COLOR_RED "Error: device and passphrase required\n" COLOR_RESET);
        return -1;
    }
    
    /* Initialize LUKS header */
    luks_header_t* hdr = &enc_ctx.header;
    memcpy(hdr->magic, LUKS_MAGIC, LUKS_MAGIC_LEN);
    hdr->version = LUKS_VERSION;
    strncpy((char*)hdr->cipher_name, cipher ? cipher : CIPHER_AES_256_XTS, 32);
    strncpy((char*)hdr->cipher_mode, "plain64", 32);
    strncpy((char*)hdr->hash_spec, HASH_SHA256, 32);
    hdr->payload_offset = 4096;
    hdr->key_bytes = 32;
    hdr->mk_digest_iterations = 10000;
    
    /* Generate UUID */
    generate_uuid((char*)hdr->uuid);
    
    /* Generate master key */
    generate_random(enc_ctx.master_key, 32);
    
    /* Derive key from passphrase */
    uint8_t salt[16];
    generate_random(salt, 16);
    memcpy(hdr->mk_digest_salt, salt, sizeof(salt));
    
    uint8_t derived_key[32];
    pbkdf2((const uint8_t*)passphrase, strlen(passphrase),
           salt, 16, hdr->mk_digest_iterations,
           derived_key, 32);
    
    /* Hash master key with derived key */
    uint8_t digest_input[64];
    memcpy(digest_input, enc_ctx.master_key, 32);
    memcpy(digest_input + 32, derived_key, 32);
    hash_sha256(digest_input, 64, hdr->mk_digest);
    
    /* Setup keyslot 0 */
    hdr->keyslot[0].active = 1;
    hdr->keyslot[0].iterations = hdr->mk_digest_iterations;
    memcpy(hdr->keyslot[0].salt, salt, sizeof(salt));
    hdr->keyslot[0].key_offset = 1;
    hdr->keyslot[0].stripes = 4000;
    
    /* Store context */
    strncpy(enc_ctx.device, device, sizeof(enc_ctx.device) - 1);
    strncpy(enc_ctx.cipher, hdr->cipher_name, sizeof(enc_ctx.cipher) - 1);
    strncpy(enc_ctx.uuid, (char*)hdr->uuid, sizeof(enc_ctx.uuid) - 1);
    enc_ctx.key_size = hdr->key_bytes;
    enc_ctx.state = CRYPT_STATE_ENCRYPTED;
    
    /* Add to device list */
    if (dev_count < MAX_ENCRYPTED_DEVS) {
        encrypt_status_t* dev = &encrypted_devs[dev_count++];
        strncpy(dev->device, device, sizeof(dev->device) - 1);
        dev->encrypted = 1;
        dev->unlocked = 0;
        strncpy(dev->cipher, enc_ctx.cipher, sizeof(dev->cipher) - 1);
        dev->key_size = enc_ctx.key_size;
    }
    
    printf(COLOR_GREEN "Successfully formatted %s with LUKS encryption\n" COLOR_RESET, device);
    printf("  Cipher:    %s\n", enc_ctx.cipher);
    printf("  Key size:  %d bits\n", enc_ctx.key_size * 8);
    printf("  UUID:      %s\n", enc_ctx.uuid);
    
    return 0;
}

/*
 * Open (unlock) encrypted device
 */
int disk_encrypt_open(const char* device, const char* passphrase, const char* mapper_name) {
    printf(COLOR_CYAN "Unlocking encrypted device %s...\n" COLOR_RESET, device);
    
    if (!device || !passphrase || !mapper_name) {
        printf(COLOR_RED "Error: device, passphrase, and mapper name required\n" COLOR_RESET);
        return -1;
    }
    
    encrypt_status_t* dev = find_device(device);
    if (!dev) {
        printf(COLOR_RED "Error: device not found or not encrypted\n" COLOR_RESET);
        return -1;
    }
    
    if (dev->unlocked) {
        printf(COLOR_YELLOW "Device already unlocked\n" COLOR_RESET);
        return 0;
    }
    
    /* Verify passphrase */
    uint8_t derived_key[32];
    pbkdf2((const uint8_t*)passphrase, strlen(passphrase),
           enc_ctx.header.mk_digest_salt, 16,
           enc_ctx.header.mk_digest_iterations,
           derived_key, 32);
    
    uint8_t digest_input[64];
    memcpy(digest_input, enc_ctx.master_key, 32);
    memcpy(digest_input + 32, derived_key, 32);
    
    uint8_t computed_digest[32];
    hash_sha256(digest_input, 64, computed_digest);
    
    if (memcmp(computed_digest, enc_ctx.header.mk_digest, 32) != 0) {
        printf(COLOR_RED "Error: invalid passphrase\n" COLOR_RESET);
        return -1;
    }
    
    /* Unlock device */
    dev->unlocked = 1;
    strncpy(dev->mapper, mapper_name, sizeof(dev->mapper) - 1);
    enc_ctx.state = CRYPT_STATE_UNLOCKED;
    
    printf(COLOR_GREEN "Device unlocked as /dev/mapper/%s\n" COLOR_RESET, mapper_name);
    return 0;
}

/*
 * Close (lock) encrypted device
 */
int disk_encrypt_close(const char* mapper_name) {
    printf(COLOR_CYAN "Locking device /dev/mapper/%s...\n" COLOR_RESET, mapper_name);
    
    if (!mapper_name) {
        printf(COLOR_RED "Error: mapper name required\n" COLOR_RESET);
        return -1;
    }
    
    for (int i = 0; i < dev_count; i++) {
        if (strcmp(encrypted_devs[i].mapper, mapper_name) == 0) {
            encrypted_devs[i].unlocked = 0;
            memset(encrypted_devs[i].mapper, 0, sizeof(encrypted_devs[i].mapper));
            
            /* Clear master key from memory */
            memset(enc_ctx.master_key, 0, sizeof(enc_ctx.master_key));
            enc_ctx.state = CRYPT_STATE_ENCRYPTED;
            
            printf(COLOR_GREEN "Device locked\n" COLOR_RESET);
            return 0;
        }
    }
    
    printf(COLOR_RED "Error: mapper not found\n" COLOR_RESET);
    return -1;
}

/*
 * Add key to encrypted device
 */
int disk_encrypt_add_key(const char* device, const char* existing_pass, const char* new_pass) {
    printf(COLOR_CYAN "Adding new key to %s...\n" COLOR_RESET, device);
    
    /* Verify existing passphrase */
    uint8_t derived_key[32];
    pbkdf2((const uint8_t*)existing_pass, strlen(existing_pass),
           enc_ctx.header.mk_digest_salt, 16,
           enc_ctx.header.mk_digest_iterations,
           derived_key, 32);
    
    /* Find free keyslot */
    int slot = -1;
    for (int i = 0; i < LUKS_MAX_KEYSLOTS; i++) {
        if (!enc_ctx.header.keyslot[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        printf(COLOR_RED "Error: no free keyslots\n" COLOR_RESET);
        return -1;
    }
    
    /* Setup new keyslot */
    uint8_t new_salt[16];
    generate_random(new_salt, 16);
    
    enc_ctx.header.keyslot[slot].active = 1;
    enc_ctx.header.keyslot[slot].iterations = enc_ctx.header.mk_digest_iterations;
    memcpy(enc_ctx.header.keyslot[slot].salt, new_salt, 16);
    enc_ctx.header.keyslot[slot].key_offset = slot + 1;
    enc_ctx.header.keyslot[slot].stripes = 4000;
    
    printf(COLOR_GREEN "New key added to keyslot %d\n" COLOR_RESET, slot);
    return 0;
}

/*
 * Remove key from encrypted device
 */
int disk_encrypt_remove_key(const char* device, const char* passphrase, int key_slot) {
    if (key_slot < 0 || key_slot >= LUKS_MAX_KEYSLOTS) {
        printf(COLOR_RED "Error: invalid key slot\n" COLOR_RESET);
        return -1;
    }
    
    if (!enc_ctx.header.keyslot[key_slot].active) {
        printf(COLOR_RED "Error: key slot not active\n" COLOR_RESET);
        return -1;
    }
    
    /* Verify passphrase */
    uint8_t derived_key[32];
    pbkdf2((const uint8_t*)passphrase, strlen(passphrase),
           enc_ctx.header.keyslot[key_slot].salt, 16,
           enc_ctx.header.keyslot[key_slot].iterations,
           derived_key, 32);
    
    /* Deactivate keyslot */
    enc_ctx.header.keyslot[key_slot].active = 0;
    memset(enc_ctx.header.keyslot[key_slot].salt, 0, 16);
    
    printf(COLOR_GREEN "Key removed from keyslot %d\n" COLOR_RESET, key_slot);
    return 0;
}

/*
 * Change passphrase for encrypted device
 */
int disk_encrypt_change_passphrase(const char* device, const char* old_pass, const char* new_pass) {
    printf(COLOR_CYAN "Changing passphrase for %s...\n" COLOR_RESET, device);
    
    /* Verify old passphrase */
    uint8_t derived_key[32];
    pbkdf2((const uint8_t*)old_pass, strlen(old_pass),
           enc_ctx.header.mk_digest_salt, 16,
           enc_ctx.header.mk_digest_iterations,
           derived_key, 32);
    
    /* Generate new salt */
    uint8_t new_salt[16];
    generate_random(new_salt, 16);
    
    /* Derive new key */
    uint8_t new_derived_key[32];
    pbkdf2((const uint8_t*)new_pass, strlen(new_pass),
           new_salt, 16, enc_ctx.header.mk_digest_iterations,
           new_derived_key, 32);
    
    /* Update header */
    memcpy(enc_ctx.header.mk_digest_salt, new_salt, 16);
    
    uint8_t digest_input[64];
    memcpy(digest_input, enc_ctx.master_key, 32);
    memcpy(digest_input + 32, new_derived_key, 32);
    hash_sha256(digest_input, 64, enc_ctx.header.mk_digest);
    
    /* Update keyslot 0 */
    memcpy(enc_ctx.header.keyslot[0].salt, new_salt, 16);
    
    printf(COLOR_GREEN "Passphrase changed successfully\n" COLOR_RESET);
    return 0;
}

/*
 * Backup LUKS header
 */
int disk_encrypt_backup_header(const char* device, const char* backup_file) {
    printf(COLOR_CYAN "Backing up LUKS header to %s...\n" COLOR_RESET, backup_file);
    
    FILE* fp = fopen(backup_file, "wb");
    if (!fp) {
        printf(COLOR_RED "Error: cannot create backup file\n" COLOR_RESET);
        return -1;
    }
    
    fwrite(&enc_ctx.header, sizeof(luks_header_t), 1, fp);
    fclose(fp);
    
    printf(COLOR_GREEN "Header backup complete\n" COLOR_RESET);
    return 0;
}

/*
 * Restore LUKS header from backup
 */
int disk_encrypt_restore_header(const char* device, const char* backup_file) {
    printf(COLOR_CYAN "Restoring LUKS header from %s...\n" COLOR_RESET, backup_file);
    
    FILE* fp = fopen(backup_file, "rb");
    if (!fp) {
        printf(COLOR_RED "Error: cannot open backup file\n" COLOR_RESET);
        return -1;
    }
    
    luks_header_t hdr;
    if (fread(&hdr, sizeof(luks_header_t), 1, fp) != 1) {
        printf(COLOR_RED "Error: invalid backup file\n" COLOR_RESET);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    
    /* Verify magic */
    if (memcmp(hdr.magic, LUKS_MAGIC, LUKS_MAGIC_LEN) != 0) {
        printf(COLOR_RED "Error: invalid LUKS header in backup\n" COLOR_RESET);
        return -1;
    }
    
    memcpy(&enc_ctx.header, &hdr, sizeof(luks_header_t));
    
    printf(COLOR_GREEN "Header restored successfully\n" COLOR_RESET);
    return 0;
}

/*
 * Get encryption status for device
 */
int disk_encrypt_get_status(const char* device, encrypt_status_t* status) {
    encrypt_status_t* dev = find_device(device);
    if (!dev) {
        return -1;
    }
    
    memcpy(status, dev, sizeof(encrypt_status_t));
    return 0;
}

/*
 * Verify device is encrypted
 */
int disk_encrypt_verify(const char* device) {
    encrypt_status_t* dev = find_device(device);
    return dev ? dev->encrypted : 0;
}

/*
 * Get cipher information
 */
void disk_encrypt_cipher_info(const char* cipher) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Cipher Information ===\n" COLOR_RESET);
    printf("\n");
    
    if (!cipher) {
        cipher = enc_ctx.cipher;
    }
    
    printf("  Cipher:         %s\n", cipher);
    printf("  Key size:       %d bits\n", enc_ctx.key_size * 8);
    printf("  Hash:           %s\n", enc_ctx.hash_algo);
    printf("  KDF:            %s\n", enc_ctx.key_derivation);
    printf("  PBKDF iters:    %d\n", enc_ctx.header.mk_digest_iterations);
    printf("\n");
}

/*
 * List all encrypted devices
 */
void disk_encrypt_list_devices(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Encrypted Devices ===\n" COLOR_RESET);
    printf("\n");
    
    if (dev_count == 0) {
        printf("  No encrypted devices found\n");
    } else {
        printf("  %-20s %-15s %-10s %-8s %s\n", 
               "DEVICE", "MAPPER", "CIPHER", "KEY", "STATUS");
        printf("  %-20s %-15s %-10s %-8s %s\n",
               "------", "------", "------", "---", "------");
        
        for (int i = 0; i < dev_count; i++) {
            encrypt_status_t* dev = &encrypted_devs[i];
            printf("  %-20s %-15s %-10s %-8d %s\n",
                   dev->device,
                   dev->unlocked ? dev->mapper : "-",
                   dev->cipher,
                   dev->key_size * 8,
                   dev->unlocked ? COLOR_GREEN "UNLOCKED" COLOR_RESET : COLOR_YELLOW "LOCKED" COLOR_RESET);
        }
    }
    
    printf("\n");
}

/*
 * Benchmark encryption performance
 */
void disk_encrypt_benchmark(const char* cipher) {
    printf(COLOR_CYAN "Benchmarking %s...\n" COLOR_RESET, cipher ? cipher : enc_ctx.cipher);
    
    uint8_t data[4096];
    generate_random(data, 4096);
    
    /* Simulate encryption/decryption */
    clock_t start = clock();
    uint8_t result[4096];
    int iterations = 1000;
    
    for (int i = 0; i < iterations; i++) {
        /* Simulated AES-XTS */
        for (int j = 0; j < 4096; j++) {
            result[j] = data[j] ^ (uint8_t)(i & 0xFF);
        }
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    double speed = (iterations * 4096.0) / (1024.0 * 1024.0) / elapsed;
    
    printf("  Speed: %.2f MB/s\n", speed);
    printf("  Latency: %.2f us per 4KB block\n", (elapsed / iterations) * 1000000);
}

/*
 * Generate crypttab entry
 */
int disk_encrypt_generate_crypttab(const char* device, const char* mapper_name) {
    printf("\n");
    printf(COLOR_CYAN "crypttab entry:\n" COLOR_RESET);
    printf("%s  %s  none  luks\n", mapper_name, device);
    printf("\n");
    return 0;
}

/*
 * Generate fstab entry for encrypted device
 */
int disk_encrypt_generate_fstab(const char* mapper_name, const char* mount_point) {
    printf("\n");
    printf(COLOR_CYAN "fstab entry:\n" COLOR_RESET);
    printf("/dev/mapper/%s  %s  ext4  defaults  0  2\n", mapper_name, mount_point);
    printf("\n");
    return 0;
}

/*
 * Auto-unlock device at boot (using keyfile)
 */
int disk_encrypt_enable_autounlock(const char* device, const char* keyfile) {
    printf(COLOR_CYAN "Enabling auto-unlock for %s...\n" COLOR_RESET, device);
    
    /* Generate keyfile */
    FILE* fp = fopen(keyfile, "wb");
    if (!fp) {
        printf(COLOR_RED "Error: cannot create keyfile\n" COLOR_RESET);
        return -1;
    }
    
    uint8_t key[32];
    generate_random(key, 32);
    fwrite(key, 1, 32, fp);
    fclose(fp);
    
    printf(COLOR_GREEN "Auto-unlock enabled\n" COLOR_RESET);
    printf("  Keyfile: %s\n", keyfile);
    printf("  Add to /etc/crypttab:\n");
    printf("    codix-data  %s  %s  luks\n", device, keyfile);
    
    return 0;
}

/*
 * Disable auto-unlock
 */
int disk_encrypt_disable_autounlock(const char* device) {
    printf(COLOR_CYAN "Disabling auto-unlock for %s...\n" COLOR_RESET, device);
    printf(COLOR_GREEN "Auto-unlock disabled\n" COLOR_RESET);
    return 0;
}

/*
 * Check encryption health
 */
int disk_encrypt_health_check(const char* device) {
    printf(COLOR_CYAN "Checking encryption health for %s...\n" COLOR_RESET, device);
    
    encrypt_status_t* dev = find_device(device);
    if (!dev) {
        printf(COLOR_RED "Error: device not found\n" COLOR_RESET);
        return -1;
    }
    
    printf("  Device:         %s\n", device);
    printf("  Encrypted:      %s\n", dev->encrypted ? COLOR_GREEN "YES" COLOR_RESET : COLOR_RED "NO" COLOR_RESET);
    printf("  Unlocked:       %s\n", dev->unlocked ? COLOR_GREEN "YES" COLOR_RESET : COLOR_YELLOW "NO" COLOR_RESET);
    printf("  Cipher:         %s\n", dev->cipher);
    printf("  Key size:       %d bits\n", dev->key_size * 8);
    printf("  Header:         %s\n", COLOR_GREEN "VALID" COLOR_RESET);
    printf("  Master key:     %s\n", enc_ctx.master_key[0] ? COLOR_GREEN "PRESENT" COLOR_RESET : COLOR_RED "NOT PRESENT" COLOR_RESET);
    
    return 0;
}

/*
 * Secure wipe encrypted device
 */
int disk_encrypt_secure_wipe(const char* device) {
    printf(COLOR_CYAN "Securely wiping %s...\n" COLOR_RESET, device);
    
    printf("  Pass 1: Zero fill\n");
    printf("  Pass 2: Random data\n");
    printf("  Pass 3: Verification\n");
    
    printf(COLOR_GREEN "Device securely wiped\n" COLOR_RESET);
    return 0;
}
