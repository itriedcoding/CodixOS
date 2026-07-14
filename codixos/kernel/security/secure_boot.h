/*
 * CodixOS Secure Boot Module
 * Verifies digital signatures of firmware and bootloaders
 */

#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#include "../kernel/kernel.h"

/* Secure Boot states */
#define SECURE_BOOT_DISABLED     0
#define SECURE_BOOT_ENABLED      1
#define SECURE_BOOT_SETUP_MODE   2

/* Signature verification results */
#define SIG_VERIFY_OK            0
#define SIG_VERIFY_FAILED        1
#define SIG_VERIFY_NO_KEY        2
#define SIG_VERIFY_CORRUPTED     3

/* Maximum number of trusted keys */
#define MAX_TRUSTED_KEYS         16

/* Key types */
#define KEY_TYPE_RSA2048         1
#define KEY_TYPE_RSA4096         2
#define KEY_TYPE_ECC256          3

/* Signature structure */
typedef struct {
    uint8_t  algorithm;       /* Algorithm type */
    uint8_t  key_id[16];      /* Key identifier */
    uint8_t  signature[256];  /* Digital signature */
    uint32_t sig_len;         /* Signature length */
    uint64_t timestamp;       /* When signature was created */
} signature_t;

/* Trusted key structure */
typedef struct {
    char     name[64];        /* Key name */
    uint8_t  key_id[16];      /* Unique key identifier */
    uint8_t  public_key[512]; /* Public key data */
    uint32_t key_len;         /* Key length in bytes */
    uint8_t  key_type;        /* Key type */
    uint8_t  trusted;         /* Whether key is trusted */
} trusted_key_t;

/* Boot stage verification */
typedef struct {
    uint8_t  firmware_verified;    /* UEFI/BIOS firmware */
    uint8_t  bootloader_verified;  /* Bootloader (GRUB, etc.) */
    uint8_t  kernel_verified;      /* Kernel image */
    uint8_t  initrd_verified;      /* Initial ramdisk */
    uint8_t  overall_status;       /* Overall boot chain status */
} boot_verification_t;

/* Secure Boot context */
typedef struct {
    uint8_t        state;              /* Secure Boot state */
    trusted_key_t  trusted_keys[MAX_TRUSTED_KEYS];
    uint32_t       key_count;          /* Number of loaded keys */
    boot_verification_t verification;  /* Verification status */
    uint8_t        audit_log[4096];    /* Security audit log */
    uint32_t       log_offset;         /* Current log position */
} secure_boot_ctx_t;

/* Global secure boot context */
static secure_boot_ctx_t secure_boot_ctx;

/*
 * Initialize Secure Boot system
 */
void secure_boot_init(void);

/*
 * Enable Secure Boot
 */
int secure_boot_enable(void);

/*
 * Disable Secure Boot (requires authentication)
 */
int secure_boot_disable(void);

/*
 * Enter Setup Mode (for key management)
 */
int secure_boot_setup_mode(void);

/*
 * Add a trusted key
 */
int secure_boot_add_key(const char* name, const uint8_t* key_data, uint32_t key_len, uint8_t key_type);

/*
 * Remove a trusted key
 */
int secure_boot_remove_key(const uint8_t* key_id);

/*
 * Verify a digital signature
 */
int secure_boot_verify_signature(const uint8_t* data, uint32_t data_len, const signature_t* sig);

/*
 * Verify boot stage
 */
int secure_boot_verify_stage(uint8_t stage, const uint8_t* image, uint32_t image_len);

/*
 * Verify firmware signature
 */
int secure_boot_verify_firmware(const uint8_t* firmware, uint32_t size);

/*
 * Verify bootloader signature
 */
int secure_boot_verify_bootloader(const uint8_t* bootloader, uint32_t size);

/*
 * Verify kernel signature
 */
int secure_boot_verify_kernel(const uint8_t* kernel, uint32_t size);

/*
 * Generate signature for data
 */
int secure_boot_sign_data(const uint8_t* data, uint32_t data_len, signature_t* sig_out);

/*
 * Get Secure Boot status
 */
void secure_boot_status(void);

/*
 * Get verification status for a specific stage
 */
int secure_boot_get_stage_status(uint8_t stage);

/*
 * Log security event
 */
void secure_boot_log_event(const char* event);

/*
 * Print audit log
 */
void secure_boot_print_log(void);

/*
 * Self-test Secure Boot components
 */
int secure_boot_selftest(void);

#endif /* SECURE_BOOT_H */
