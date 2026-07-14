/*
 * CodixOS System Hardening Module
 * Disable unnecessary services, ports, and software
 */

#ifndef HARDENING_H
#define HARDENING_H

/* Type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

/* Service states */
#define SERVICE_STATE_DISABLED    0
#define SERVICE_STATE_ENABLED     1
#define SERVICE_STATE_MASKED      2

/* Port states */
#define PORT_STATE_CLOSED         0
#define PORT_STATE_OPEN           1
#define PORT_STATE_FILTERED       2

/* Hardening levels */
#define HARDEN_LEVEL_NONE         0
#define HARDEN_LEVEL_LOW          1
#define HARDEN_LEVEL_MEDIUM       2
#define HARDEN_LEVEL_HIGH         3
#define HARDEN_LEVEL_PARANOID     4

/* Maximum counts */
#define MAX_SERVICES              64
#define MAX_PORTS                 32
#define MAX_HARDEN_RULES          128
#define MAX_SOFTWARE_BLACKLIST    32

/* Service structure */
typedef struct {
    char        name[64];
    uint8_t     state;
    uint8_t     required;       /* Required for system operation */
    uint8_t     security_risk;  /* 0=safe, 1=low, 2=medium, 3=high */
    char        description[128];
} service_t;

/* Port rule structure */
typedef struct {
    uint16_t    port;
    uint8_t     protocol;       /* 0=TCP, 1=UDP, 2=BOTH */
    uint8_t     state;
    char        service[64];    /* Service name */
    char        allowed_sources[128]; /* Comma-separated IPs or "*" for all */
} port_rule_t;

/* Hardening rule structure */
typedef struct {
    char        category[32];
    char        name[64];
    uint8_t     enabled;
    char        description[128];
} harden_rule_t;

/* System hardening context */
typedef struct {
    service_t       services[MAX_SERVICES];
    uint32_t        service_count;
    port_rule_t     ports[MAX_PORTS];
    uint32_t        port_count;
    harden_rule_t   rules[MAX_HARDEN_RULES];
    uint32_t        rule_count;
    char            blacklist[MAX_SOFTWARE_BLACKLIST][64];
    uint32_t        blacklist_count;
    uint8_t         hardening_level;
    uint8_t         firewall_enabled;
    uint8_t         selinux_enabled;    /* Or AppArmor */
    uint8_t         audit_enabled;
    uint8_t         ptrace_scope;       /* 0=unrestricted, 1=restricted, 2=disabled */
    uint32_t        kernel_modules_disabled; /* Number of disabled modules */
    uint32_t        open_ports;
    uint32_t        enabled_services;
} harden_ctx_t;

/*
 * Initialize system hardening
 */
void hardening_init(void);

/*
 * Set hardening level (applies predefined rules)
 */
void hardening_set_level(uint8_t level);

/*
 * Get current hardening level
 */
uint8_t hardening_get_level(void);

/*
 * Enable/disable a service
 */
int hardening_service_enable(const char* name);
int hardening_service_disable(const char* name);
int hardening_service_mask(const char* name);

/*
 * Get service status
 */
int hardening_service_status(const char* name);

/*
 * Add port rule
 */
int hardening_add_port_rule(uint16_t port, uint8_t protocol, const char* service, const char* sources);

/*
 * Remove port rule
 */
int hardening_remove_port_rule(uint16_t port, uint8_t protocol);

/*
 * Enable/disable port
 */
int hardening_port_enable(uint16_t port);
int hardening_port_disable(uint16_t port);

/*
 * Add software to blacklist
 */
int hardening_blacklist_add(const char* software);

/*
 * Remove software from blacklist
 */
int hardening_blacklist_remove(const char* software);

/*
 * Check if software is blacklisted
 */
int hardening_is_blacklisted(const char* software);

/*
 * Enable/disable firewall
 */
void hardening_firewall_enable(void);
void hardening_firewall_disable(void);

/*
 * Enable/disable SELinux/AppArmor
 */
void hardening_selinux_enable(void);
void hardening_selinux_disable(void);

/*
 * Set ptrace scope
 */
void hardening_set_ptrace_scope(uint8_t scope);

/*
 * Disable kernel modules
 */
int hardening_disable_module(const char* module_name);

/*
 * Enable kernel module
 */
int hardening_enable_module(const char* module_name);

/*
 * Apply a specific hardening rule
 */
int hardening_apply_rule(const char* category, const char* name, uint8_t enable);

/*
 * Get system security score (0-100)
 */
uint32_t hardening_get_score(void);

/*
 * Show hardening status
 */
void hardening_status(void);

/*
 * Show all services
 */
void hardening_list_services(void);

/*
 * Show all port rules
 */
void hardening_list_ports(void);

/*
 * Show blacklisted software
 */
void hardening_list_blacklist(void);

/*
 * Show all hardening rules
 */
void hardening_list_rules(void);

/*
 * Export hardening configuration
 */
int hardening_export_config(const char* filename);

/*
 * Import hardening configuration
 */
int hardening_import_config(const char* filename);

/*
 * Audit system configuration
 */
void hardening_audit(void);

/*
 * Self-test
 */
int hardening_selftest(void);

#endif /* HARDENING_H */
