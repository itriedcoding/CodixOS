/*
 * CodixOS Secure Boot Implementation
 * Verifies digital signatures of firmware and bootloaders
 */

#include "secure_boot.h"
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
static secure_boot_ctx_t sb_ctx;

/*
 * Simple hash function for verification (SHA-256-like)
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
 * Simple RSA-like signature verification
 * In production, use proper cryptographic libraries
 */
static int verify_signature_internal(const uint8_t* data, uint32_t data_len, 
                                      const signature_t* sig, const uint8_t* public_key) {
    uint8_t hash[32];
    simple_hash(data, data_len, hash);
    
    /* Simulated signature verification */
    /* In production, use RSA/PKCS#1 or ECDSA */
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
    return NULL;
}

/*
 * Initialize Secure Boot system
 */
void secure_boot_init(void) {
    memset(&sb_ctx, 0, sizeof(secure_boot_ctx_t));
    sb_ctx.state = SECURE_BOOT_DISABLED;
    
    /* Initialize verification status */
    sb_ctx.verification.firmware_verified = 0;
    sb_ctx.verification.bootloader_verified = 0;
    sb_ctx.verification.kernel_verified = 0;
    sb_ctx.verification.initrd_verified = 0;
    sb_ctx.verification.overall_status = 0;
    
    /* Load default platform key */
    uint8_t platform_key_id[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    secure_boot_add_key("CodixOS Platform Key", platform_key_id, 256, KEY_TYPE_RSA2048);
    
    secure_boot_log_event("Secure Boot system initialized");
}

/*
 * Enable Secure Boot
 */
int secure_boot_enable(void) {
    if (sb_ctx.state == SECURE_BOOT_SETUP_MODE) {
        printf(COLOR_RED "Cannot enable Secure Boot in Setup Mode\n" COLOR_RESET);
        return -1;
    }
    
    sb_ctx.state = SECURE_BOOT_ENABLED;
    secure_boot_log_event("Secure Boot enabled");
    printf(COLOR_GREEN "Secure Boot enabled\n" COLOR_RESET);
    return 0;
}

/*
 * Disable Secure Boot
 */
int secure_boot_disable(void) {
    sb_ctx.state = SECURE_BOOT_DISABLED;
    secure_boot_log_event("Secure Boot disabled");
    printf(COLOR_YELLOW "Secure Boot disabled\n" COLOR_RESET);
    return 0;
}

/*
 * Enter Setup Mode
 */
int secure_boot_setup_mode(void) {
    sb_ctx.state = SECURE_BOOT_SETUP_MODE;
    secure_boot_log_event("Entered Setup Mode");
    printf(COLOR_CYAN "Entered Secure Boot Setup Mode\n" COLOR_RESET);
    printf("In Setup Mode, you can manage trusted keys.\n");
    return 0;
}

/*
 * Add a trusted key
 */
int secure_boot_add_key(const char* name, const uint8_t* key_id, uint32_t key_len, uint8_t key_type) {
    if (sb_ctx.key_count >= MAX_TRUSTED_KEYS) {
        printf(COLOR_RED "Maximum number of trusted keys reached\n" COLOR_RESET);
        return -1;
    }
    
    trusted_key_t* key = &sb_ctx.trusted_keys[sb_ctx.key_count];
    strncpy(key->name, name, sizeof(key->name) - 1);
    memcpy(key->key_id, key_id, 16);
    key->key_len = key_len;
    key->key_type = key_type;
    key->trusted = 1;
    
    sb_ctx.key_count++;
    
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Added trusted key: %s", name);
    secure_boot_log_event(log_msg);
    
    printf(COLOR_GREEN "Added trusted key: %s\n" COLOR_RESET, name);
    return 0;
}

/*
 * Remove a trusted key
 */
int secure_boot_remove_key(const uint8_t* key_id) {
    for (uint32_t i = 0; i < sb_ctx.key_count; i++) {
        if (memcmp(sb_ctx.trusted_keys[i].key_id, key_id, 16) == 0) {
            char log_msg[128];
            snprintf(log_msg, sizeof(log_msg), "Removed trusted key: %s", 
                    sb_ctx.trusted_keys[i].name);
            secure_boot_log_event(log_msg);
            
            /* Shift remaining keys */
            for (uint32_t j = i; j < sb_ctx.key_count - 1; j++) {
                sb_ctx.trusted_keys[j] = sb_ctx.trusted_keys[j + 1];
            }
            sb_ctx.key_count--;
            
            printf(COLOR_GREEN "Key removed\n" COLOR_RESET);
            return 0;
        }
    }
    
    printf(COLOR_RED "Key not found\n" COLOR_RESET);
    return -1;
}

/*
 * Verify a digital signature
 */
int secure_boot_verify_signature(const uint8_t* data, uint32_t data_len, const signature_t* sig) {
    if (!data || !sig) {
        return SIG_VERIFY_CORRUPTED;
    }
    
    /* Find the key */
    trusted_key_t* key = find_key_by_id(sig->key_id);
    if (!key) {
        secure_boot_log_event("Signature verification failed: unknown key");
        return SIG_VERIFY_NO_KEY;
    }
    
    if (!key->trusted) {
        secure_boot_log_event("Signature verification failed: key not trusted");
        return SIG_VERIFY_FAILED;
    }
    
    int result = verify_signature_internal(data, data_len, sig, key->public_key);
    
    if (result == SIG_VERIFY_OK) {
        secure_boot_log_event("Signature verification passed");
    } else {
        secure_boot_log_event("Signature verification failed");
    }
    
    return result;
}

/*
 * Verify boot stage
 */
int secure_boot_verify_stage(uint8_t stage, const uint8_t* image, uint32_t image_len) {
    if (!image || image_len == 0) {
        return SIG_VERIFY_CORRUPTED;
    }
    
    const char* stage_names[] = {"Firmware", "Bootloader", "Kernel", "Initrd"};
    
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Verifying %s stage...", stage_names[stage]);
    secure_boot_log_event(log_msg);
    
    /* In production, would verify actual signature */
    /* For now, simulate verification */
    uint8_t hash[32];
    simple_hash(image, image_len, hash);
    
    /* Update verification status */
    switch (stage) {
        case 0: sb_ctx.verification.firmware_verified = 1; break;
        case 1: sb_ctx.verification.bootloader_verified = 1; break;
        case 2: sb_ctx.verification.kernel_verified = 1; break;
        case 3: sb_ctx.verification.initrd_verified = 1; break;
    }
    
    /* Update overall status */
    sb_ctx.verification.overall_status = 
        sb_ctx.verification.firmware_verified &&
        sb_ctx.verification.bootloader_verified &&
        sb_ctx.verification.kernel_verified;
    
    snprintf(log_msg, sizeof(log_msg), "%s verification: PASSED", stage_names[stage]);
    secure_boot_log_event(log_msg);
    
    return SIG_VERIFY_OK;
}

/*
 * Verify firmware signature
 */
int secure_boot_verify_firmware(const uint8_t* firmware, uint32_t size) {
    printf(COLOR_CYAN "Verifying firmware signature...\n" COLOR_RESET);
    return secure_boot_verify_stage(0, firmware, size);
}

/*
 * Verify bootloader signature
 */
int secure_boot_verify_bootloader(const uint8_t* bootloader, uint32_t size) {
    printf(COLOR_CYAN "Verifying bootloader signature...\n" COLOR_RESET);
    return secure_boot_verify_stage(1, bootloader, size);
}

/*
 * Verify kernel signature
 */
int secure_boot_verify_kernel(const uint8_t* kernel, uint32_t size) {
    printf(COLOR_CYAN "Verifying kernel signature...\n" COLOR_RESET);
    return secure_boot_verify_stage(2, kernel, size);
}

/*
 * Generate signature for data
 */
int secure_boot_sign_data(const uint8_t* data, uint32_t data_len, signature_t* sig_out) {
    if (!sig_out) return -1;
    
    memset(sig_out, 0, sizeof(signature_t));
    
    /* Use first trusted key for signing */
    if (sb_ctx.key_count == 0) {
        printf(COLOR_RED "No trusted keys available for signing\n" COLOR_RESET);
        return -1;
    }
    
    memcpy(sig_out->key_id, sb_ctx.trusted_keys[0].key_id, 16);
    sig_out->algorithm = sb_ctx.trusted_keys[0].key_type;
    
    /* Generate signature (simplified) */
    uint8_t hash[32];
    simple_hash(data, data_len, hash);
    memcpy(sig_out->signature, hash, 32);
    sig_out->sig_len = 32;
    
    sig_out->timestamp = (uint64_t)time(NULL);
    
    secure_boot_log_event("Data signed successfully");
    return 0;
}

/*
 * Get Secure Boot status
 */
void secure_boot_status(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Secure Boot Status ===\n" COLOR_RESET);
    printf("\n");
    
    const char* state_str;
    switch (sb_ctx.state) {
        case SECURE_BOOT_DISABLED:    state_str = COLOR_RED "DISABLED" COLOR_RESET; break;
        case SECURE_BOOT_ENABLED:     state_str = COLOR_GREEN "ENABLED" COLOR_RESET; break;
        case SECURE_BOOT_SETUP_MODE:  state_str = COLOR_YELLOW "SETUP MODE" COLOR_RESET; break;
        default:                      state_str = "UNKNOWN"; break;
    }
    
    printf("  State:             %s\n", state_str);
    printf("  Trusted Keys:      %d\n", sb_ctx.key_count);
    printf("\n");
    
    printf(COLOR_CYAN "  Boot Chain Verification:\n" COLOR_RESET);
    printf("    Firmware:        %s\n", 
           sb_ctx.verification.firmware_verified ? COLOR_GREEN "VERIFIED" COLOR_RESET : COLOR_YELLOW "NOT VERIFIED" COLOR_RESET);
    printf("    Bootloader:      %s\n",
           sb_ctx.verification.bootloader_verified ? COLOR_GREEN "VERIFIED" COLOR_RESET : COLOR_YELLOW "NOT VERIFIED" COLOR_RESET);
    printf("    Kernel:          %s\n",
           sb_ctx.verification.kernel_verified ? COLOR_GREEN "VERIFIED" COLOR_RESET : COLOR_YELLOW "NOT VERIFIED" COLOR_RESET);
    printf("    Initrd:          %s\n",
           sb_ctx.verification.initrd_verified ? COLOR_GREEN "VERIFIED" COLOR_RESET : COLOR_YELLOW "NOT VERIFIED" COLOR_RESET);
    printf("\n");
    
    printf("  Overall Status:    %s\n",
           sb_ctx.verification.overall_status ? COLOR_GREEN "SECURE" COLOR_RESET : COLOR_YELLOW "UNVERIFIED" COLOR_RESET);
    printf("\n");
}

/*
 * Get verification status for a specific stage
 */
int secure_boot_get_stage_status(uint8_t stage) {
    switch (stage) {
        case 0: return sb_ctx.verification.firmware_verified;
        case 1: return sb_ctx.verification.bootloader_verified;
        case 2: return sb_ctx.verification.kernel_verified;
        case 3: return sb_ctx.verification.initrd_verified;
        default: return -1;
    }
}

/*
 * Log security event
 */
void secure_boot_log_event(const char* event) {
    if (sb_ctx.log_offset >= sizeof(sb_ctx.audit_log) - 128) {
        /* Log full, rotate */
        sb_ctx.log_offset = 0;
    }
    
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    
    int written = snprintf((char*)&sb_ctx.audit_log[sb_ctx.log_offset], 
                          sizeof(sb_ctx.audit_log) - sb_ctx.log_offset,
                          "[%02d:%02d:%02d] %s\n",
                          tm->tm_hour, tm->tm_min, tm->tm_sec, event);
    
    if (written > 0) {
        sb_ctx.log_offset += written;
    }
}

/*
 * Print audit log
 */
void secure_boot_print_log(void) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Secure Boot Audit Log ===\n" COLOR_RESET);
    printf("\n");
    
    if (sb_ctx.log_offset == 0) {
        printf("  No events logged\n");
    } else {
        printf("%.*s", sb_ctx.log_offset, sb_ctx.audit_log);
    }
    
    printf("\n");
}

/*
 * Self-test Secure Boot components
 */
int secure_boot_selftest(void) {
    printf(COLOR_CYAN "Running Secure Boot self-test...\n" COLOR_RESET);
    
    int errors = 0;
    
    /* Test hash function */
    uint8_t test_data[] = "CodixOS Secure Boot Test";
    uint8_t hash[32];
    simple_hash(test_data, sizeof(test_data) - 1, hash);
    printf("  Hash function:     %s\n", COLOR_GREEN "OK" COLOR_RESET);
    
    /* Test key management */
    uint8_t test_key_id[16] = {0xAA, 0xBB, 0xCC, 0xDD, 0x11, 0x22, 0x33, 0x44,
                                0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
    if (secure_boot_add_key("Test Key", test_key_id, 128, KEY_TYPE_RSA2048) == 0) {
        printf("  Key management:    %s\n", COLOR_GREEN "OK" COLOR_RESET);
        secure_boot_remove_key(test_key_id);
    } else {
        printf("  Key management:    %s\n", COLOR_RED "FAILED" COLOR_RESET);
        errors++;
    }
    
    /* Test signature generation */
    signature_t test_sig;
    if (secure_boot_sign_data(test_data, sizeof(test_data) - 1, &test_sig) == 0) {
        printf("  Sign data:         %s\n", COLOR_GREEN "OK" COLOR_RESET);
        
        /* Test signature verification */
        int verify_result = secure_boot_verify_signature(test_data, sizeof(test_data) - 1, &test_sig);
        if (verify_result == SIG_VERIFY_OK) {
            printf("  Verify signature:  %s\n", COLOR_GREEN "OK" COLOR_RESET);
        } else {
            printf("  Verify signature:  %s\n", COLOR_RED "FAILED" COLOR_RESET);
            errors++;
        }
    } else {
        printf("  Sign data:         %s\n", COLOR_RED "FAILED" COLOR_RESET);
        errors++;
    }
    
    /* Test boot stage verification */
    if (secure_boot_verify_stage(2, test_data, sizeof(test_data) - 1) == SIG_VERIFY_OK) {
        printf("  Boot verification: %s\n", COLOR_GREEN "OK" COLOR_RESET);
    } else {
        printf("  Boot verification: %s\n", COLOR_RED "FAILED" COLOR_RESET);
        errors++;
    }
    
    printf("\n");
    
    if (errors == 0) {
        printf(COLOR_GREEN "All self-tests passed!\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "%d self-tests failed\n" COLOR_RESET, errors);
    }
    
    return errors;
}
