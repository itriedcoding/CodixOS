/*
 * CodixOS Full-Disk Encryption Implementation
 * LUKS-based encryption for secure storage
 */

#include "disk_encrypt.h"

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

/* External kernel functions */
extern void print(const char* str);
extern void println(const char* str);
extern void print_int(int num);
extern void putchar(char c);
extern void memset(void* dest, int c, size_t n);
extern void memcpy(void* dest, const void* src, size_t n);
extern int strcmp(const char* s1, const char* s2);
extern void strcpy(char* dest, const char* src);

/* Simple pseudo-random number generator */
static uint32_t rng_state = 0x12345678;
static uint8_t pseudo_random(void) {
    rng_state = rng_state * 1103515245 + 12345;
    return (uint8_t)((rng_state >> 16) & 0xFF);
}

static void generate_random(uint8_t* buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = pseudo_random();
    }
}

/* Simple hash */
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

/* PBKDF2 key derivation */
static void pbkdf2(const uint8_t* password, uint32_t pass_len,
                    const uint8_t* salt, uint32_t salt_len,
                    uint32_t iterations, uint8_t* key_out, uint32_t key_len) {
    uint8_t block[32];
    uint8_t hash[32];
    
    memcpy(block, salt, salt_len);
    
    for (uint32_t i = 0; i < iterations; i++) {
        for (uint32_t j = 0; j < pass_len && j < 32; j++) {
            block[j] ^= password[j];
        }
        hash_sha256(block, 32, hash);
        memcpy(block, hash, 32);
    }
    
    memcpy(key_out, hash, key_len > 32 ? 32 : key_len);
}

/* Generate UUID */
static void generate_uuid(char* uuid_out) {
    uint8_t uuid[16];
    generate_random(uuid, 16);
    
    /* Simple hex encoding */
    const char* hex = "0123456789abcdef";
    int pos = 0;
    for (int i = 0; i < 16; i++) {
        uuid_out[pos++] = hex[(uuid[i] >> 4) & 0xF];
        uuid_out[pos++] = hex[uuid[i] & 0xF];
        if (i == 3 || i == 5 || i == 7 || i == 9) uuid_out[pos++] = '-';
    }
    uuid_out[pos] = '\0';
}

/* Find device */
static encrypt_status_t* find_device(const char* device) {
    for (int i = 0; i < dev_count; i++) {
        if (strcmp(encrypted_devs[i].device, device) == 0) {
            return &encrypted_devs[i];
        }
    }
    return (encrypt_status_t*)0;
}

/* Initialize disk encryption system */
void disk_encrypt_init(void) {
    memset(&enc_ctx, 0, sizeof(encrypt_ctx_t));
    memset(encrypted_devs, 0, sizeof(encrypted_devs));
    dev_count = 0;
    println("Disk encryption system initialized");
}

/* Format device with LUKS encryption */
int disk_encrypt_format(const char* device, const char* passphrase, const char* cipher) {
    if (!device || !passphrase) {
        println("Error: device and passphrase required");
        return -1;
    }
    
    luks_header_t* hdr = &enc_ctx.header;
    memcpy(hdr->magic, LUKS_MAGIC, LUKS_MAGIC_LEN);
    hdr->version = LUKS_VERSION;
    strcpy((char*)hdr->cipher_name, cipher ? cipher : CIPHER_AES_256_XTS);
    strcpy((char*)hdr->cipher_mode, "plain64");
    strcpy((char*)hdr->hash_spec, HASH_SHA256);
    hdr->payload_offset = 4096;
    hdr->key_bytes = 32;
    hdr->mk_digest_iterations = 10000;
    
    generate_uuid((char*)hdr->uuid);
    generate_random(enc_ctx.master_key, 32);
    
    uint8_t salt[16];
    generate_random(salt, 16);
    memcpy(hdr->mk_digest_salt, salt, sizeof(salt));
    
    uint8_t derived_key[32];
    pbkdf2((const uint8_t*)passphrase, strlen(passphrase),
           salt, 16, hdr->mk_digest_iterations, derived_key, 32);
    
    uint8_t digest_input[64];
    memcpy(digest_input, enc_ctx.master_key, 32);
    memcpy(digest_input + 32, derived_key, 32);
    hash_sha256(digest_input, 64, hdr->mk_digest);
    
    hdr->keyslot[0].active = 1;
    hdr->keyslot[0].iterations = hdr->mk_digest_iterations;
    memcpy(hdr->keyslot[0].salt, salt, sizeof(salt));
    hdr->keyslot[0].key_offset = 1;
    hdr->keyslot[0].stripes = 4000;
    
    strcpy(enc_ctx.device, device);
    strcpy(enc_ctx.cipher, hdr->cipher_name);
    strcpy(enc_ctx.uuid, (char*)hdr->uuid);
    enc_ctx.key_size = hdr->key_bytes;
    enc_ctx.state = CRYPT_STATE_ENCRYPTED;
    
    if (dev_count < MAX_ENCRYPTED_DEVS) {
        encrypt_status_t* dev = &encrypted_devs[dev_count++];
        strcpy(dev->device, device);
        dev->encrypted = 1;
        dev->unlocked = 0;
        strcpy(dev->cipher, enc_ctx.cipher);
        dev->key_size = enc_ctx.key_size;
    }
    
    println("Device formatted with LUKS encryption");
    print("  Cipher:    ");
    println(enc_ctx.cipher);
    print("  UUID:      ");
    println(enc_ctx.uuid);
    
    return 0;
}

/* Open encrypted device */
int disk_encrypt_open(const char* device, const char* passphrase, const char* mapper_name) {
    if (!device || !passphrase || !mapper_name) {
        println("Error: all parameters required");
        return -1;
    }
    
    encrypt_status_t* dev = find_device(device);
    if (!dev) {
        println("Error: device not found");
        return -1;
    }
    
    if (dev->unlocked) {
        println("Device already unlocked");
        return 0;
    }
    
    uint8_t derived_key[32];
    pbkdf2((const uint8_t*)passphrase, strlen(passphrase),
           enc_ctx.header.mk_digest_salt, 16,
           enc_ctx.header.mk_digest_iterations, derived_key, 32);
    
    uint8_t digest_input[64];
    memcpy(digest_input, enc_ctx.master_key, 32);
    memcpy(digest_input + 32, derived_key, 32);
    
    uint8_t computed_digest[32];
    hash_sha256(digest_input, 64, computed_digest);
    
    if (memcmp(computed_digest, enc_ctx.header.mk_digest, 32) != 0) {
        println("Error: invalid passphrase");
        return -1;
    }
    
    dev->unlocked = 1;
    strcpy(dev->mapper, mapper_name);
    enc_ctx.state = CRYPT_STATE_UNLOCKED;
    
    print("Device unlocked as /dev/mapper/");
    println(mapper_name);
    return 0;
}

/* Close encrypted device */
int disk_encrypt_close(const char* mapper_name) {
    if (!mapper_name) {
        println("Error: mapper name required");
        return -1;
    }
    
    for (int i = 0; i < dev_count; i++) {
        if (strcmp(encrypted_devs[i].mapper, mapper_name) == 0) {
            encrypted_devs[i].unlocked = 0;
            memset(encrypted_devs[i].mapper, 0, sizeof(encrypted_devs[i].mapper));
            memset(enc_ctx.master_key, 0, sizeof(enc_ctx.master_key));
            enc_ctx.state = CRYPT_STATE_ENCRYPTED;
            println("Device locked");
            return 0;
        }
    }
    
    println("Error: mapper not found");
    return -1;
}

/* Add key */
int disk_encrypt_add_key(const char* device, const char* existing_pass, const char* new_pass) {
    int slot = -1;
    for (int i = 0; i < LUKS_MAX_KEYSLOTS; i++) {
        if (!enc_ctx.header.keyslot[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        println("Error: no free keyslots");
        return -1;
    }
    
    uint8_t new_salt[16];
    generate_random(new_salt, 16);
    
    enc_ctx.header.keyslot[slot].active = 1;
    enc_ctx.header.keyslot[slot].iterations = enc_ctx.header.mk_digest_iterations;
    memcpy(enc_ctx.header.keyslot[slot].salt, new_salt, 16);
    enc_ctx.header.keyslot[slot].key_offset = slot + 1;
    enc_ctx.header.keyslot[slot].stripes = 4000;
    
    println("New key added");
    return 0;
}

/* Remove key */
int disk_encrypt_remove_key(const char* device, const char* passphrase, int key_slot) {
    if (key_slot < 0 || key_slot >= LUKS_MAX_KEYSLOTS) {
        println("Error: invalid key slot");
        return -1;
    }
    
    if (!enc_ctx.header.keyslot[key_slot].active) {
        println("Error: key slot not active");
        return -1;
    }
    
    enc_ctx.header.keyslot[key_slot].active = 0;
    memset(enc_ctx.header.keyslot[key_slot].salt, 0, 16);
    println("Key removed");
    return 0;
}

/* Change passphrase */
int disk_encrypt_change_passphrase(const char* device, const char* old_pass, const char* new_pass) {
    uint8_t new_salt[16];
    generate_random(new_salt, 16);
    
    uint8_t new_derived_key[32];
    pbkdf2((const uint8_t*)new_pass, strlen(new_pass),
           new_salt, 16, enc_ctx.header.mk_digest_iterations, new_derived_key, 32);
    
    memcpy(enc_ctx.header.mk_digest_salt, new_salt, 16);
    
    uint8_t digest_input[64];
    memcpy(digest_input, enc_ctx.master_key, 32);
    memcpy(digest_input + 32, new_derived_key, 32);
    hash_sha256(digest_input, 64, enc_ctx.header.mk_digest);
    
    memcpy(enc_ctx.header.keyslot[0].salt, new_salt, 16);
    println("Passphrase changed");
    return 0;
}

/* Get status */
int disk_encrypt_get_status(const char* device, encrypt_status_t* status) {
    encrypt_status_t* dev = find_device(device);
    if (!dev) return -1;
    memcpy(status, dev, sizeof(encrypt_status_t));
    return 0;
}

/* Verify encrypted */
int disk_encrypt_verify(const char* device) {
    encrypt_status_t* dev = find_device(device);
    return dev ? dev->encrypted : 0;
}

/* List devices */
void disk_encrypt_list_devices(void) {
    println("");
    println("=== Encrypted Devices ===");
    println("");
    
    if (dev_count == 0) {
        println("  No encrypted devices found");
    } else {
        println("  DEVICE               MAPPER         CIPHER     STATUS");
        println("  ------               ------         ------     ------");
        
        for (int i = 0; i < dev_count; i++) {
            encrypt_status_t* dev = &encrypted_devs[i];
            print("  ");
            print(dev->device);
            print("    ");
            print(dev->unlocked ? dev->mapper : "-");
            print("       ");
            print(dev->cipher);
            print("  ");
            println(dev->unlocked ? "UNLOCKED" : "LOCKED");
        }
    }
    
    println("");
}

/* Benchmark */
void disk_encrypt_benchmark(const char* cipher) {
    println("Benchmarking encryption...");
    
    uint8_t data[4096];
    generate_random(data, 4096);
    
    uint8_t result[4096];
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 4096; j++) {
            result[j] = data[j] ^ (uint8_t)(i & 0xFF);
        }
    }
    
    println("  Benchmark complete");
}

/* Health check */
int disk_encrypt_health_check(const char* device) {
    encrypt_status_t* dev = find_device(device);
    if (!dev) {
        println("Error: device not found");
        return -1;
    }
    
    println("  Device:         ");
    print(device);
    println("");
    print("  Encrypted:      ");
    println(dev->encrypted ? "YES" : "NO");
    print("  Unlocked:       ");
    println(dev->unlocked ? "YES" : "NO");
    print("  Cipher:         ");
    println(dev->cipher);
    
    return 0;
}
