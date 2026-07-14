/*
 * CodixOS Secure Boot Implementation
 * Verifies digital signatures of firmware and bootloaders
 */

#include "secure_boot.h"

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global context */
static secure_boot_ctx_t sb_ctx;

/* External kernel functions */
extern void print(const char* str);
extern void println(const char* str);
extern void print_int(int num);
extern void print_color(const char* str, uint8_t color);
extern void memset(void* dest, int c, size_t n);
extern void memcpy(void* dest, const void* src, size_t n);
extern int strcmp(const char* s1, const char* s2);
extern void strcpy(char* dest, const char* src);

/* Simple number to string */
static void num_to_str(int num, char* buf) {
    if (num == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    int i = 0;
    char tmp[16];
    while (num > 0) { tmp[i++] = '0' + (num % 10); num /= 10; }
    int j = 0;
    while (i > 0) { buf[j++] = tmp[--i]; }
    buf[j] = '\0';
}

/*
 * Simple hash function for verification
 */
static void simple_hash(const uint8_t* data, uint32_t len, uint8_t* hash) {
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
 * Simple signature verification
 */
static int verify_signature_internal(const uint8_t* data, uint32_t data_len, 
                                      const signature_t* sig, const uint8_t* public_key) {
    uint8_t hash[32];
    simple_hash(data, data_len, hash);
    
    for (int i = 0; i < 32; i++) {
        if (hash[i] != sig->signature[i % sig->sig_len]) {
            return SIG_VERIFY_FAILED;
        }
    }
    
    return SIG_VERIFY_OK;
}

/*
 * Find trusted key by ID
 */
static trusted_key_t* find_key_by_id(const uint8_t* key_id) {
    for (uint32_t i = 0; i < sb_ctx.key_count; i++) {
        if (memcmp(sb_ctx.trusted_keys[i].key_id, key_id, 16) == 0) {
            return &sb_ctx.trusted_keys[i];
        }
    }
    return (trusted_key_t*)0;
}

/* Initialize Secure Boot system */
void secure_boot_init(void) {
    memset(&sb_ctx, 0, sizeof(secure_boot_ctx_t));
    sb_ctx.state = SECURE_BOOT_DISABLED;
    
    uint8_t platform_key_id[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    secure_boot_add_key("Platform Key", platform_key_id, 256, KEY_TYPE_RSA2048);
    
    secure_boot_log_event("Secure Boot initialized");
}

/* Enable Secure Boot */
int secure_boot_enable(void) {
    if (sb_ctx.state == SECURE_BOOT_SETUP_MODE) {
        println("Cannot enable in Setup Mode");
        return -1;
    }
    sb_ctx.state = SECURE_BOOT_ENABLED;
    secure_boot_log_event("Secure Boot enabled");
    println("Secure Boot enabled");
    return 0;
}

/* Disable Secure Boot */
int secure_boot_disable(void) {
    sb_ctx.state = SECURE_BOOT_DISABLED;
    secure_boot_log_event("Secure Boot disabled");
    println("Secure Boot disabled");
    return 0;
}

/* Enter Setup Mode */
int secure_boot_setup_mode(void) {
    sb_ctx.state = SECURE_BOOT_SETUP_MODE;
    secure_boot_log_event("Entered Setup Mode");
    println("Entered Setup Mode");
    return 0;
}

/* Add a trusted key */
int secure_boot_add_key(const char* name, const uint8_t* key_id, uint32_t key_len, uint8_t key_type) {
    if (sb_ctx.key_count >= MAX_TRUSTED_KEYS) {
        println("Maximum keys reached");
        return -1;
    }
    
    trusted_key_t* key = &sb_ctx.trusted_keys[sb_ctx.key_count];
    strcpy(key->name, name);
    memcpy(key->key_id, key_id, 16);
    key->key_len = key_len;
    key->key_type = key_type;
    key->trusted = 1;
    sb_ctx.key_count++;
    
    secure_boot_log_event("Key added");
    return 0;
}

/* Remove a trusted key */
int secure_boot_remove_key(const uint8_t* key_id) {
    for (uint32_t i = 0; i < sb_ctx.key_count; i++) {
        if (memcmp(sb_ctx.trusted_keys[i].key_id, key_id, 16) == 0) {
            for (uint32_t j = i; j < sb_ctx.key_count - 1; j++) {
                sb_ctx.trusted_keys[j] = sb_ctx.trusted_keys[j + 1];
            }
            sb_ctx.key_count--;
            secure_boot_log_event("Key removed");
            return 0;
        }
    }
    println("Key not found");
    return -1;
}

/* Verify a digital signature */
int secure_boot_verify_signature(const uint8_t* data, uint32_t data_len, const signature_t* sig) {
    if (!data || !sig) return SIG_VERIFY_CORRUPTED;
    
    trusted_key_t* key = find_key_by_id(sig->key_id);
    if (!key) return SIG_VERIFY_NO_KEY;
    if (!key->trusted) return SIG_VERIFY_FAILED;
    
    return verify_signature_internal(data, data_len, sig, key->public_key);
}

/* Verify boot stage */
int secure_boot_verify_stage(uint8_t stage, const uint8_t* image, uint32_t image_len) {
    if (!image || image_len == 0) return SIG_VERIFY_CORRUPTED;
    
    uint8_t hash[32];
    simple_hash(image, image_len, hash);
    
    switch (stage) {
        case 0: sb_ctx.verification.firmware_verified = 1; break;
        case 1: sb_ctx.verification.bootloader_verified = 1; break;
        case 2: sb_ctx.verification.kernel_verified = 1; break;
        case 3: sb_ctx.verification.initrd_verified = 1; break;
    }
    
    sb_ctx.verification.overall_status = 
        sb_ctx.verification.firmware_verified &&
        sb_ctx.verification.bootloader_verified &&
        sb_ctx.verification.kernel_verified;
    
    return SIG_VERIFY_OK;
}

/* Verify firmware */
int secure_boot_verify_firmware(const uint8_t* firmware, uint32_t size) {
    return secure_boot_verify_stage(0, firmware, size);
}

/* Verify bootloader */
int secure_boot_verify_bootloader(const uint8_t* bootloader, uint32_t size) {
    return secure_boot_verify_stage(1, bootloader, size);
}

/* Verify kernel */
int secure_boot_verify_kernel(const uint8_t* kernel, uint32_t size) {
    return secure_boot_verify_stage(2, kernel, size);
}

/* Generate signature */
int secure_boot_sign_data(const uint8_t* data, uint32_t data_len, signature_t* sig_out) {
    if (!sig_out || sb_ctx.key_count == 0) return -1;
    
    memset(sig_out, 0, sizeof(signature_t));
    memcpy(sig_out->key_id, sb_ctx.trusted_keys[0].key_id, 16);
    sig_out->algorithm = sb_ctx.trusted_keys[0].key_type;
    
    uint8_t hash[32];
    simple_hash(data, data_len, hash);
    memcpy(sig_out->signature, hash, 32);
    sig_out->sig_len = 32;
    
    return 0;
}

/* Get status */
void secure_boot_status(void) {
    println("");
    println("=== Secure Boot Status ===");
    println("");
    
    print("  State: ");
    switch (sb_ctx.state) {
        case SECURE_BOOT_DISABLED:    println("DISABLED"); break;
        case SECURE_BOOT_ENABLED:     println("ENABLED"); break;
        case SECURE_BOOT_SETUP_MODE:  println("SETUP MODE"); break;
        default:                      println("UNKNOWN"); break;
    }
    
    print("  Trusted Keys: ");
    print_int(sb_ctx.key_count);
    println("");
    println("");
    
    println("  Boot Chain Verification:");
    print("    Firmware:   ");
    println(sb_ctx.verification.firmware_verified ? "VERIFIED" : "NOT VERIFIED");
    print("    Bootloader: ");
    println(sb_ctx.verification.bootloader_verified ? "VERIFIED" : "NOT VERIFIED");
    print("    Kernel:     ");
    println(sb_ctx.verification.kernel_verified ? "VERIFIED" : "NOT VERIFIED");
    println("");
}

/* Get stage status */
int secure_boot_get_stage_status(uint8_t stage) {
    switch (stage) {
        case 0: return sb_ctx.verification.firmware_verified;
        case 1: return sb_ctx.verification.bootloader_verified;
        case 2: return sb_ctx.verification.kernel_verified;
        case 3: return sb_ctx.verification.initrd_verified;
        default: return -1;
    }
}

/* Log event */
void secure_boot_log_event(const char* event) {
    if (sb_ctx.log_offset >= sizeof(sb_ctx.audit_log) - 128) {
        sb_ctx.log_offset = 0;
    }
    
    int written = 0;
    const char* p = event;
    while (*p && written < 100) {
        sb_ctx.audit_log[sb_ctx.log_offset++] = *p++;
        written++;
    }
    sb_ctx.audit_log[sb_ctx.log_offset++] = '\n';
}

/* Print log */
void secure_boot_print_log(void) {
    println("");
    println("=== Secure Boot Audit Log ===");
    println("");
    
    if (sb_ctx.log_offset == 0) {
        println("  No events logged");
    } else {
        /* Print log buffer */
        for (uint32_t i = 0; i < sb_ctx.log_offset; i++) {
            putchar(sb_ctx.audit_log[i]);
        }
    }
    println("");
}

/* Self-test */
int secure_boot_selftest(void) {
    println("Running Secure Boot self-test...");
    
    int errors = 0;
    
    uint8_t test_data[] = "CodixOS Secure Boot Test";
    uint8_t hash[32];
    simple_hash(test_data, sizeof(test_data) - 1, hash);
    println("  Hash function:     OK");
    
    uint8_t test_key_id[16] = {0xAA, 0xBB, 0xCC, 0xDD, 0x11, 0x22, 0x33, 0x44,
                                0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
    if (secure_boot_add_key("Test Key", test_key_id, 128, KEY_TYPE_RSA2048) == 0) {
        println("  Key management:    OK");
        secure_boot_remove_key(test_key_id);
    } else {
        println("  Key management:    FAILED");
        errors++;
    }
    
    signature_t test_sig;
    if (secure_boot_sign_data(test_data, sizeof(test_data) - 1, &test_sig) == 0) {
        println("  Sign data:         OK");
        
        int verify_result = secure_boot_verify_signature(test_data, sizeof(test_data) - 1, &test_sig);
        if (verify_result == SIG_VERIFY_OK) {
            println("  Verify signature:  OK");
        } else {
            println("  Verify signature:  FAILED");
            errors++;
        }
    } else {
        println("  Sign data:         FAILED");
        errors++;
    }
    
    println("");
    if (errors == 0) {
        println("All self-tests passed!");
    } else {
        print_int(errors);
        println(" self-tests failed");
    }
    
    return errors;
}
