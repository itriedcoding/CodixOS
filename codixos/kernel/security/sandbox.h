/*
 * CodixOS Sandbox/Isolation Module
 * Process isolation and capability-based security
 */

#ifndef SANDBOX_H
#define SANDBOX_H

/* Type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

/* Capability bits */
#define CAP_CHOWN            (1 << 0)
#define CAP_DAC_OVERRIDE     (1 << 1)
#define CAP_DAC_READ_SEARCH  (1 << 2)
#define CAP_FOWNER           (1 << 3)
#define CAP_FSETID           (1 << 4)
#define CAP_KILL             (1 << 5)
#define CAP_SETGID           (1 << 6)
#define CAP_SETUID           (1 << 7)
#define CAP_SETPCAP          (1 << 8)
#define CAP_LINUX_IMMUTABLE  (1 << 9)
#define CAP_NET_BIND_SERVICE (1 << 10)
#define CAP_NET_BROADCAST    (1 << 11)
#define CAP_NET_ADMIN        (1 << 12)
#define CAP_NET_RAW          (1 << 13)
#define CAP_IPC_LOCK         (1 << 14)
#define CAP_IPC_OWNER        (1 << 15)
#define CAP_SYS_MODULE       (1 << 16)
#define CAP_SYS_RAWIO        (1 << 17)
#define CAP_SYS_CHROOT       (1 << 18)
#define CAP_SYS_PTRACE       (1 << 19)
#define CAP_SYS_PACCT        (1 << 20)
#define CAP_SYS_ADMIN        (1 << 21)
#define CAP_SYS_BOOT         (1 << 22)
#define CAP_SYS_NICE         (1 << 23)
#define CAP_SYS_RESOURCE     (1 << 24)
#define CAP_SYS_TIME         (1 << 25)
#define CAP_SYS_TTY_CONFIG   (1 << 26)
#define CAP_MKNOD            (1 << 27)
#define CAP_LEASE            (1 << 28)
#define CAP_AUDIT_WRITE      (1 << 29)
#define CAP_AUDIT_CONTROL    (1 << 30)
#define CAP_SETFCAP          (1 << 31)

/* Sandbox types */
#define SANDBOX_TYPE_NONE       0
#define SANDBOX_TYPE_CHROOT     1
#define SANDBOX_TYPE_NAMESPACE  2
#define SANDBOX_TYPE_CONTAINER  3
#define SANDBOX_TYPE_VM         4

/* Namespace types */
#define NS_TYPE_PID        0x01
#define NS_TYPE_NET        0x02
#define NS_TYPE_MNT        0x04
#define NS_TYPE_USER       0x08
#define NS_TYPE_UTS        0x10
#define NS_TYPE_IPC        0x20
#define NS_TYPE_CGROUP     0x40

/* Resource limits */
#define RLIMIT_CPU         0
#define RLIMIT_FSIZE       1
#define RLIMIT_DATA        2
#define RLIMIT_STACK       3
#define RLIMIT_CORE        4
#define RLIMIT_RSS         5
#define RLIMIT_NPROC       6
#define RLIMIT_NOFILE      7
#define RLIMIT_MEMLOCK     8
#define RLIMIT_AS          9

/* Maximum values */
#define MAX_SANDBOXES      32
#define MAX_NAMESPACES     16
#define MAX_CAPABILITIES   32
#define MAX_RESOURCE_LIMITS 16

/* Capability set */
typedef struct {
    uint64_t permitted;        /* Permitted capabilities */
    uint64_t effective;        /* Effective capabilities */
    uint64_t inheritable;      /* Inheritable capabilities */
    uint64_t bounding;         /* Bounding set */
} cap_set_t;

/* Resource limit */
typedef struct {
    uint32_t type;
    uint64_t soft_limit;
    uint64_t hard_limit;
} rlimit_t;

/* Namespace */
typedef struct {
    uint32_t    id;
    uint8_t     type;
    uint32_t    ref_count;
    char        name[64];
} namespace_t;

/* Sandbox configuration */
typedef struct {
    uint32_t    id;
    char        name[64];
    uint8_t     type;
    uint32_t    pid;
    cap_set_t   capabilities;
    uint32_t    namespace_count;
    uint32_t    namespaces[MAX_NAMESPACES];
    uint32_t    rlimit_count;
    rlimit_t    rlimits[MAX_RESOURCE_LIMITS];
    uint8_t     readonly_rootfs;
    uint8_t     no_new_privs;
    uint8_t     seccomp_enabled;
    uint8_t     apparmor_profile[64];
    uint8_t     selinux_context[64];
    char        rootfs[256];
    uint64_t    memory_limit;
    uint64_t    cpu_shares;
    uint32_t    pids_limit;
    uint32_t    created;
    uint32_t    status;         /* 0=stopped, 1=running */
} sandbox_t;

/* Sandbox context */
typedef struct {
    sandbox_t       sandboxes[MAX_SANDBOXES];
    uint32_t        sandbox_count;
    namespace_t     namespaces[MAX_NAMESPACES];
    uint32_t        namespace_count;
    uint32_t        next_sandbox_id;
    uint32_t        next_namespace_id;
} sandbox_ctx_t;

/*
 * Initialize sandbox system
 */
void sandbox_init(void);

/*
 * Create a new sandbox
 */
int sandbox_create(const char* name, uint8_t type);

/*
 * Destroy a sandbox
 */
int sandbox_destroy(uint32_t sandbox_id);

/*
 * Start a sandbox
 */
int sandbox_start(uint32_t sandbox_id);

/*
 * Stop a sandbox
 */
int sandbox_stop(uint32_t sandbox_id);

/*
 * Execute command in sandbox
 */
int sandbox_exec(uint32_t sandbox_id, const char* command);

/*
 * Add capability to sandbox
 */
int sandbox_add_capability(uint32_t sandbox_id, uint64_t cap);

/*
 * Remove capability from sandbox
 */
int sandbox_remove_capability(uint32_t sandbox_id, uint64_t cap);

/*
 * Drop all capabilities
 */
int sandbox_drop_all_caps(uint32_t sandbox_id);

/*
 * Set capability set
 */
int sandbox_set_caps(uint32_t sandbox_id, const cap_set_t* caps);

/*
 * Get capability set
 */
int sandbox_get_caps(uint32_t sandbox_id, cap_set_t* caps_out);

/*
 * Add namespace to sandbox
 */
int sandbox_add_namespace(uint32_t sandbox_id, uint8_t ns_type);

/*
 * Remove namespace from sandbox
 */
int sandbox_remove_namespace(uint32_t sandbox_id, uint8_t ns_type);

/*
 * Set resource limit
 */
int sandbox_set_rlimit(uint32_t sandbox_id, uint32_t type, uint64_t soft, uint64_t hard);

/*
 * Get resource limit
 */
int sandbox_get_rlimit(uint32_t sandbox_id, uint32_t type, rlimit_t* rlimit_out);

/*
 * Set memory limit
 */
int sandbox_set_memory_limit(uint32_t sandbox_id, uint64_t limit);

/*
 * Set CPU limit
 */
int sandbox_set_cpu_limit(uint32_t sandbox_id, uint32_t shares);

/*
 * Set PID limit
 */
int sandbox_set_pids_limit(uint32_t sandbox_id, uint32_t limit);

/*
 * Enable/disable read-only rootfs
 */
void sandbox_set_readonly(uint32_t sandbox_id, int readonly);

/*
 * Enable/disable no_new_privs
 */
void sandbox_set_no_new_privs(uint32_t sandbox_id, int enable);

/*
 * Enable seccomp
 */
void sandbox_enable_seccomp(uint32_t sandbox_id);

/*
 * Set AppArmor profile
 */
int sandbox_set_apparmor(uint32_t sandbox_id, const char* profile);

/*
 * Set SELinux context
 */
int sandbox_set_selinux(uint32_t sandbox_id, const char* context);

/*
 * Set root filesystem
 */
int sandbox_set_rootfs(uint32_t sandbox_id, const char* rootfs);

/*
 * Get sandbox status
 */
int sandbox_get_status(uint32_t sandbox_id);

/*
 * Get sandbox info
 */
int sandbox_get_info(uint32_t sandbox_id, sandbox_t* info_out);

/*
 * List all sandboxes
 */
void sandbox_list(void);

/*
 * Show sandbox details
 */
void sandbox_show(uint32_t sandbox_id);

/*
 * Apply seccomp filter
 */
int sandbox_apply_seccomp(uint32_t sandbox_id);

/*
 * Apply AppArmor profile
 */
int sandbox_apply_apparmor(uint32_t sandbox_id);

/*
 * Apply SELinux context
 */
int sandbox_apply_selinux(uint32_t sandbox_id);

/*
 * Check if sandbox is running
 */
int sandbox_is_running(uint32_t sandbox_id);

/*
 * Get sandbox by name
 */
int sandbox_get_by_name(const char* name);

/*
 * Self-test
 */
int sandbox_selftest(void);

#endif /* SANDBOX_H */
