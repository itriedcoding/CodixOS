/*
 * CodixOS Authentication Module
 * User authentication with password policies and MFA/TOTP
 */

#ifndef AUTH_H
#define AUTH_H

/* Type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t size_t;

/* Authentication states */
#define AUTH_STATE_NONE           0
#define AUTH_STATE_LOCKED         1
#define AUTH_STATE_FAILED         2
#define AUTH_STATE_AUTHENTICATED  3

/* Password policy defaults */
#define MIN_PASSWORD_LENGTH       12
#define MAX_PASSWORD_LENGTH       128
#define MAX_LOGIN_ATTEMPTS        5
#define LOCKOUT_DURATION          300    /* 5 minutes in seconds */
#define PASSWORD_EXPIRY_DAYS      90
#define PASSWORD_HISTORY_SIZE     12

/* MFA types */
#define MFA_TYPE_NONE             0
#define MFA_TYPE_TOTP             1
#define MFA_TYPE_HOTP             2
#define MFA_TYPE_SMS              3
#define MFA_TYPE_HARDWARE_KEY     4

/* User flags */
#define USER_FLAG_ADMIN           0x01
#define USER_FLAG_PASSWORDLESS    0x02
#define USER_FLAG_MFA_REQUIRED    0x04
#define USER_FLAG_LOCKED          0x08
#define USER_FLAG_EXPIRED         0x10

/* Password complexity flags */
#define PASSComplexity_UPPERCASE  0x01
#define PASSComplexity_LOWERCASE  0x02
#define PASSComplexity_DIGITS     0x04
#define PASSComplexity_SYMBOLS    0x08
#define PASSComplexity_UNICODE    0x10

/* Max users and sessions */
#define MAX_USERS                 32
#define MAX_SESSIONS              16
#define MAX_PASSWORD_HISTORY      12
#define MAX_MFA_SEEDS             8

/* User structure */
typedef struct {
    uint32_t    uid;                    /* User ID */
    char        username[64];           /* Username */
    uint8_t     password_hash[64];      /* Bcrypt/Argon2 hash */
    uint8_t     salt[32];               /* Password salt */
    uint32_t    flags;                  /* User flags */
    uint32_t    password_changed;       /* Timestamp of last change */
    uint32_t    login_attempts;         /* Failed login attempts */
    uint32_t    lockout_until;          /* Lockout expiry timestamp */
    uint8_t     mfa_type;               /* MFA method */
    uint8_t     mfa_seed[32];           /* TOTP/HOTP seed */
    uint32_t    mfa_counter;            /* HOTP counter */
    uint8_t     password_history[MAX_PASSWORD_HISTORY][64]; /* Old password hashes */
    uint32_t    session_id;             /* Active session */
} user_t;

/* Session structure */
typedef struct {
    uint32_t    session_id;             /* Session ID */
    uint32_t    uid;                    /* User ID */
    uint32_t    created;                /* Creation timestamp */
    uint32_t    last_activity;          /* Last activity timestamp */
    uint32_t    expires;                /* Expiry timestamp */
    uint8_t     mfa_verified;           /* MFA completed */
    char        terminal[32];           /* Terminal device */
    char        source_ip[16];          /* Source IP address */
} session_t;

/* Password policy structure */
typedef struct {
    uint32_t    min_length;             /* Minimum password length */
    uint32_t    max_length;             /* Maximum password length */
    uint8_t     required_complexity;    /* Required complexity flags */
    uint32_t    expiry_days;            /* Password expiry in days */
    uint32_t    history_size;           /* Password history to check */
    uint8_t     require_mfa;            /* Require MFA for all users */
    uint32_t    max_login_attempts;     /* Max failed attempts before lockout */
    uint32_t    lockout_duration;       /* Lockout duration in seconds */
} password_policy_t;

/* Auth context */
typedef struct {
    user_t          users[MAX_USERS];
    uint32_t        user_count;
    session_t       sessions[MAX_SESSIONS];
    uint32_t        session_count;
    password_policy_t policy;
    uint32_t        next_uid;
    uint32_t        next_session_id;
} auth_ctx_t;

/* Audit event types */
#define AUDIT_EVENT_LOGIN          1
#define AUDIT_EVENT_LOGOUT         2
#define AUDIT_EVENT_LOGIN_FAILED   3
#define AUDIT_EVENT_PASSWORD_CHANGE 4
#define AUDIT_EVENT_MFA_SETUP      5
#define AUDIT_EVENT_USER_CREATE    6
#define AUDIT_EVENT_USER_DELETE    7
#define AUDIT_EVENT_LOCKOUT        8
#define AUDIT_EVENT_PRIVILEGE_ESC  9

/*
 * Initialize authentication system
 */
void auth_init(void);

/*
 * Create a new user
 */
int auth_create_user(const char* username, const char* password, uint32_t flags);

/*
 * Delete a user
 */
int auth_delete_user(const char* username);

/*
 * Authenticate user with password
 */
int auth_login(const char* username, const char* password);

/*
 * Authenticate with MFA token
 */
int auth_verify_mfa(uint32_t session_id, const char* token);

/*
 * Logout user
 */
int auth_logout(uint32_t session_id);

/*
 * Change password
 */
int auth_change_password(const char* username, const char* old_pass, const char* new_pass);

/*
 * Reset password (admin)
 */
int auth_reset_password(const char* username, const char* new_pass);

/*
 * Check if password meets policy
 */
int auth_check_password_policy(const char* password);

/*
 * Generate TOTP code
 */
int auth_generate_totp(const uint8_t* seed, uint32_t time_step);

/*
 * Verify TOTP code
 */
int auth_verify_totp(const uint8_t* seed, const char* token, uint32_t time_step);

/*
 * Setup MFA for user
 */
int auth_setup_mfa(const char* username, uint8_t mfa_type);

/*
 * Disable MFA for user
 */
int auth_disable_mfa(const char* username);

/*
 * Check if user is locked out
 */
int auth_is_locked_out(const char* username);

/*
 * Get user info
 */
int auth_get_user(const char* username, user_t* user_out);

/*
 * Get session info
 */
int auth_get_session(uint32_t session_id, session_t* session_out);

/*
 * Update password policy
 */
void auth_set_policy(const password_policy_t* policy);

/*
 * Get current policy
 */
void auth_get_policy(password_policy_t* policy_out);

/*
 * Show authentication status
 */
void auth_status(void);

/*
 * Show audit log
 */
void auth_show_audit_log(void);

/*
 * List all users
 */
void auth_list_users(void);

/*
 * List active sessions
 */
void auth_list_sessions(void);

/*
 * Lock user account
 */
int auth_lock_user(const char* username);

/*
 * Unlock user account
 */
int auth_unlock_user(const char* username);

/*
 * Enable/disable MFA requirement
 */
int auth_set_mfa_required(const char* username, int required);

/*
 * Cleanup expired sessions
 */
void auth_cleanup_sessions(void);

/*
 * Self-test authentication system
 */
int auth_selftest(void);

#endif /* AUTH_H */
