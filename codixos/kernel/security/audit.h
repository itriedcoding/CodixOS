/*
 * CodixOS Audit/Logging Module
 * Comprehensive event tracking and security auditing
 */

#ifndef AUDIT_H
#define AUDIT_H

/* Type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* Log levels */
#define LOG_LEVEL_EMERGENCY  0
#define LOG_LEVEL_ALERT      1
#define LOG_LEVEL_CRITICAL   2
#define LOG_LEVEL_ERROR      3
#define LOG_LEVEL_WARNING    4
#define LOG_LEVEL_NOTICE     5
#define LOG_LEVEL_INFO       6
#define LOG_LEVEL_DEBUG      7

/* Event categories */
#define AUDIT_CATEGORY_AUTH       1    /* Authentication events */
#define AUDIT_CATEGORY_SYSTEM     2    /* System events */
#define AUDIT_CATEGORY_NETWORK    3    /* Network events */
#define AUDIT_CATEGORY_FILE       4    /* File system events */
#define AUDIT_CATEGORY_PROCESS    5    /* Process events */
#define AUDIT_CATEGORY_SECURITY   6    /* Security events */
#define AUDIT_CATEGORY_ADMIN      7    /* Administrative events */

/* Event types */
#define EVENT_LOGIN               100
#define EVENT_LOGOUT              101
#define EVENT_LOGIN_FAILED        102
#define EVENT_PASSWORD_CHANGE     103
#define EVENT_MFA_VERIFY          104
#define EVENT_USER_CREATE         110
#define EVENT_USER_DELETE         111
#define EVENT_USER_MODIFY         112
#define EVENT_GROUP_CREATE        113
#define EVENT_GROUP_DELETE        114
#define EVENT_SUDO_EXECUTE        120
#define EVENT_SU_EXECUTE          121
#define EVENT_SERVICE_START       200
#define EVENT_SERVICE_STOP        201
#define EVENT_SERVICE_FAIL        202
#define EVENT_BOOT                210
#define EVENT_SHUTDOWN            211
#define EVENT_REBOOT              212
#define EVENT_NETWORK_CONNECT     300
#define EVENT_NETWORK_DISCONNECT  301
#define EVENT_FIREWALL_BLOCK      302
#define EVENT_DNS_RESOLVE         303
#define EVENT_FILE_READ           400
#define EVENT_FILE_WRITE          401
#define EVENT_FILE_DELETE         402
#define EVENT_FILE_CHMOD          403
#define EVENT_FILE_CHOWN          404
#define EVENT_FILE_CREATE         405
#define EVENT_PROCESS_CREATE      500
#define EVENT_PROCESS_EXIT        501
#define EVENT_PROCESS_KILL        502
#define EVENT_PROCESS_SIGNAL      503
#define EVENT_MODULE_LOAD         600
#define EVENT_MODULE_UNLOAD       601
#define EVENT_CONFIG_CHANGE       602
#define EVENT_HARDENING_CHANGE    603
#define EVENT_ADMIN_COMMAND       700
#define EVENT_CONFIG_BACKUP       701
#define EVENT_CONFIG_RESTORE      702

/* Maximum values */
#define MAX_LOG_ENTRIES           4096
#define MAX_LOG_RULES            64
#define MAX_ALERT_RECIPIENTS     8
#define MAX_LOG_RETENTION_DAYS   90

/* Log entry structure */
typedef struct {
    uint32_t    id;                 /* Entry ID */
    uint32_t    timestamp;          /* Timestamp */
    uint8_t     level;              /* Log level */
    uint8_t     category;           /* Event category */
    uint16_t    event_type;         /* Event type */
    uint32_t    uid;                /* User ID */
    uint32_t    pid;                /* Process ID */
    uint8_t     success;            /* 1=success, 0=failure */
    char        source[64];         /* Source (IP, device, etc.) */
    char        message[256];       /* Log message */
    uint8_t     data[128];          /* Additional data */
    uint32_t    data_len;           /* Data length */
} log_entry_t;

/* Log filter rule */
typedef struct {
    uint8_t     enabled;
    uint8_t     category;
    uint16_t    event_type;
    uint8_t     level;
    uint8_t     action;             /* 0=log, 1=alert, 2=ignore */
} log_rule_t;

/* Alert recipient */
typedef struct {
    char        name[64];
    uint8_t     enabled;
    uint8_t     method;             /* 0=syslog, 1=email, 2=webhook */
    char        destination[128];
} alert_recipient_t;

/* Audit context */
typedef struct {
    log_entry_t     entries[MAX_LOG_ENTRIES];
    uint32_t        entry_count;
    uint32_t        next_id;
    log_rule_t      rules[MAX_LOG_RULES];
    uint32_t        rule_count;
    alert_recipient_t recipients[MAX_ALERT_RECIPIENTS];
    uint32_t        recipient_count;
    uint8_t         enabled;
    uint8_t         level_filter;       /* Minimum log level to store */
    uint8_t         console_output;     /* Echo to console */
    uint32_t        retention_days;     /* Days to keep logs */
    uint32_t        max_size_mb;        /* Maximum log size */
    uint32_t        total_events;       /* Total events logged */
    uint32_t        filtered_events;    /* Events filtered out */
    uint32_t        security_events;    /* Security-related events */
} audit_ctx_t;

/*
 * Initialize audit system
 */
void audit_init(void);

/*
 * Enable/disable audit logging
 */
void audit_enable(void);
void audit_disable(void);

/*
 * Log an event
 */
int audit_log(uint8_t level, uint8_t category, uint16_t event_type,
              uint32_t uid, uint32_t pid, uint8_t success,
              const char* source, const char* message,
              const uint8_t* data, uint32_t data_len);

/*
 * Log authentication event
 */
int audit_log_auth(uint16_t event_type, uint32_t uid, const char* username,
                   const char* source, uint8_t success);

/*
 * Log system event
 */
int audit_log_system(uint16_t event_type, const char* message, uint8_t success);

/*
 * Log network event
 */
int audit_log_network(uint16_t event_type, const char* source_ip,
                      const char* dest_ip, uint16_t port, uint8_t success);

/*
 * Log file event
 */
int audit_log_file(uint16_t event_type, uint32_t uid, const char* filename,
                   const char* operation, uint8_t success);

/*
 * Log process event
 */
int audit_log_process(uint16_t event_type, uint32_t uid, uint32_t pid,
                      const char* process_name, uint8_t success);

/*
 * Add log filter rule
 */
int audit_add_rule(uint8_t category, uint16_t event_type, uint8_t level, uint8_t action);

/*
 * Remove log filter rule
 */
int audit_remove_rule(uint32_t rule_index);

/*
 * Add alert recipient
 */
int audit_add_recipient(const char* name, uint8_t method, const char* destination);

/*
 * Remove alert recipient
 */
int audit_remove_recipient(const char* name);

/*
 * Search logs by category
 */
int audit_search_category(uint8_t category, uint32_t* results, uint32_t max_results);

/*
 * Search logs by user
 */
int audit_search_user(uint32_t uid, uint32_t* results, uint32_t max_results);

/*
 * Search logs by event type
 */
int audit_search_event(uint16_t event_type, uint32_t* results, uint32_t max_results);

/*
 * Search logs by time range
 */
int audit_search_time(uint32_t start_time, uint32_t end_time, uint32_t* results, uint32_t max_results);

/*
 * Get log entry by ID
 */
int audit_get_entry(uint32_t id, log_entry_t* entry_out);

/*
 * Export logs
 */
int audit_export(const char* filename, uint32_t start_time, uint32_t end_time);

/*
 * Rotate logs (remove old entries)
 */
void audit_rotate(void);

/*
 * Set log level filter
 */
void audit_set_level(uint8_t level);

/*
 * Set console output
 */
void audit_set_console(uint8_t enabled);

/*
 * Set retention period
 */
void audit_set_retention(uint32_t days);

/*
 * Show audit status
 */
void audit_status(void);

/*
 * Show recent logs
 */
void audit_show_recent(uint32_t count);

/*
 * Show logs by category
 */
void audit_show_category(uint8_t category);

/*
 * Show logs by user
 */
void audit_show_user(uint32_t uid);

/*
 * Show security events
 */
void audit_show_security(void);

/*
 * Show failed events
 */
void audit_show_failures(void);

/*
 * Show statistics
 */
void audit_statistics(void);

/*
 * Clear all logs
 */
void audit_clear(void);

/*
 * Self-test
 */
int audit_selftest(void);

#endif /* AUDIT_H */
