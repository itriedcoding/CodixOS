/*
 * CodixOS Full-Disk Encryption Module
 * LUKS-based encryption for secure storage
 */

#ifndef DISK_ENCRYPT_H
#define DISK_ENCRYPT_H

#include "../kernel/kernel.h"

/* Encryption states */
#define CRYPT_STATE_NONE        0
#define CRYPT_STATE_PLAIN       1
#define CRYPT_STATE_ENCRYPTED   2
#define CRYPT_STATE_UNLOCKED    3

/* LUKS constants */
#define LUKS_MAGIC              "LUKS\xba\xbe"
#define LUKS_MAGIC_LEN          6
#define LUKS_VERSION            1
#define LUKS_MAX_KEYSLOTS       8
#define LUKS_KEY_LEN            64

/* Cipher types */
#define CIPHER_AES_256_XTS      "aes-xts-plain64"
#define CIPHER_AES_128_CBC      "aes-cbc-essiv:sha256"
#define CIPHER_SERPENT_256      "serpent-xts-plain64"

/* Hash algorithms */
#define HASH_SHA256             "sha256"
#define HASH_SHA512             "sha512"
#define HASH_SHA3_256           "sha3-256"

/* Key derivation functions */
#define KDF_PBKDF2              "pbkdf2"
#define KDF_ARGON2              "argon2id"
#define KDF_BCRYPT              "bcrypt"

/* LUKS header structure */
typedef struct {
    uint8_t  magic[6];           /* LUKS magic bytes */
    uint16_t version;            /* LUKS version */
    uint8_t  cipher_name[32];    /* Cipher name */
    uint8_t  cipher_mode[32];    /* Cipher mode */
    uint8_t  hash_spec[32];      /* Hash specification */
    uint32_t payload_offset;     /* Payload data offset */
    uint32_t key_bytes;          /* Key size in bytes */
    uint32_t mk_digest_iterations; /* Master key digest iterations */
    uint32_t mk_digest_salt[16]; /* Master key digest salt */
    uint8_t  mk_digest[32];      /* Master key digest */
    uint8_t  uuid[40];           /* Device UUID */
    struct {
        uint32_t active;         /* Key slot active */
        uint32_t iterations;     /* PBKDF iterations */
        uint32_t salt[16];       /* Salt */
        uint32_t key_offset;     /* Key material offset */
        uint32_t stripes;        /* Anti-forensic stripes */
    } keyslot[LUKS_MAX_KEYSLOTS];
} luks_header_t;

/* Encryption context */
typedef struct {
    uint8_t  state;              /* Encryption state */
    uint8_t  device[256];        /* Device path */
    uint8_t  cipher[64];         /* Cipher in use */
    uint8_t  hash_algo[32];      /* Hash algorithm */
    uint8_t  key_derivation[32]; /* Key derivation function */
    uint32_t key_size;           /* Key size in bytes */
    uint8_t  master_key[64];     /* Master key (when unlocked) */
    uint8_t  volume_key[32];     /* Volume key */
    uint8_t  uuid[40];           /* Device UUID */
    luks_header_t header;        /* LUKS header */
} encrypt_ctx_t;

/* Encryption status for display */
typedef struct {
    char     device[256];        /* Device path */
    char     mapper[128];        /* Mapper name */
    uint8_t  encrypted;          /* Is encrypted */
    uint8_t  unlocked;           /* Is unlocked */
    char     cipher[64];         /* Cipher type */
    uint32_t key_size;           /* Key size */
} encrypt_status_t;

/* Global encryption context */
static encrypt_ctx_t encrypt_ctx;

/*
 * Initialize disk encryption system
 */
void disk_encrypt_init(void);

/*
 * Format device with LUKS encryption
 */
int disk_encrypt_format(const char* device, const char* passphrase, const char* cipher);

/*
 * Open (unlock) encrypted device
 */
int disk_encrypt_open(const char* device, const char* passphrase, const char* mapper_name);

/*
 * Close (lock) encrypted device
 */
int disk_encrypt_close(const char* mapper_name);

/*
 * Add key to encrypted device
 */
int disk_encrypt_add_key(const char* device, const char* existing_pass, const char* new_pass);

/*
 * Remove key from encrypted device
 */
int disk_encrypt_remove_key(const char* device, const char* passphrase, int key_slot);

/*
 * Change passphrase for encrypted device
 */
int disk_encrypt_change_passphrase(const char* device, const char* old_pass, const char* new_pass);

/*
 * Backup LUKS header
 */
int disk_encrypt_backup_header(const char* device, const char* backup_file);

/*
 * Restore LUKS header from backup
 */
int disk_encrypt_restore_header(const char* device, const char* backup_file);

/*
 * Get encryption status for device
 */
int disk_encrypt_get_status(const char* device, encrypt_status_t* status);

/*
 * Verify device is encrypted
 */
int disk_encrypt_verify(const char* device);

/*
 * Get cipher information
 */
void disk_encrypt_cipher_info(const char* cipher);

/*
 * List all encrypted devices
 */
void disk_encrypt_list_devices(void);

/*
 * Benchmark encryption performance
 */
void disk_encrypt_benchmark(const char* cipher);

/*
 * Generate crypttab entry
 */
int disk_encrypt_generate_crypttab(const char* device, const char* mapper_name);

/*
 * Generate fstab entry for encrypted device
 */
int disk_encrypt_generate_fstab(const char* mapper_name, const char* mount_point);

/*
 * Auto-unlock device at boot (using keyfile)
 */
int disk_encrypt_enable_autounlock(const char* device, const char* keyfile);

/*
 * Disable auto-unlock
 */
int disk_encrypt_disable_autounlock(const char* device);

/*
 * Check encryption health
 */
int disk_encrypt_health_check(const char* device);

/*
 * Secure wipe encrypted device
 */
int disk_encrypt_secure_wipe(const char* device);

#endif /* DISK_ENCRYPT_H */
