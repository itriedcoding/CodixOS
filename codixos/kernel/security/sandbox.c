/*
 * CodixOS Sandbox/Isolation Implementation
 * Process isolation and capability-based security
 */

#include "sandbox.h"

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global context */
static sandbox_ctx_t sandbox_ctx;

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

/* Find sandbox by ID */
static sandbox_t* find_sandbox(uint32_t id) {
    for (uint32_t i = 0; i < sandbox_ctx.sandbox_count; i++) {
        if (sandbox_ctx.sandboxes[i].id == id) {
            return &sandbox_ctx.sandboxes[i];
        }
    }
    return (sandbox_t*)0;
}

/* Initialize sandbox system */
void sandbox_init(void) {
    memset(&sandbox_ctx, 0, sizeof(sandbox_ctx_t));
    sandbox_ctx.next_sandbox_id = 1;
    sandbox_ctx.next_namespace_id = 1;
    
    println("Sandbox system initialized");
}

/* Create sandbox */
int sandbox_create(const char* name, uint8_t type) {
    if (!name) {
        println("Error: name required");
        return -1;
    }
    
    if (sandbox_ctx.sandbox_count >= MAX_SANDBOXES) {
        println("Error: maximum sandboxes reached");
        return -1;
    }
    
    sandbox_t* sb = &sandbox_ctx.sandboxes[sandbox_ctx.sandbox_count++];
    memset(sb, 0, sizeof(sandbox_t));
    
    sb->id = sandbox_ctx.next_sandbox_id++;
    strcpy(sb->name, name);
    sb->type = type;
    sb->status = 0;
    
    /* Default: drop all capabilities */
    sb->capabilities.permitted = 0;
    sb->capabilities.effective = 0;
    sb->capabilities.inheritable = 0;
    sb->capabilities.bounding = 0xFFFFFFFFFFFFFFFF;
    
    /* Default resource limits */
    sb->memory_limit = 512 * 1024 * 1024; /* 512MB */
    sb->cpu_shares = 1024;
    sb->pids_limit = 256;
    
    println("Sandbox created:");
    print("  ID:   ");
    print_int(sb->id);
    println("");
    print("  Name: ");
    println(name);
    
    return sb->id;
}

/* Destroy sandbox */
int sandbox_destroy(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    if (sb->status == 1) {
        println("Error: stop sandbox first");
        return -1;
    }
    
    /* Remove by shifting */
    uint32_t idx = sb - sandbox_ctx.sandboxes;
    for (uint32_t i = idx; i < sandbox_ctx.sandbox_count - 1; i++) {
        sandbox_ctx.sandboxes[i] = sandbox_ctx.sandboxes[i + 1];
    }
    sandbox_ctx.sandbox_count--;
    
    println("Sandbox destroyed");
    return 0;
}

/* Start sandbox */
int sandbox_start(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    if (sb->status == 1) {
        println("Sandbox already running");
        return 0;
    }
    
    sb->status = 1;
    println("Sandbox started");
    return 0;
}

/* Stop sandbox */
int sandbox_stop(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    sb->status = 0;
    println("Sandbox stopped");
    return 0;
}

/* Execute in sandbox */
int sandbox_exec(uint32_t sandbox_id, const char* command) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    if (sb->status != 1) {
        println("Error: sandbox not running");
        return -1;
    }
    
    print("Executing in sandbox ");
    print_int(sandbox_id);
    print(": ");
    println(command);
    
    /* In production, would create isolated process */
    return 0;
}

/* Add capability */
int sandbox_add_capability(uint32_t sandbox_id, uint64_t cap) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    sb->capabilities.permitted |= cap;
    sb->capabilities.effective |= cap;
    
    return 0;
}

/* Remove capability */
int sandbox_remove_capability(uint32_t sandbox_id, uint64_t cap) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    sb->capabilities.permitted &= ~cap;
    sb->capabilities.effective &= ~cap;
    
    return 0;
}

/* Drop all capabilities */
int sandbox_drop_all_caps(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    sb->capabilities.permitted = 0;
    sb->capabilities.effective = 0;
    sb->capabilities.inheritable = 0;
    
    println("All capabilities dropped");
    return 0;
}

/* Set capabilities */
int sandbox_set_caps(uint32_t sandbox_id, const cap_set_t* caps) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    memcpy(&sb->capabilities, caps, sizeof(cap_set_t));
    return 0;
}

/* Get capabilities */
int sandbox_get_caps(uint32_t sandbox_id, cap_set_t* caps_out) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    memcpy(caps_out, &sb->capabilities, sizeof(cap_set_t));
    return 0;
}

/* Add namespace */
int sandbox_add_namespace(uint32_t sandbox_id, uint8_t ns_type) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    if (sb->namespace_count >= MAX_NAMESPACES) {
        println("Error: maximum namespaces reached");
        return -1;
    }
    
    sb->namespaces[sb->namespace_count++] = ns_type;
    return 0;
}

/* Remove namespace */
int sandbox_remove_namespace(uint32_t sandbox_id, uint8_t ns_type) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    for (uint32_t i = 0; i < sb->namespace_count; i++) {
        if (sb->namespaces[i] == ns_type) {
            for (uint32_t j = i; j < sb->namespace_count - 1; j++) {
                sb->namespaces[j] = sb->namespaces[j + 1];
            }
            sb->namespace_count--;
            return 0;
        }
    }
    
    return -1;
}

/* Set resource limit */
int sandbox_set_rlimit(uint32_t sandbox_id, uint32_t type, uint64_t soft, uint64_t hard) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    if (sb->rlimit_count >= MAX_RESOURCE_LIMITS) {
        println("Error: maximum limits reached");
        return -1;
    }
    
    rlimit_t* rl = &sb->rlimits[sb->rlimit_count++];
    rl->type = type;
    rl->soft_limit = soft;
    rl->hard_limit = hard;
    
    return 0;
}

/* Get resource limit */
int sandbox_get_rlimit(uint32_t sandbox_id, uint32_t type, rlimit_t* rlimit_out) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    for (uint32_t i = 0; i < sb->rlimit_count; i++) {
        if (sb->rlimits[i].type == type) {
            memcpy(rlimit_out, &sb->rlimits[i], sizeof(rlimit_t));
            return 0;
        }
    }
    
    return -1;
}

/* Set memory limit */
int sandbox_set_memory_limit(uint32_t sandbox_id, uint64_t limit) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    sb->memory_limit = limit;
    return 0;
}

/* Set CPU limit */
int sandbox_set_cpu_limit(uint32_t sandbox_id, uint32_t shares) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    sb->cpu_shares = shares;
    return 0;
}

/* Set PID limit */
int sandbox_set_pids_limit(uint32_t sandbox_id, uint32_t limit) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    sb->pids_limit = limit;
    return 0;
}

/* Set readonly */
void sandbox_set_readonly(uint32_t sandbox_id, int readonly) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (sb) {
        sb->readonly_rootfs = readonly;
    }
}

/* Set no_new_privs */
void sandbox_set_no_new_privs(uint32_t sandbox_id, int enable) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (sb) {
        sb->no_new_privs = enable;
    }
}

/* Enable seccomp */
void sandbox_enable_seccomp(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (sb) {
        sb->seccomp_enabled = 1;
    }
}

/* Set AppArmor */
int sandbox_set_apparmor(uint32_t sandbox_id, const char* profile) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    strcpy((char*)sb->apparmor_profile, profile);
    return 0;
}

/* Set SELinux */
int sandbox_set_selinux(uint32_t sandbox_id, const char* context) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    strcpy((char*)sb->selinux_context, context);
    return 0;
}

/* Set rootfs */
int sandbox_set_rootfs(uint32_t sandbox_id, const char* rootfs) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    strcpy(sb->rootfs, rootfs);
    return 0;
}

/* Get status */
int sandbox_get_status(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) return -1;
    return sb->status;
}

/* Get info */
int sandbox_get_info(uint32_t sandbox_id, sandbox_t* info_out) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return -1;
    }
    
    memcpy(info_out, sb, sizeof(sandbox_t));
    return 0;
}

/* List sandboxes */
void sandbox_list(void) {
    println("");
    println("=== Sandboxes ===");
    println("");
    
    if (sandbox_ctx.sandbox_count == 0) {
        println("  No sandboxes");
    } else {
        println("  ID   NAME                 TYPE        STATUS");
        println("  ---  ----                 ----        ------");
        
        for (uint32_t i = 0; i < sandbox_ctx.sandbox_count; i++) {
            sandbox_t* sb = &sandbox_ctx.sandboxes[i];
            
            print("  ");
            print_int(sb->id);
            print("    ");
            print(sb->name);
            for (int p = strlen(sb->name); p < 21; p++) print(" ");
            
            switch (sb->type) {
                case SANDBOX_TYPE_NONE:      print("none        "); break;
                case SANDBOX_TYPE_CHROOT:    print("chroot      "); break;
                case SANDBOX_TYPE_NAMESPACE: print("namespace   "); break;
                case SANDBOX_TYPE_CONTAINER: print("container   "); break;
                case SANDBOX_TYPE_VM:        print("vm          "); break;
            }
            
            println(sb->status ? "RUNNING" : "STOPPED");
        }
    }
    println("");
}

/* Show sandbox details */
void sandbox_show(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    if (!sb) {
        println("Error: sandbox not found");
        return;
    }
    
    println("");
    print("=== Sandbox: ");
    print(sb->name);
    println(" ===");
    println("");
    
    print("  ID:        ");
    print_int(sb->id);
    println("");
    print("  Status:    ");
    println(sb->status ? "RUNNING" : "STOPPED");
    print("  Type:      ");
    switch (sb->type) {
        case SANDBOX_TYPE_NONE:      println("none"); break;
        case SANDBOX_TYPE_CHROOT:    println("chroot"); break;
        case SANDBOX_TYPE_NAMESPACE: println("namespace"); break;
        case SANDBOX_TYPE_CONTAINER: println("container"); break;
        case SANDBOX_TYPE_VM:        println("vm"); break;
    }
    
    println("");
    println("  Capabilities:");
    print("    Permitted:   0x");
    print_int(sb->capabilities.permitted);
    println("");
    print("    Effective:   0x");
    print_int(sb->capabilities.effective);
    println("");
    
    println("");
    println("  Resource Limits:");
    print("    Memory:      ");
    print_int(sb->memory_limit / (1024 * 1024));
    println(" MB");
    print("    CPU Shares:  ");
    print_int(sb->cpu_shares);
    println("");
    print("    PIDs:        ");
    print_int(sb->pids_limit);
    println("");
    
    println("");
    print("  Namespaces:  ");
    print_int(sb->namespace_count);
    println("");
    print("  Seccomp:     ");
    println(sb->seccomp_enabled ? "Enabled" : "Disabled");
    print("  No New Privs:");
    println(sb->no_new_privs ? "Yes" : "No");
    print("  Read-only:   ");
    println(sb->readonly_rootfs ? "Yes" : "No");
    
    if (sb->apparmor_profile[0]) {
        print("  AppArmor:    ");
        println((char*)sb->apparmor_profile);
    }
    if (sb->selinux_context[0]) {
        print("  SELinux:     ");
        println((char*)sb->selinux_context);
    }
    println("");
}

/* Check if running */
int sandbox_is_running(uint32_t sandbox_id) {
    sandbox_t* sb = find_sandbox(sandbox_id);
    return sb ? sb->status : 0;
}

/* Get by name */
int sandbox_get_by_name(const char* name) {
    for (uint32_t i = 0; i < sandbox_ctx.sandbox_count; i++) {
        if (strcmp(sandbox_ctx.sandboxes[i].name, name) == 0) {
            return sandbox_ctx.sandboxes[i].id;
        }
    }
    return -1;
}

/* Self-test */
int sandbox_selftest(void) {
    println("Running sandbox self-test...");
    
    int errors = 0;
    
    /* Test create */
    int sb_id = sandbox_create("test-sandbox", SANDBOX_TYPE_NAMESPACE);
    if (sb_id > 0) {
        println("  Create:           OK");
        
        /* Test capabilities */
        if (sandbox_add_capability(sb_id, CAP_NET_BIND_SERVICE) == 0) {
            println("  Add capability:   OK");
        } else {
            println("  Add capability:   FAILED");
            errors++;
        }
        
        /* Test namespaces */
        if (sandbox_add_namespace(sb_id, NS_TYPE_NET) == 0) {
            println("  Add namespace:    OK");
        } else {
            println("  Add namespace:    FAILED");
            errors++;
        }
        
        /* Test resource limits */
        if (sandbox_set_memory_limit(sb_id, 256 * 1024 * 1024) == 0) {
            println("  Set memory:       OK");
        } else {
            println("  Set memory:       FAILED");
            errors++;
        }
        
        /* Test start/stop */
        if (sandbox_start(sb_id) == 0 && sandbox_stop(sb_id) == 0) {
            println("  Start/stop:       OK");
        } else {
            println("  Start/stop:       FAILED");
            errors++;
        }
        
        /* Cleanup */
        sandbox_destroy(sb_id);
    } else {
        println("  Create:           FAILED");
        errors++;
    }
    
    println("");
    if (errors == 0) {
        println("All sandbox self-tests passed!");
    } else {
        print_int(errors);
        println(" self-tests failed");
    }
    
    return errors;
}
