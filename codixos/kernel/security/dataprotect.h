/*
 * CodixOS Data Protection Module
 * Encryption in transit (TLS/SSL) and key management
 */

#ifndef DATAPROTECT_H
#define DATAPROTECT_H

/* Type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* TLS versions */
#define TLS_VERSION_1_0     0x0301
#define TLS_VERSION_1_1     0x0302
#define TLS_VERSION_1_2     0x0303
#define TLS_VERSION_1_3     0x0304

/* Cipher suites */
#define TLS_RSA_WITH_AES_128_CBC_SHA      0x002F
#define TLS_RSA_WITH_AES_256_CBC_SHA      0x0035
#define TLS_RSA_WITH_AES_128_GCM_SHA256   0x009C
#define TLS_RSA_WITH_AES_256_GCM_SHA384   0x009D
#define TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256  0xC02F
#define TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384  0xC030
#define TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305    0xCCA8

/* Key types */
#define KEY_TYPE_RSA_2048    1
#define KEY_TYPE_RSA_4096    2
#define KEY_TYPE_ECDSA_P256  3
#define KEY_TYPE_ECDSA_P384  4
#define KEY_TYPE_ED25519     5

/* Certificate types */
#define CERT_TYPE_SELF_SIGNED  0
#define CERT_TYPE_CA_SIGNED    1
#define CERT_TYPE_LETSENCRYPT  2

/* Connection states */
#define CONN_STATE_CLOSED       0
#define CONN_STATE_CONNECTING   1
#define CONN_STATE_HANDSHAKE    2
#define CONN_STATE_ESTABLISHED  3
#define CONN_STATE_CLOSING      4

/* Maximum values */
#define MAX_TLS_CONNECTIONS    16
#define MAX_CERTIFICATES       8
#define MAX_KEY_PAIRS          8
#define MAX_CA_CERTS           16

/* Key pair structure */
typedef struct {
    uint32_t    id;
    char        name[64];
    uint8_t     key_type;
    uint8_t     private_key[512];
    uint32_t    private_key_len;
    uint8_t     public_key[512];
    uint32_t    public_key_len;
} key_pair_t;

/* Certificate structure */
typedef struct {
    uint32_t    id;
    char        subject[128];
    char        issuer[128];
    uint8_t     cert_type;
    uint32_t    key_pair_id;
    uint8_t     der_encoded[2048];
    uint32_t    der_len;
    uint32_t    valid_from;
    uint32_t    valid_to;
    uint8_t     revoked;
    uint8_t     fingerprint[32];
} certificate_t;

/* TLS connection structure */
typedef struct {
    uint32_t    id;
    uint32_t    state;
    uint32_t    client_fd;
    uint32_t    server_fd;
    uint16_t    version;
    uint16_t    cipher_suite;
    uint32_t    cert_id;
    uint8_t     session_id[32];
    uint8_t     master_secret[48];
    uint8_t     client_random[32];
    uint8_t     server_random[32];
    char        hostname[256];
    uint32_t    created;
    uint32_t    last_activity;
    uint8_t     encrypted;
    uint64_t    bytes_sent;
    uint64_t    bytes_received;
} tls_connection_t;

/* Data protection context */
typedef struct {
    key_pair_t      key_pairs[MAX_KEY_PAIRS];
    uint32_t        key_pair_count;
    certificate_t   certificates[MAX_CERTIFICATES];
    uint32_t        cert_count;
    certificate_t   ca_certs[MAX_CA_CERTS];
    uint32_t        ca_count;
    tls_connection_t connections[MAX_TLS_CONNECTIONS];
    uint32_t        connection_count;
    uint32_t        next_key_id;
    uint32_t        next_cert_id;
    uint32_t        next_conn_id;
    uint16_t        min_tls_version;
    uint16_t        max_tls_version;
    uint8_t         require_client_cert;
    uint8_t         verifyhostname;
} dataprotect_ctx_t;

/*
 * Initialize data protection system
 */
void dataprotect_init(void);

/*
 * Generate RSA key pair
 */
int dataprotect_generate_key(const char* name, uint8_t key_type);

/*
 * Load key pair
 */
int dataprotect_load_key(uint32_t key_id, const uint8_t* private_key, uint32_t len);

/*
 * Delete key pair
 */
int dataprotect_delete_key(uint32_t key_id);

/*
 * Generate self-signed certificate
 */
int dataprotect_generate_self_signed(const char* subject, uint32_t key_id, uint32_t days_valid);

/*
 * Generate CSR (Certificate Signing Request)
 */
int dataprotect_generate_csr(const char* subject, uint32_t key_id, uint8_t* csr_out, uint32_t* csr_len);

/*
 * Import certificate
 */
int dataprotect_import_cert(const uint8_t* cert_der, uint32_t len, const char* subject);

/*
 * Import CA certificate
 */
int dataprotect_import_ca_cert(const uint8_t* cert_der, uint32_t len, const char* subject);

/*
 * Delete certificate
 */
int dataprotect_delete_cert(uint32_t cert_id);

/*
 * Revoke certificate
 */
int dataprotect_revoke_cert(uint32_t cert_id);

/*
 * Get certificate info
 */
int dataprotect_get_cert_info(uint32_t cert_id, certificate_t* info_out);

/*
 * Verify certificate chain
 */
int dataprotect_verify_cert_chain(uint32_t cert_id);

/*
 * Check if certificate is trusted
 */
int dataprotect_is_trusted(uint32_t cert_id);

/*
 * Create TLS connection (client)
 */
int dataprotect_connect(const char* hostname, uint16_t port);

/*
 * Create TLS listener (server)
 */
int dataprotect_listen(uint16_t port, uint32_t cert_id);

/*
 * Accept TLS connection
 */
int dataprotect_accept(uint32_t listener_fd, uint32_t cert_id);

/*
 * Close TLS connection
 */
int dataprotect_close(uint32_t conn_id);

/*
 * Send encrypted data
 */
int dataprotect_send(uint32_t conn_id, const uint8_t* data, uint32_t len);

/*
 * Receive encrypted data
 */
int dataprotect_recv(uint32_t conn_id, uint8_t* data, uint32_t max_len, uint32_t* actual_len);

/*
 * Perform TLS handshake
 */
int dataprotect_handshake(uint32_t conn_id);

/*
 * Set minimum TLS version
 */
void dataprotect_set_min_version(uint16_t version);

/*
 * Set maximum TLS version
 */
void dataprotect_set_max_version(uint16_t version);

/*
 * Require client certificates
 */
void dataprotect_require_client_cert(int require);

/*
 * Enable/disable hostname verification
 */
void dataprotect_verify_hostname(int verify);

/*
 * Get connection info
 */
int dataprotect_get_connection(uint32_t conn_id, tls_connection_t* info_out);

/*
 * Show all key pairs
 */
void dataprotect_list_keys(void);

/*
 * Show all certificates
 */
void dataprotect_list_certs(void);

/*
 * Show CA certificates
 */
void dataprotect_list_ca_certs(void);

/*
 * Show active connections
 */
void dataprotect_list_connections(void);

/*
 * Show data protection status
 */
void dataprotect_status(void);

/*
 * Export certificate to PEM
 */
int dataprotect_export_cert_pem(uint32_t cert_id, char* pem_out, uint32_t* pem_len);

/*
 * Export private key to PEM
 */
int dataprotect_export_key_pem(uint32_t key_id, char* pem_out, uint32_t* pem_len);

/*
 * Self-test
 */
int dataprotect_selftest(void);

#endif /* DATAPROTECT_H */
