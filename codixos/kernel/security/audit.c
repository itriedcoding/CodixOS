/*
 * CodixOS Audit/Logging Implementation
 * Comprehensive event tracking and security auditing
 */

#include "audit.h"

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global context */
static audit_ctx_t audit_ctx;

/* External kernel functions */
extern void print(const char* str);
extern void println(const char* str);
extern void print_int(int num);
extern void putchar(char c);
extern void memset(void* dest, int c, size_t n);
extern void memcpy(void* dest, const void* src, size_t n);
extern int strcmp(const char* s1, const char* s2);
extern void strcpy(char* dest, const char* src);
extern void strcat(char* dest, const char* src);
extern size_t strlen(const char* str);

/* Get level string */
static const char* get_level_str(uint8_t level) {
    switch (level) {
        case LOG_LEVEL_EMERGENCY: return "EMERG";
        case LOG_LEVEL_ALERT:     return "ALERT";
        case LOG_LEVEL_CRITICAL:  return "CRIT";
        case LOG_LEVEL_ERROR:     return "ERROR";
        case LOG_LEVEL_WARNING:   return "WARN";
        case LOG_LEVEL_NOTICE:    return "NOTICE";
        case LOG_LEVEL_INFO:      return "INFO";
        case LOG_LEVEL_DEBUG:     return "DEBUG";
        default:                  return "UNKNOWN";
    }
}

/* Get category string */
static const char* get_category_str(uint8_t category) {
    switch (category) {
        case AUDIT_CATEGORY_AUTH:     return "AUTH";
        case AUDIT_CATEGORY_SYSTEM:   return "SYSTEM";
        case AUDIT_CATEGORY_NETWORK:  return "NETWORK";
        case AUDIT_CATEGORY_FILE:     return "FILE";
        case AUDIT_CATEGORY_PROCESS:  return "PROCESS";
        case AUDIT_CATEGORY_SECURITY: return "SECURITY";
        case AUDIT_CATEGORY_ADMIN:    return "ADMIN";
        default:                      return "UNKNOWN";
    }
}

/* Get event string */
static const char* get_event_str(uint16_t event_type) {
    switch (event_type) {
        case EVENT_LOGIN:              return "LOGIN";
        case EVENT_LOGOUT:             return "LOGOUT";
        case EVENT_LOGIN_FAILED:       return "LOGIN_FAILED";
        case EVENT_PASSWORD_CHANGE:    return "PASS_CHANGE";
        case EVENT_MFA_VERIFY:         return "MFA_VERIFY";
        case EVENT_USER_CREATE:        return "USER_CREATE";
        case EVENT_USER_DELETE:        return "USER_DELETE";
        case EVENT_SUDO_EXECUTE:       return "SUDO";
        case EVENT_SERVICE_START:      return "SVC_START";
        case EVENT_SERVICE_STOP:       return "SVC_STOP";
        case EVENT_BOOT:               return "BOOT";
        case EVENT_SHUTDOWN:           return "SHUTDOWN";
        case EVENT_NETWORK_CONNECT:    return "NET_CONNECT";
        case EVENT_FIREWALL_BLOCK:     return "FW_BLOCK";
        case EVENT_FILE_READ:          return "FILE_READ";
        case EVENT_FILE_WRITE:         return "FILE_WRITE";
        case EVENT_FILE_DELETE:         return "FILE_DELETE";
        case EVENT_PROCESS_CREATE:     return "PROC_CREATE";
        case EVENT_PROCESS_KILL:       return "PROC_KILL";
        case EVENT_CONFIG_CHANGE:      return "CONFIG_CHANGE";
        case EVENT_ADMIN_COMMAND:      return "ADMIN_CMD";
        default:                       return "OTHER";
    }
}

/* Check if event should be logged */
static int should_log(uint8_t level, uint8_t category, uint16_t event_type) {
    if (!audit_ctx.enabled) return 0;
    if (level > audit_ctx.level_filter) return 0;
    
    for (uint32_t i = 0; i < audit_ctx.rule_count; i++) {
        if (!audit_ctx.rules[i].enabled) continue;
        
        if (audit_ctx.rules[i].category != 0 && audit_ctx.rules[i].category != category)
            continue;
        if (audit_ctx.rules[i].event_type != 0 && audit_ctx.rules[i].event_type != event_type)
            continue;
        if (audit_ctx.rules[i].level != 0 && audit_ctx.rules[i].level < level)
            continue;
        
        if (audit_ctx.rules[i].action == 2) return 0; /* Ignore */
    }
    
    return 1;
}

/* Send alerts */
static void send_alerts(log_entry_t* entry) {
    for (uint32_t i = 0; i < audit_ctx.recipient_count; i++) {
        if (!audit_ctx.recipients[i].enabled) continue;
        
        /* In production, send actual alerts */
        /* For now, just log that we would send */
    }
}

/* Format timestamp */
static void format_timestamp(uint32_t timestamp, char* buf) {
    uint32_t hours = (timestamp / 3600) % 24;
    uint32_t minutes = (timestamp / 60) % 60;
    uint32_t seconds = timestamp % 60;
    
    buf[0] = '0' + (hours / 10);
    buf[1] = '0' + (hours % 10);
    buf[2] = ':';
    buf[3] = '0' + (minutes / 10);
    buf[4] = '0' + (minutes % 10);
    buf[5] = ':';
    buf[6] = '0' + (seconds / 10);
    buf[7] = '0' + (seconds % 10);
    buf[8] = '\0';
}

/* Initialize audit system */
void audit_init(void) {
    memset(&audit_ctx, 0, sizeof(audit_ctx_t));
    
    audit_ctx.enabled = 1;
    audit_ctx.level_filter = LOG_LEVEL_WARNING; /* Log warnings and above */
    audit_ctx.console_output = 1;
    audit_ctx.retention_days = MAX_LOG_RETENTION_DAYS;
    audit_ctx.max_size_mb = 100;
    audit_ctx.next_id = 1;
    
    println("Audit system initialized");
}

/* Enable audit */
void audit_enable(void) {
    audit_ctx.enabled = 1;
    println("Audit logging enabled");
}

/* Disable audit */
void audit_disable(void) {
    audit_ctx.enabled = 0;
    println("Audit logging disabled");
}

/* Log event */
int audit_log(uint8_t level, uint8_t category, uint16_t event_type,
              uint32_t uid, uint32_t pid, uint8_t success,
              const char* source, const char* message,
              const uint8_t* data, uint32_t data_len) {
    
    if (!should_log(level, category, event_type)) {
        audit_ctx.filtered_events++;
        return 0;
    }
    
    if (audit_ctx.entry_count >= MAX_LOG_ENTRIES) {
        audit_rotate(); /* Rotate old entries */
        if (audit_ctx.entry_count >= MAX_LOG_ENTRIES) {
            return -1; /* Still full */
        }
    }
    
    log_entry_t* entry = &audit_ctx.entries[audit_ctx.entry_count++];
    memset(entry, 0, sizeof(log_entry_t));
    
    entry->id = audit_ctx.next_id++;
    entry->timestamp = 1000000; /* Fake timestamp */
    entry->level = level;
    entry->category = category;
    entry->event_type = event_type;
    entry->uid = uid;
    entry->pid = pid;
    entry->success = success;
    
    if (source) strcpy(entry->source, source);
    if (message) strcpy(entry->message, message);
    if (data && data_len > 0 && data_len <= 128) {
        memcpy(entry->data, data, data_len);
        entry->data_len = data_len;
    }
    
    audit_ctx.total_events++;
    if (category == AUDIT_CATEGORY_SECURITY) audit_ctx.security_events++;
    
    /* Console output */
    if (audit_ctx.console_output) {
        char time_str[16];
        format_timestamp(entry->timestamp, time_str);
        
        print("[");
        print(time_str);
        print("] [");
        print(get_level_str(level));
        print("] [");
        print(get_category_str(category));
        print("] ");
        print(get_event_str(event_type));
        print(": ");
        println(message ? message : "");
    }
    
    /* Send alerts for high-severity events */
    if (level <= LOG_LEVEL_ERROR) {
        send_alerts(entry);
    }
    
    return 0;
}

/* Log auth event */
int audit_log_auth(uint16_t event_type, uint32_t uid, const char* username,
                   const char* source, uint8_t success) {
    char msg[256];
    strcpy(msg, username);
    
    uint8_t level = success ? LOG_LEVEL_INFO : LOG_LEVEL_WARNING;
    return audit_log(level, AUDIT_CATEGORY_AUTH, event_type, uid, 0,
                     success, source, msg, (const uint8_t*)0, 0);
}

/* Log system event */
int audit_log_system(uint16_t event_type, const char* message, uint8_t success) {
    return audit_log(LOG_LEVEL_INFO, AUDIT_CATEGORY_SYSTEM, event_type,
                     0, 0, success, "kernel", message, (const uint8_t*)0, 0);
}

/* Log network event */
int audit_log_network(uint16_t event_type, const char* source_ip,
                      const char* dest_ip, uint16_t port, uint8_t success) {
    char msg[256];
    strcpy(msg, source_ip);
    strcat(msg, " -> ");
    strcat(msg, dest_ip);
    
    return audit_log(LOG_LEVEL_INFO, AUDIT_CATEGORY_NETWORK, event_type,
                     0, 0, success, source_ip, msg, (const uint8_t*)0, 0);
}

/* Log file event */
int audit_log_file(uint16_t event_type, uint32_t uid, const char* filename,
                   const char* operation, uint8_t success) {
    char msg[256];
    strcpy(msg, operation);
    strcat(msg, " ");
    strcat(msg, filename);
    
    return audit_log(LOG_LEVEL_INFO, AUDIT_CATEGORY_FILE, event_type,
                     uid, 0, success, "filesystem", msg, (const uint8_t*)0, 0);
}

/* Log process event */
int audit_log_process(uint16_t event_type, uint32_t uid, uint32_t pid,
                      const char* process_name, uint8_t success) {
    char msg[256];
    strcpy(msg, process_name);
    
    return audit_log(LOG_LEVEL_INFO, AUDIT_CATEGORY_PROCESS, event_type,
                     uid, pid, success, "kernel", msg, (const uint8_t*)0, 0);
}

/* Add filter rule */
int audit_add_rule(uint8_t category, uint16_t event_type, uint8_t level, uint8_t action) {
    if (audit_ctx.rule_count >= MAX_LOG_RULES) {
        println("Error: maximum rules reached");
        return -1;
    }
    
    log_rule_t* rule = &audit_ctx.rules[audit_ctx.rule_count++];
    rule->enabled = 1;
    rule->category = category;
    rule->event_type = event_type;
    rule->level = level;
    rule->action = action;
    
    return 0;
}

/* Remove rule */
int audit_remove_rule(uint32_t rule_index) {
    if (rule_index >= audit_ctx.rule_count) return -1;
    
    for (uint32_t i = rule_index; i < audit_ctx.rule_count - 1; i++) {
        audit_ctx.rules[i] = audit_ctx.rules[i + 1];
    }
    audit_ctx.rule_count--;
    
    return 0;
}

/* Add alert recipient */
int audit_add_recipient(const char* name, uint8_t method, const char* destination) {
    if (audit_ctx.recipient_count >= MAX_ALERT_RECIPIENTS) {
        println("Error: maximum recipients reached");
        return -1;
    }
    
    alert_recipient_t* recip = &audit_ctx.recipients[audit_ctx.recipient_count++];
    strcpy(recip->name, name);
    recip->enabled = 1;
    recip->method = method;
    strcpy(recip->destination, destination);
    
    return 0;
}

/* Remove recipient */
int audit_remove_recipient(const char* name) {
    for (uint32_t i = 0; i < audit_ctx.recipient_count; i++) {
        if (strcmp(audit_ctx.recipients[i].name, name) == 0) {
            for (uint32_t j = i; j < audit_ctx.recipient_count - 1; j++) {
                audit_ctx.recipients[j] = audit_ctx.recipients[j + 1];
            }
            audit_ctx.recipient_count--;
            return 0;
        }
    }
    return -1;
}

/* Search by category */
int audit_search_category(uint8_t category, uint32_t* results, uint32_t max_results) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < audit_ctx.entry_count && count < max_results; i++) {
        if (audit_ctx.entries[i].category == category) {
            results[count++] = audit_ctx.entries[i].id;
        }
    }
    return count;
}

/* Search by user */
int audit_search_user(uint32_t uid, uint32_t* results, uint32_t max_results) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < audit_ctx.entry_count && count < max_results; i++) {
        if (audit_ctx.entries[i].uid == uid) {
            results[count++] = audit_ctx.entries[i].id;
        }
    }
    return count;
}

/* Search by event type */
int audit_search_event(uint16_t event_type, uint32_t* results, uint32_t max_results) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < audit_ctx.entry_count && count < max_results; i++) {
        if (audit_ctx.entries[i].event_type == event_type) {
            results[count++] = audit_ctx.entries[i].id;
        }
    }
    return count;
}

/* Search by time range */
int audit_search_time(uint32_t start_time, uint32_t end_time, uint32_t* results, uint32_t max_results) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < audit_ctx.entry_count && count < max_results; i++) {
        if (audit_ctx.entries[i].timestamp >= start_time &&
            audit_ctx.entries[i].timestamp <= end_time) {
            results[count++] = audit_ctx.entries[i].id;
        }
    }
    return count;
}

/* Get entry by ID */
int audit_get_entry(uint32_t id, log_entry_t* entry_out) {
    for (uint32_t i = 0; i < audit_ctx.entry_count; i++) {
        if (audit_ctx.entries[i].id == id) {
            memcpy(entry_out, &audit_ctx.entries[i], sizeof(log_entry_t));
            return 0;
        }
    }
    return -1;
}

/* Rotate logs */
void audit_rotate(void) {
    uint32_t keep = MAX_LOG_ENTRIES / 2;
    if (audit_ctx.entry_count <= keep) return;
    
    /* Move recent entries to front */
    uint32_t start = audit_ctx.entry_count - keep;
    for (uint32_t i = 0; i < keep; i++) {
        audit_ctx.entries[i] = audit_ctx.entries[start + i];
    }
    audit_ctx.entry_count = keep;
    
    println("Audit logs rotated");
}

/* Set level */
void audit_set_level(uint8_t level) {
    audit_ctx.level_filter = level;
    print("Log level set to: ");
    println(get_level_str(level));
}

/* Set console */
void audit_set_console(uint8_t enabled) {
    audit_ctx.console_output = enabled;
}

/* Set retention */
void audit_set_retention(uint32_t days) {
    audit_ctx.retention_days = days;
}

/* Show status */
void audit_status(void) {
    println("");
    println("=== Audit System Status ===");
    println("");
    
    print("  Enabled:         ");
    println(audit_ctx.enabled ? "Yes" : "No");
    print("  Console Output:  ");
    println(audit_ctx.console_output ? "Yes" : "No");
    print("  Level Filter:    ");
    println(get_level_str(audit_ctx.level_filter));
    print("  Retention:       ");
    print_int(audit_ctx.retention_days);
    println(" days");
    println("");
    
    print("  Total Events:    ");
    print_int(audit_ctx.total_events);
    println("");
    print("  Stored Entries:  ");
    print_int(audit_ctx.entry_count);
    println("");
    print("  Filtered Events: ");
    print_int(audit_ctx.filtered_events);
    println("");
    print("  Security Events: ");
    print_int(audit_ctx.security_events);
    println("");
    print("  Rules:           ");
    print_int(audit_ctx.rule_count);
    println("");
    print("  Recipients:      ");
    print_int(audit_ctx.recipient_count);
    println("");
}

/* Show recent logs */
void audit_show_recent(uint32_t count) {
    println("");
    println("=== Recent Audit Logs ===");
    println("");
    
    uint32_t start = 0;
    if (audit_ctx.entry_count > count) {
        start = audit_ctx.entry_count - count;
    }
    
    for (uint32_t i = start; i < audit_ctx.entry_count; i++) {
        log_entry_t* e = &audit_ctx.entries[i];
        
        char time_str[16];
        format_timestamp(e->timestamp, time_str);
        
        print("[");
        print(time_str);
        print("] ");
        print(get_level_str(e->level));
        print(" ");
        print(get_event_str(e->event_type));
        print(": ");
        println(e->message);
    }
    println("");
}

/* Show by category */
void audit_show_category(uint8_t category) {
    println("");
    print("=== Logs for Category: ");
    print(get_category_str(category));
    println(" ===");
    println("");
    
    for (uint32_t i = 0; i < audit_ctx.entry_count; i++) {
        if (audit_ctx.entries[i].category == category) {
            log_entry_t* e = &audit_ctx.entries[i];
            print("[");
            print(get_event_str(e->event_type));
            print("] ");
            println(e->message);
        }
    }
    println("");
}

/* Show by user */
void audit_show_user(uint32_t uid) {
    println("");
    print("=== Logs for User: ");
    print_int(uid);
    println(" ===");
    println("");
    
    for (uint32_t i = 0; i < audit_ctx.entry_count; i++) {
        if (audit_ctx.entries[i].uid == uid) {
            log_entry_t* e = &audit_ctx.entries[i];
            print("[");
            print(get_event_str(e->event_type));
            print("] ");
            println(e->message);
        }
    }
    println("");
}

/* Show security events */
void audit_show_security(void) {
    audit_show_category(AUDIT_CATEGORY_SECURITY);
}

/* Show failures */
void audit_show_failures(void) {
    println("");
    println("=== Failed Events ===");
    println("");
    
    for (uint32_t i = 0; i < audit_ctx.entry_count; i++) {
        if (!audit_ctx.entries[i].success) {
            log_entry_t* e = &audit_ctx.entries[i];
            print("[");
            print(get_event_str(e->event_type));
            print("] UID:");
            print_int(e->uid);
            print(" - ");
            println(e->message);
        }
    }
    println("");
}

/* Show statistics */
void audit_statistics(void) {
    println("");
    println("=== Audit Statistics ===");
    println("");
    
    /* Count by category */
    uint32_t cat_count[8] = {0};
    for (uint32_t i = 0; i < audit_ctx.entry_count; i++) {
        if (audit_ctx.entries[i].category < 8) {
            cat_count[audit_ctx.entries[i].category]++;
        }
    }
    
    println("  Events by Category:");
    for (int i = 1; i < 8; i++) {
        if (cat_count[i] > 0) {
            print("    ");
            print(get_category_str(i));
            print(": ");
            print_int(cat_count[i]);
            println("");
        }
    }
    println("");
    
    /* Count failures */
    uint32_t failures = 0;
    for (uint32_t i = 0; i < audit_ctx.entry_count; i++) {
        if (!audit_ctx.entries[i].success) failures++;
    }
    
    print("  Success Rate: ");
    if (audit_ctx.total_events > 0) {
        uint32_t rate = 100 - (failures * 100 / audit_ctx.total_events);
        print_int(rate);
        println("%");
    } else {
        println("N/A");
    }
    println("");
}

/* Clear logs */
void audit_clear(void) {
    audit_ctx.entry_count = 0;
    audit_ctx.next_id = 1;
    println("Audit logs cleared");
}

/* Self-test */
int audit_selftest(void) {
    println("Running audit self-test...");
    
    int errors = 0;
    
    /* Test logging */
    if (audit_log(LOG_LEVEL_INFO, AUDIT_CATEGORY_SYSTEM, EVENT_BOOT,
                  0, 0, 1, "kernel", "System booted", (const uint8_t*)0, 0) == 0) {
        println("  Basic logging:    OK");
    } else {
        println("  Basic logging:    FAILED");
        errors++;
    }
    
    /* Test auth logging */
    if (audit_log_auth(EVENT_LOGIN, 1000, "testuser", "127.0.0.1", 1) == 0) {
        println("  Auth logging:     OK");
    } else {
        println("  Auth logging:     FAILED");
        errors++;
    }
    
    /* Test search */
    uint32_t results[10];
    int count = audit_search_category(AUDIT_CATEGORY_SYSTEM, results, 10);
    if (count > 0) {
        println("  Search:           OK");
    } else {
        println("  Search:           FAILED");
        errors++;
    }
    
    /* Test rule */
    if (audit_add_rule(AUDIT_CATEGORY_AUTH, 0, LOG_LEVEL_WARNING, 1) == 0) {
        println("  Filter rule:      OK");
        audit_remove_rule(audit_ctx.rule_count - 1);
    } else {
        println("  Filter rule:      FAILED");
        errors++;
    }
    
    println("");
    if (errors == 0) {
        println("All audit self-tests passed!");
    } else {
        print_int(errors);
        println(" self-tests failed");
    }
    
    return errors;
}
