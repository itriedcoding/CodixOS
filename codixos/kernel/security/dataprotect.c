/*
 * CodixOS Data Protection Implementation
 * Encryption in transit (TLS/SSL) and key management
 */

#include "dataprotect.h"

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global context */
static dataprotect_ctx_t dp_ctx;

/* External kernel functions */
extern void print(const char* str);
extern void println(const char* str);
extern void print_int(int num);
extern void putchar(char c);
extern void memset(void* dest, int c, size_t n);
extern void memcpy(void* dest, const void* src, size_t n);
extern int strcmp(const char* s1, const char* s2);
extern void strcpy(char* dest, const char* src);
extern size_t strlen(const char* str);

/* Pseudo-random number generator */
static uint32_t dp_rng_state = 0xDEADBEEF;
static uint8_t dp_random(void) {
    dp_rng_state = dp_rng_state * 1103515245 + 12345;
    return (uint8_t)((dp_rng_state >> 16) & 0xFF);
}

static void dp_random_bytes(uint8_t* buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = dp_random();
    }
}

/* Simple hash */
static void dp_hash(const uint8_t* data, uint32_t len, uint8_t* hash) {
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

/* Find key pair */
static key_pair_t* find_key(uint32_t id) {
    for (uint32_t i = 0; i < dp_ctx.key_pair_count; i++) {
        if (dp_ctx.key_pairs[i].id == id) return &dp_ctx.key_pairs[i];
    }
    return (key_pair_t*)0;
}

/* Find certificate */
static certificate_t* find_cert(uint32_t id) {
    for (uint32_t i = 0; i < dp_ctx.cert_count; i++) {
        if (dp_ctx.certificates[i].id == id) return &dp_ctx.certificates[i];
    }
    return (certificate_t*)0;
}

/* Find CA cert */
static certificate_t* find_ca_cert(uint32_t id) {
    for (uint32_t i = 0; i < dp_ctx.ca_count; i++) {
        if (dp_ctx.ca_certs[i].id == id) return &dp_ctx.ca_certs[i];
    }
    return (certificate_t*)0;
}

/* Find connection */
static tls_connection_t* find_connection(uint32_t id) {
    for (uint32_t i = 0; i < dp_ctx.connection_count; i++) {
        if (dp_ctx.connections[i].id == id) return &dp_ctx.connections[i];
    }
    return (tls_connection_t*)0;
}

/* Initialize */
void dataprotect_init(void) {
    memset(&dp_ctx, 0, sizeof(dataprotect_ctx_t));
    dp_ctx.next_key_id = 1;
    dp_ctx.next_cert_id = 1;
    dp_ctx.next_conn_id = 1;
    dp_ctx.min_tls_version = TLS_VERSION_1_2;
    dp_ctx.max_tls_version = TLS_VERSION_1_3;
    dp_ctx.require_client_cert = 0;
    dp_ctx.verifyhostname = 1;
    
    println("Data protection system initialized");
}

/* Generate key pair */
int dataprotect_generate_key(const char* name, uint8_t key_type) {
    if (!name) {
        println("Error: name required");
        return -1;
    }
    
    if (dp_ctx.key_pair_count >= MAX_KEY_PAIRS) {
        println("Error: maximum key pairs reached");
        return -1;
    }
    
    key_pair_t* kp = &dp_ctx.key_pairs[dp_ctx.key_pair_count++];
    memset(kp, 0, sizeof(key_pair_t));
    
    kp->id = dp_ctx.next_key_id++;
    strcpy(kp->name, name);
    kp->key_type = key_type;
    
    /* Generate simulated keys */
    uint32_t key_len = (key_type == KEY_TYPE_RSA_2048) ? 256 :
                       (key_type == KEY_TYPE_RSA_4096) ? 512 : 64;
    
    dp_random_bytes(kp->private_key, key_len);
    kp->private_key_len = key_len;
    
    dp_random_bytes(kp->public_key, key_len);
    kp->public_key_len = key_len;
    
    print("Key pair generated: ");
    print(name);
    print(" (ID: ");
    print_int(kp->id);
    println(")");
    
    return kp->id;
}

/* Delete key */
int dataprotect_delete_key(uint32_t key_id) {
    for (uint32_t i = 0; i < dp_ctx.key_pair_count; i++) {
        if (dp_ctx.key_pairs[i].id == key_id) {
            for (uint32_t j = i; j < dp_ctx.key_pair_count - 1; j++) {
                dp_ctx.key_pairs[j] = dp_ctx.key_pairs[j + 1];
            }
            dp_ctx.key_pair_count--;
            println("Key pair deleted");
            return 0;
        }
    }
    println("Error: key not found");
    return -1;
}

/* Generate self-signed certificate */
int dataprotect_generate_self_signed(const char* subject, uint32_t key_id, uint32_t days_valid) {
    key_pair_t* kp = find_key(key_id);
    if (!kp) {
        println("Error: key not found");
        return -1;
    }
    
    if (dp_ctx.cert_count >= MAX_CERTIFICATES) {
        println("Error: maximum certificates reached");
        return -1;
    }
    
    certificate_t* cert = &dp_ctx.certificates[dp_ctx.cert_count++];
    memset(cert, 0, sizeof(certificate_t));
    
    cert->id = dp_ctx.next_cert_id++;
    strcpy(cert->subject, subject);
    strcpy(cert->issuer, subject); /* Self-signed */
    cert->cert_type = CERT_TYPE_SELF_SIGNED;
    cert->key_pair_id = key_id;
    cert->valid_from = 1000000;
    cert->valid_to = cert->valid_from + (days_valid * 86400);
    
    /* Generate certificate fingerprint */
    dp_hash(kp->public_key, kp->public_key_len, cert->fingerprint);
    
    print("Self-signed certificate generated for: ");
    println(subject);
    print("  ID: ");
    print_int(cert->id);
    println("");
    
    return cert->id;
}

/* Import certificate */
int dataprotect_import_cert(const uint8_t* cert_der, uint32_t len, const char* subject) {
    if (!cert_der || len > 2048) {
        println("Error: invalid certificate");
        return -1;
    }
    
    if (dp_ctx.cert_count >= MAX_CERTIFICATES) {
        println("Error: maximum certificates reached");
        return -1;
    }
    
    certificate_t* cert = &dp_ctx.certificates[dp_ctx.cert_count++];
    memset(cert, 0, sizeof(certificate_t));
    
    cert->id = dp_ctx.next_cert_id++;
    strcpy(cert->subject, subject);
    cert->cert_type = CERT_TYPE_CA_SIGNED;
    memcpy(cert->der_encoded, cert_der, len);
    cert->der_len = len;
    
    dp_hash(cert_der, len, cert->fingerprint);
    
    print("Certificate imported for: ");
    println(subject);
    
    return cert->id;
}

/* Import CA certificate */
int dataprotect_import_ca_cert(const uint8_t* cert_der, uint32_t len, const char* subject) {
    if (!cert_der || len > 2048) {
        println("Error: invalid certificate");
        return -1;
    }
    
    if (dp_ctx.ca_count >= MAX_CA_CERTS) {
        println("Error: maximum CA certificates reached");
        return -1;
    }
    
    certificate_t* cert = &dp_ctx.ca_certs[dp_ctx.ca_count++];
    memset(cert, 0, sizeof(certificate_t));
    
    cert->id = dp_ctx.next_cert_id++;
    strcpy(cert->subject, subject);
    strcpy(cert->issuer, subject);
    cert->cert_type = CERT_TYPE_CA_SIGNED;
    memcpy(cert->der_encoded, cert_der, len);
    cert->der_len = len;
    
    dp_hash(cert_der, len, cert->fingerprint);
    
    print("CA certificate imported: ");
    println(subject);
    
    return cert->id;
}

/* Delete certificate */
int dataprotect_delete_cert(uint32_t cert_id) {
    for (uint32_t i = 0; i < dp_ctx.cert_count; i++) {
        if (dp_ctx.certificates[i].id == cert_id) {
            for (uint32_t j = i; j < dp_ctx.cert_count - 1; j++) {
                dp_ctx.certificates[j] = dp_ctx.certificates[j + 1];
            }
            dp_ctx.cert_count--;
            println("Certificate deleted");
            return 0;
        }
    }
    println("Error: certificate not found");
    return -1;
}

/* Revoke certificate */
int dataprotect_revoke_cert(uint32_t cert_id) {
    certificate_t* cert = find_cert(cert_id);
    if (!cert) {
        println("Error: certificate not found");
        return -1;
    }
    
    cert->revoked = 1;
    println("Certificate revoked");
    return 0;
}

/* Get cert info */
int dataprotect_get_cert_info(uint32_t cert_id, certificate_t* info_out) {
    certificate_t* cert = find_cert(cert_id);
    if (!cert) return -1;
    memcpy(info_out, cert, sizeof(certificate_t));
    return 0;
}

/* Verify certificate chain */
int dataprotect_verify_cert_chain(uint32_t cert_id) {
    certificate_t* cert = find_cert(cert_id);
    if (!cert) return -1;
    
    if (cert->revoked) {
        println("Error: certificate is revoked");
        return -1;
    }
    
    /* In production, verify against CA chain */
    return 0;
}

/* Check if trusted */
int dataprotect_is_trusted(uint32_t cert_id) {
    certificate_t* cert = find_cert(cert_id);
    if (!cert) return 0;
    return !cert->revoked;
}

/* Connect (client) */
int dataprotect_connect(const char* hostname, uint16_t port) {
    if (!hostname) return -1;
    
    if (dp_ctx.connection_count >= MAX_TLS_CONNECTIONS) {
        println("Error: maximum connections reached");
        return -1;
    }
    
    tls_connection_t* conn = &dp_ctx.connections[dp_ctx.connection_count++];
    memset(conn, 0, sizeof(tls_connection_t));
    
    conn->id = dp_ctx.next_conn_id++;
    conn->state = CONN_STATE_CONNECTING;
    strcpy(conn->hostname, hostname);
    conn->created = 1000000;
    conn->last_activity = conn->created;
    conn->encrypted = 1;
    
    /* Generate random values */
    dp_random_bytes(conn->client_random, 32);
    
    print("Connecting to ");
    print(hostname);
    print(":");
    print_int(port);
    println("");
    
    /* Simulate handshake */
    conn->state = CONN_STATE_ESTABLISHED;
    conn->version = TLS_VERSION_1_3;
    conn->cipher_suite = TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
    dp_random_bytes(conn->master_secret, 48);
    
    print("TLS connection established (ID: ");
    print_int(conn->id);
    println(")");
    
    return conn->id;
}

/* Listen (server) */
int dataprotect_listen(uint16_t port, uint32_t cert_id) {
    certificate_t* cert = find_cert(cert_id);
    if (!cert) {
        println("Error: certificate not found");
        return -1;
    }
    
    print("Listening on port ");
    print_int(port);
    println(" with TLS");
    
    return 0;
}

/* Accept */
int dataprotect_accept(uint32_t listener_fd, uint32_t cert_id) {
    if (dp_ctx.connection_count >= MAX_TLS_CONNECTIONS) {
        println("Error: maximum connections reached");
        return -1;
    }
    
    tls_connection_t* conn = &dp_ctx.connections[dp_ctx.connection_count++];
    memset(conn, 0, sizeof(tls_connection_t));
    
    conn->id = dp_ctx.next_conn_id++;
    conn->state = CONN_STATE_ESTABLISHED;
    conn->cert_id = cert_id;
    conn->encrypted = 1;
    dp_random_bytes(conn->server_random, 32);
    
    return conn->id;
}

/* Close connection */
int dataprotect_close(uint32_t conn_id) {
    for (uint32_t i = 0; i < dp_ctx.connection_count; i++) {
        if (dp_ctx.connections[i].id == conn_id) {
            for (uint32_t j = i; j < dp_ctx.connection_count - 1; j++) {
                dp_ctx.connections[j] = dp_ctx.connections[j + 1];
            }
            dp_ctx.connection_count--;
            println("Connection closed");
            return 0;
        }
    }
    return -1;
}

/* Send data */
int dataprotect_send(uint32_t conn_id, const uint8_t* data, uint32_t len) {
    tls_connection_t* conn = find_connection(conn_id);
    if (!conn || conn->state != CONN_STATE_ESTABLISHED) {
        return -1;
    }
    
    conn->bytes_sent += len;
    conn->last_activity = 1000000;
    
    /* In production, encrypt and send */
    return len;
}

/* Receive data */
int dataprotect_recv(uint32_t conn_id, uint8_t* data, uint32_t max_len, uint32_t* actual_len) {
    tls_connection_t* conn = find_connection(conn_id);
    if (!conn || conn->state != CONN_STATE_ESTABLISHED) {
        return -1;
    }
    
    /* Simulate receiving data */
    *actual_len = 0;
    conn->last_activity = 1000000;
    
    return 0;
}

/* Handshake */
int dataprotect_handshake(uint32_t conn_id) {
    tls_connection_t* conn = find_connection(conn_id);
    if (!conn) return -1;
    
    conn->state = CONN_STATE_HANDSHAKE;
    
    /* Simulate handshake steps */
    dp_random_bytes(conn->server_random, 32);
    dp_random_bytes(conn->master_secret, 48);
    
    conn->state = CONN_STATE_ESTABLISHED;
    conn->cipher_suite = TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
    conn->version = TLS_VERSION_1_3;
    
    return 0;
}

/* Set min version */
void dataprotect_set_min_version(uint16_t version) {
    dp_ctx.min_tls_version = version;
}

/* Set max version */
void dataprotect_set_max_version(uint16_t version) {
    dp_ctx.max_tls_version = version;
}

/* Require client cert */
void dataprotect_require_client_cert(int require) {
    dp_ctx.require_client_cert = require;
}

/* Verify hostname */
void dataprotect_verify_hostname(int verify) {
    dp_ctx.verifyhostname = verify;
}

/* Get connection info */
int dataprotect_get_connection(uint32_t conn_id, tls_connection_t* info_out) {
    tls_connection_t* conn = find_connection(conn_id);
    if (!conn) return -1;
    memcpy(info_out, conn, sizeof(tls_connection_t));
    return 0;
}

/* List keys */
void dataprotect_list_keys(void) {
    println("");
    println("=== Key Pairs ===");
    println("");
    
    if (dp_ctx.key_pair_count == 0) {
        println("  No key pairs");
    } else {
        for (uint32_t i = 0; i < dp_ctx.key_pair_count; i++) {
            key_pair_t* kp = &dp_ctx.key_pairs[i];
            print("  ID: ");
            print_int(kp->id);
            print(" Name: ");
            print(kp->name);
            print(" Type: ");
            switch (kp->key_type) {
                case KEY_TYPE_RSA_2048:   println("RSA-2048"); break;
                case KEY_TYPE_RSA_4096:   println("RSA-4096"); break;
                case KEY_TYPE_ECDSA_P256: println("ECDSA-P256"); break;
                case KEY_TYPE_ECDSA_P384: println("ECDSA-P384"); break;
                case KEY_TYPE_ED25519:    println("Ed25519"); break;
                default:                  println("Unknown"); break;
            }
        }
    }
    println("");
}

/* List certificates */
void dataprotect_list_certs(void) {
    println("");
    println("=== Certificates ===");
    println("");
    
    if (dp_ctx.cert_count == 0) {
        println("  No certificates");
    } else {
        for (uint32_t i = 0; i < dp_ctx.cert_count; i++) {
            certificate_t* c = &dp_ctx.certificates[i];
            print("  ID: ");
            print_int(c->id);
            print(" Subject: ");
            print(c->subject);
            if (c->revoked) print(" [REVOKED]");
            println("");
        }
    }
    println("");
}

/* List connections */
void dataprotect_list_connections(void) {
    println("");
    println("=== TLS Connections ===");
    println("");
    
    if (dp_ctx.connection_count == 0) {
        println("  No active connections");
    } else {
        for (uint32_t i = 0; i < dp_ctx.connection_count; i++) {
            tls_connection_t* c = &dp_ctx.connections[i];
            print("  ID: ");
            print_int(c->id);
            print(" Host: ");
            print(c->hostname);
            print(" State: ");
            switch (c->state) {
                case CONN_STATE_CLOSED:      println("CLOSED"); break;
                case CONN_STATE_CONNECTING:  println("CONNECTING"); break;
                case CONN_STATE_HANDSHAKE:   println("HANDSHAKE"); break;
                case CONN_STATE_ESTABLISHED: println("ESTABLISHED"); break;
                default:                     println("UNKNOWN"); break;
            }
        }
    }
    println("");
}

/* Status */
void dataprotect_status(void) {
    println("");
    println("=== Data Protection Status ===");
    println("");
    
    print("  Key Pairs:    ");
    print_int(dp_ctx.key_pair_count);
    println("");
    print("  Certificates: ");
    print_int(dp_ctx.cert_count);
    println("");
    print("  CA Certs:     ");
    print_int(dp_ctx.ca_count);
    println("");
    print("  Connections:  ");
    print_int(dp_ctx.connection_count);
    println("");
    
    println("");
    println("  TLS Configuration:");
    print("    Min Version:   ");
    print_int(dp_ctx.min_tls_version >> 8);
    print(".");
    print_int(dp_ctx.min_tls_version & 0xFF);
    println("");
    print("    Max Version:   ");
    print_int(dp_ctx.max_tls_version >> 8);
    print(".");
    print_int(dp_ctx.max_tls_version & 0xFF);
    println("");
    print("    Client Certs:  ");
    println(dp_ctx.require_client_cert ? "Required" : "Optional");
    print("    Hostname Vfy:  ");
    println(dp_ctx.verifyhostname ? "Enabled" : "Disabled");
    println("");
}

/* Self-test */
int dataprotect_selftest(void) {
    println("Running data protection self-test...");
    
    int errors = 0;
    
    /* Test key generation */
    int key_id = dataprotect_generate_key("test-key", KEY_TYPE_RSA_2048);
    if (key_id > 0) {
        println("  Key generation:  OK");
        
        /* Test certificate */
        int cert_id = dataprotect_generate_self_signed("CN=test", key_id, 365);
        if (cert_id > 0) {
            println("  Self-signed cert:OK");
            dataprotect_delete_cert(cert_id);
        } else {
            println("  Self-signed cert:FAILED");
            errors++;
        }
        
        dataprotect_delete_key(key_id);
    } else {
        println("  Key generation:  FAILED");
        errors++;
    }
    
    /* Test connection */
    int conn_id = dataprotect_connect("example.com", 443);
    if (conn_id > 0) {
        println("  TLS connect:     OK");
        dataprotect_close(conn_id);
    } else {
        println("  TLS connect:     FAILED");
        errors++;
    }
    
    println("");
    if (errors == 0) {
        println("All data protection self-tests passed!");
    } else {
        print_int(errors);
        println(" self-tests failed");
    }
    
    return errors;
}
