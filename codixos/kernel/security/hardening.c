/*
 * CodixOS System Hardening Implementation
 * Disable unnecessary services, ports, and software
 */

#include "hardening.h"

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global context */
static harden_ctx_t harden_ctx;

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
extern size_t strlen(const char* str);

/* Find service by name */
static service_t* find_service(const char* name) {
    for (uint32_t i = 0; i < harden_ctx.service_count; i++) {
        if (strcmp(harden_ctx.services[i].name, name) == 0) {
            return &harden_ctx.services[i];
        }
    }
    return (service_t*)0;
}

/* Find port rule */
static port_rule_t* find_port_rule(uint16_t port, uint8_t protocol) {
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        if (harden_ctx.ports[i].port == port && harden_ctx.ports[i].protocol == protocol) {
            return &harden_ctx.ports[i];
        }
    }
    return (port_rule_t*)0;
}

/* Find hardening rule */
static harden_rule_t* find_rule(const char* category, const char* name) {
    for (uint32_t i = 0; i < harden_ctx.rule_count; i++) {
        if (strcmp(harden_ctx.rules[i].category, category) == 0 &&
            strcmp(harden_ctx.rules[i].name, name) == 0) {
            return &harden_ctx.rules[i];
        }
    }
    return (harden_rule_t*)0;
}

/* Add predefined services */
static void add_default_services(void) {
    const char* services[][3] = {
        {"sshd", "1", "1", "SSH Daemon"},
        {"cron", "1", "0", "Cron Scheduler"},
        {"syslog", "1", "0", "System Logger"},
        {"networkd", "1", "0", "Network Manager"},
        {"firewalld", "1", "0", "Firewall Service"},
        {"httpd", "0", "1", "Web Server"},
        {"ftpd", "0", "3", "FTP Server"},
        {"telnetd", "0", "3", "Telnet Server"},
        {"sendmail", "0", "2", "Mail Server"},
        {"named", "0", "2", "DNS Server"},
        {"smbd", "0", "2", "Samba File Server"},
        {"nfsd", "0", "2", "NFS Server"},
        {"rpcbind", "0", "2", "RPC Portmapper"},
        {"avahi-daemon", "0", "2", "mDNS/DNS-SD"},
        {"cupsd", "0", "1", "Print Service"},
        {"bluetoothd", "0", "1", "Bluetooth Service"},
        {"wpa_supplicant", "0", "1", "WiFi Authentication"},
    };
    
    for (int i = 0; i < 17; i++) {
        if (harden_ctx.service_count >= MAX_SERVICES) break;
        
        service_t* svc = &harden_ctx.services[harden_ctx.service_count++];
        strcpy(svc->name, services[i][0]);
        svc->state = services[i][1][0] - '0';
        svc->required = services[i][1][0] - '0';
        svc->security_risk = services[i][2][0] - '0';
        strcpy(svc->description, services[i][3]);
        
        if (svc->state) harden_ctx.enabled_services++;
    }
}

/* Add default port rules */
static void add_default_ports(void) {
    uint16_t safe_ports[] = {22, 80, 443, 53, 123};
    uint16_t risky_ports[] = {21, 23, 25, 110, 135, 139, 445, 993, 995, 3389, 5900};
    
    for (int i = 0; i < 5; i++) {
        if (harden_ctx.port_count >= MAX_PORTS) break;
        port_rule_t* rule = &harden_ctx.ports[harden_ctx.port_count++];
        rule->port = safe_ports[i];
        rule->protocol = 0; /* TCP */
        rule->state = PORT_STATE_OPEN;
        strcpy(rule->service, "safe");
        strcpy(rule->allowed_sources, "*");
    }
    
    for (int i = 0; i < 11; i++) {
        if (harden_ctx.port_count >= MAX_PORTS) break;
        port_rule_t* rule = &harden_ctx.ports[harden_ctx.port_count++];
        rule->port = risky_ports[i];
        rule->protocol = 0;
        rule->state = PORT_STATE_CLOSED;
        strcpy(rule->service, "risky");
        strcpy(rule->allowed_sources, "");
    }
    
    harden_ctx.open_ports = 5;
}

/* Add default hardening rules */
static void add_default_rules(void) {
    const char* rules[][4] = {
        {"kernel", "module_loading", "1", "Disable kernel module loading"},
        {"kernel", "dmesg_restrict", "1", "Restrict dmesg access"},
        {"kernel", "kptr_restrict", "1", "Restrict kernel pointers"},
        {"kernel", "yama_ptrace", "1", "Enable Yama ptrace scope"},
        {"filesystem", "suid_bit", "1", "Restrict SUID binaries"},
        {"filesystem", "world_writable", "1", "Check world-writable files"},
        {"filesystem", "tmp_noexec", "1", "Mount /tmp noexec"},
        {"filesystem", "dev_shm_noexec", "1", "Mount /dev/shm noexec"},
        {"network", "ipv4_forwarding", "0", "Disable IPv4 forwarding"},
        {"network", "ipv6_forwarding", "0", "Disable IPv6 forwarding"},
        {"network", "icmp_redirects", "0", "Disable ICMP redirects"},
        {"network", "source_routing", "0", "Disable source routing"},
        {"network", "syn_cookies", "1", "Enable SYN cookies"},
        {"network", "tcp_timestamps", "0", "Disable TCP timestamps"},
        {"auth", "password hashing", "1", "Use strong password hashing"},
        {"auth", "login_defs", "1", "Harden login.defs"},
        {"auth", "su_restriction", "1", "Restrict su command"},
        {"auth", "sudo_logging", "1", "Enable sudo logging"},
        {"services", "unnecessary", "1", "Disable unnecessary services"},
        {"services", "xinetd", "1", "Disable xinetd"},
        {"services", "inetd", "1", "Disable inetd"},
    };
    
    for (int i = 0; i < 21; i++) {
        if (harden_ctx.rule_count >= MAX_HARDEN_RULES) break;
        
        harden_rule_t* rule = &harden_ctx.rules[harden_ctx.rule_count++];
        strcpy(rule->category, rules[i][0]);
        strcpy(rule->name, rules[i][1]);
        rule->enabled = rules[i][2][0] - '0';
        strcpy(rule->description, rules[i][3]);
    }
}

/* Apply medium hardening level */
static void apply_medium_level(void) {
    /* Disable risky services */
    const char* disable_svc[] = {"ftpd", "telnetd", "sendmail", "named", "smbd", "nfsd", "rpcbind"};
    for (int i = 0; i < 7; i++) {
        service_t* svc = find_service(disable_svc[i]);
        if (svc) svc->state = SERVICE_STATE_DISABLED;
    }
    
    /* Close risky ports */
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        if (harden_ctx.ports[i].port > 1024 && strcmp(harden_ctx.ports[i].service, "safe") != 0) {
            harden_ctx.ports[i].state = PORT_STATE_CLOSED;
        }
    }
    
    harden_ctx.firewall_enabled = 1;
    harden_ctx.audit_enabled = 1;
    harden_ctx.ptrace_scope = 1;
}

/* Apply high hardening level */
static void apply_high_level(void) {
    apply_medium_level();
    
    /* Disable more services */
    const char* disable_svc[] = {"httpd", "cupsd", "bluetoothd", "avahi-daemon"};
    for (int i = 0; i < 4; i++) {
        service_t* svc = find_service(disable_svc[i]);
        if (svc) svc->state = SERVICE_STATE_DISABLED;
    }
    
    harden_ctx.selinux_enabled = 1;
    harden_ctx.ptrace_scope = 2;
    harden_ctx.kernel_modules_disabled = 10;
}

/* Initialize hardening */
void hardening_init(void) {
    memset(&harden_ctx, 0, sizeof(harden_ctx_t));
    
    add_default_services();
    add_default_ports();
    add_default_rules();
    
    harden_ctx.hardening_level = HARDEN_LEVEL_LOW;
    harden_ctx.firewall_enabled = 1;
    harden_ctx.audit_enabled = 1;
    harden_ctx.ptrace_scope = 0;
    
    println("System hardening initialized");
}

/* Set hardening level */
void hardening_set_level(uint8_t level) {
    if (level > HARDEN_LEVEL_PARANOID) level = HARDEN_LEVEL_PARANOID;
    
    harden_ctx.hardening_level = level;
    
    switch (level) {
        case HARDEN_LEVEL_NONE:
            println("Hardening disabled - not recommended");
            break;
        case HARDEN_LEVEL_LOW:
            println("Low hardening applied");
            break;
        case HARDEN_LEVEL_MEDIUM:
            apply_medium_level();
            println("Medium hardening applied");
            break;
        case HARDEN_LEVEL_HIGH:
            apply_high_level();
            println("High hardening applied");
            break;
        case HARDEN_LEVEL_PARANOID:
            apply_high_level();
            /* Additional paranoid measures */
            for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
                if (harden_ctx.ports[i].port != 22) {
                    harden_ctx.ports[i].state = PORT_STATE_CLOSED;
                }
            }
            println("Paranoid hardening applied");
            break;
    }
}

/* Get hardening level */
uint8_t hardening_get_level(void) {
    return harden_ctx.hardening_level;
}

/* Enable service */
int hardening_service_enable(const char* name) {
    service_t* svc = find_service(name);
    if (!svc) {
        println("Error: service not found");
        return -1;
    }
    
    svc->state = SERVICE_STATE_ENABLED;
    harden_ctx.enabled_services++;
    return 0;
}

/* Disable service */
int hardening_service_disable(const char* name) {
    service_t* svc = find_service(name);
    if (!svc) {
        println("Error: service not found");
        return -1;
    }
    
    if (svc->required) {
        println("Error: required service cannot be disabled");
        return -1;
    }
    
    svc->state = SERVICE_STATE_DISABLED;
    harden_ctx.enabled_services--;
    return 0;
}

/* Mask service */
int hardening_service_mask(const char* name) {
    service_t* svc = find_service(name);
    if (!svc) {
        println("Error: service not found");
        return -1;
    }
    
    svc->state = SERVICE_STATE_MASKED;
    return 0;
}

/* Get service status */
int hardening_service_status(const char* name) {
    service_t* svc = find_service(name);
    if (!svc) return -1;
    return svc->state;
}

/* Add port rule */
int hardening_add_port_rule(uint16_t port, uint8_t protocol, const char* service, const char* sources) {
    if (harden_ctx.port_count >= MAX_PORTS) {
        println("Error: maximum port rules reached");
        return -1;
    }
    
    port_rule_t* rule = &harden_ctx.ports[harden_ctx.port_count++];
    rule->port = port;
    rule->protocol = protocol;
    rule->state = PORT_STATE_OPEN;
    strcpy(rule->service, service);
    strcpy(rule->allowed_sources, sources);
    
    return 0;
}

/* Remove port rule */
int hardening_remove_port_rule(uint16_t port, uint8_t protocol) {
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        if (harden_ctx.ports[i].port == port && harden_ctx.ports[i].protocol == protocol) {
            for (uint32_t j = i; j < harden_ctx.port_count - 1; j++) {
                harden_ctx.ports[j] = harden_ctx.ports[j + 1];
            }
            harden_ctx.port_count--;
            return 0;
        }
    }
    return -1;
}

/* Enable port */
int hardening_port_enable(uint16_t port) {
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        if (harden_ctx.ports[i].port == port) {
            harden_ctx.ports[i].state = PORT_STATE_OPEN;
            return 0;
        }
    }
    return -1;
}

/* Disable port */
int hardening_port_disable(uint16_t port) {
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        if (harden_ctx.ports[i].port == port) {
            harden_ctx.ports[i].state = PORT_STATE_CLOSED;
            return 0;
        }
    }
    return -1;
}

/* Add to blacklist */
int hardening_blacklist_add(const char* software) {
    if (harden_ctx.blacklist_count >= MAX_SOFTWARE_BLACKLIST) {
        println("Error: blacklist full");
        return -1;
    }
    
    strcpy(harden_ctx.blacklist[harden_ctx.blacklist_count++], software);
    return 0;
}

/* Remove from blacklist */
int hardening_blacklist_remove(const char* software) {
    for (uint32_t i = 0; i < harden_ctx.blacklist_count; i++) {
        if (strcmp(harden_ctx.blacklist[i], software) == 0) {
            for (uint32_t j = i; j < harden_ctx.blacklist_count - 1; j++) {
                strcpy(harden_ctx.blacklist[j], harden_ctx.blacklist[j + 1]);
            }
            harden_ctx.blacklist_count--;
            return 0;
        }
    }
    return -1;
}

/* Check blacklist */
int hardening_is_blacklisted(const char* software) {
    for (uint32_t i = 0; i < harden_ctx.blacklist_count; i++) {
        if (strcmp(harden_ctx.blacklist[i], software) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Enable firewall */
void hardening_firewall_enable(void) {
    harden_ctx.firewall_enabled = 1;
    println("Firewall enabled");
}

/* Disable firewall */
void hardening_firewall_disable(void) {
    harden_ctx.firewall_enabled = 0;
    println("Firewall disabled");
}

/* Enable SELinux */
void hardening_selinux_enable(void) {
    harden_ctx.selinux_enabled = 1;
    println("SELinux/AppArmor enabled");
}

/* Disable SELinux */
void hardening_selinux_disable(void) {
    harden_ctx.selinux_enabled = 0;
    println("SELinux/AppArmor disabled");
}

/* Set ptrace scope */
void hardening_set_ptrace_scope(uint8_t scope) {
    harden_ctx.ptrace_scope = scope;
    print("Ptrace scope set to: ");
    print_int(scope);
    println("");
}

/* Get security score */
uint32_t hardening_get_score(void) {
    uint32_t score = 100;
    
    /* Deductions */
    if (!harden_ctx.firewall_enabled) score -= 15;
    if (!harden_ctx.selinux_enabled) score -= 10;
    if (!harden_ctx.audit_enabled) score -= 10;
    if (harden_ctx.ptrace_scope < 1) score -= 5;
    
    /* Check for risky open ports */
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        if (harden_ctx.ports[i].state == PORT_STATE_OPEN && 
            strcmp(harden_ctx.ports[i].service, "risky") == 0) {
            score -= 5;
        }
    }
    
    /* Check for risky enabled services */
    for (uint32_t i = 0; i < harden_ctx.service_count; i++) {
        if (harden_ctx.services[i].state == SERVICE_STATE_ENABLED &&
            harden_ctx.services[i].security_risk >= 2) {
            score -= 5;
        }
    }
    
    return score;
}

/* Show status */
void hardening_status(void) {
    println("");
    println("=== System Hardening Status ===");
    println("");
    
    const char* level_str;
    switch (harden_ctx.hardening_level) {
        case HARDEN_LEVEL_NONE:     level_str = "NONE"; break;
        case HARDEN_LEVEL_LOW:      level_str = "LOW"; break;
        case HARDEN_LEVEL_MEDIUM:   level_str = "MEDIUM"; break;
        case HARDEN_LEVEL_HIGH:     level_str = "HIGH"; break;
        case HARDEN_LEVEL_PARANOID: level_str = "PARANOID"; break;
        default:                    level_str = "UNKNOWN"; break;
    }
    
    print("  Level:           ");
    println(level_str);
    print("  Security Score:  ");
    print_int(hardening_get_score());
    println("/100");
    println("");
    
    println("  Components:");
    print("    Firewall:      ");
    println(harden_ctx.firewall_enabled ? "ENABLED" : "DISABLED");
    print("    SELinux:       ");
    println(harden_ctx.selinux_enabled ? "ENABLED" : "DISABLED");
    print("    Audit:         ");
    println(harden_ctx.audit_enabled ? "ENABLED" : "DISABLED");
    print("    Ptrace Scope:  ");
    print_int(harden_ctx.ptrace_scope);
    println("");
    println("");
    
    print("  Services:  ");
    print_int(harden_ctx.enabled_services);
    print("/");
    print_int(harden_ctx.service_count);
    println(" enabled");
    print("  Open Ports: ");
    print_int(harden_ctx.open_ports);
    println("");
    println("");
}

/* List services */
void hardening_list_services(void) {
    println("");
    println("=== Services ===");
    println("");
    
    println("  NAME                 STATE        RISK  DESCRIPTION");
    println("  ----                 -----        ----  -----------");
    
    for (uint32_t i = 0; i < harden_ctx.service_count; i++) {
        service_t* svc = &harden_ctx.services[i];
        print("  ");
        print(svc->name);
        for (int p = strlen(svc->name); p < 21; p++) print(" ");
        
        switch (svc->state) {
            case SERVICE_STATE_DISABLED: print("DISABLED   "); break;
            case SERVICE_STATE_ENABLED:  print("ENABLED    "); break;
            case SERVICE_STATE_MASKED:   print("MASKED     "); break;
        }
        
        print_int(svc->security_risk);
        print("     ");
        println(svc->description);
    }
    println("");
}

/* List ports */
void hardening_list_ports(void) {
    println("");
    println("=== Port Rules ===");
    println("");
    
    println("  PORT   PROTO  STATE     SERVICE   SOURCES");
    println("  ----   -----  -----     -------   -------");
    
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        port_rule_t* rule = &harden_ctx.ports[i];
        print("  ");
        print_int(rule->port);
        print("     ");
        print(rule->protocol == 0 ? "TCP  " : rule->protocol == 1 ? "UDP  " : "BOTH ");
        print("  ");
        
        switch (rule->state) {
            case PORT_STATE_CLOSED:   print("CLOSED    "); break;
            case PORT_STATE_OPEN:     print("OPEN      "); break;
            case PORT_STATE_FILTERED: print("FILTERED  "); break;
        }
        
        print(rule->service);
        print("   ");
        println(rule->allowed_sources);
    }
    println("");
}

/* List blacklist */
void hardening_list_blacklist(void) {
    println("");
    println("=== Software Blacklist ===");
    println("");
    
    if (harden_ctx.blacklist_count == 0) {
        println("  No blacklisted software");
    } else {
        for (uint32_t i = 0; i < harden_ctx.blacklist_count; i++) {
            print("  - ");
            println(harden_ctx.blacklist[i]);
        }
    }
    println("");
}

/* List rules */
void hardening_list_rules(void) {
    println("");
    println("=== Hardening Rules ===");
    println("");
    
    println("  CATEGORY    NAME                 ENABLED  DESCRIPTION");
    println("  --------    ----                 -------  -----------");
    
    for (uint32_t i = 0; i < harden_ctx.rule_count; i++) {
        harden_rule_t* rule = &harden_ctx.rules[i];
        print("  ");
        print(rule->category);
        for (int p = strlen(rule->category); p < 12; p++) print(" ");
        print(rule->name);
        for (int p = strlen(rule->name); p < 21; p++) print(" ");
        print(rule->enabled ? "Yes      " : "No       ");
        println(rule->description);
    }
    println("");
}

/* Audit system */
void hardening_audit(void) {
    println("");
    println("=== Security Audit ===");
    println("");
    
    uint32_t issues = 0;
    
    /* Check firewall */
    if (!harden_ctx.firewall_enabled) {
        println("  [WARN] Firewall is disabled");
        issues++;
    }
    
    /* Check SELinux */
    if (!harden_ctx.selinux_enabled) {
        println("  [WARN] SELinux/AppArmor is disabled");
        issues++;
    }
    
    /* Check risky services */
    for (uint32_t i = 0; i < harden_ctx.service_count; i++) {
        if (harden_ctx.services[i].state == SERVICE_STATE_ENABLED &&
            harden_ctx.services[i].security_risk >= 2) {
            print("  [WARN] Risky service enabled: ");
            println(harden_ctx.services[i].name);
            issues++;
        }
    }
    
    /* Check risky ports */
    for (uint32_t i = 0; i < harden_ctx.port_count; i++) {
        if (harden_ctx.ports[i].state == PORT_STATE_OPEN &&
            strcmp(harden_ctx.ports[i].service, "risky") == 0) {
            print("  [WARN] Risky port open: ");
            print_int(harden_ctx.ports[i].port);
            println("");
            issues++;
        }
    }
    
    if (issues == 0) {
        println("  No security issues found");
    } else {
        print("  Total issues: ");
        print_int(issues);
        println("");
    }
    println("");
}

/* Self-test */
int hardening_selftest(void) {
    println("Running hardening self-test...");
    
    int errors = 0;
    
    /* Test service enable/disable */
    if (hardening_service_disable("ftpd") == 0) {
        println("  Service control:  OK");
    } else {
        println("  Service control:  FAILED");
        errors++;
    }
    
    /* Test port control */
    if (hardening_port_disable(21) == 0) {
        println("  Port control:     OK");
    } else {
        println("  Port control:     FAILED");
        errors++;
    }
    
    /* Test blacklist */
    if (hardening_blacklist_add("malware") == 0) {
        if (hardening_is_blacklisted("malware")) {
            println("  Blacklist:        OK");
        } else {
            println("  Blacklist:        FAILED");
            errors++;
        }
        hardening_blacklist_remove("malware");
    } else {
        println("  Blacklist:        FAILED");
        errors++;
    }
    
    /* Test hardening level */
    hardening_set_level(HARDEN_LEVEL_HIGH);
    if (harden_ctx.hardening_level == HARDEN_LEVEL_HIGH) {
        println("  Hardening level:  OK");
    } else {
        println("  Hardening level:  FAILED");
        errors++;
    }
    
    /* Test security score */
    uint32_t score = hardening_get_score();
    if (score > 0 && score <= 100) {
        println("  Security score:   OK");
    } else {
        println("  Security score:   FAILED");
        errors++;
    }
    
    println("");
    if (errors == 0) {
        println("All hardening self-tests passed!");
    } else {
        print_int(errors);
        println(" self-tests failed");
    }
    
    return errors;
}
