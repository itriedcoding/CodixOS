/*
 * CodixOS Authentication Implementation
 * User authentication with password policies and MFA/TOTP
 */

#include "auth.h"

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Global context */
static auth_ctx_t auth_ctx;

/* Audit log */
#define AUDIT_LOG_SIZE 8192
static char audit_log[AUDIT_LOG_SIZE];
static uint32_t audit_offset = 0;

/* External kernel functions */
extern void print(const char* str);
extern void println(const char* str);
extern void print_int(int num);
extern void putchar(char c);
extern void memset(void* dest, int c, size_t n);
extern void memcpy(void* dest, const void* src, size_t n);
extern int memcmp(const void* s1, const void* s2, size_t n);
extern int strcmp(const char* s1, const char* s2);
extern void strcpy(char* dest, const char* src);
extern size_t strlen(const char* str);

/* Pseudo-random number generator */
static uint32_t auth_rng_state = 0xABCD1234;
static uint8_t auth_random(void) {
    auth_rng_state = auth_rng_state * 1103515245 + 12345;
    return (uint8_t)((auth_rng_state >> 16) & 0xFF);
}

static void auth_random_bytes(uint8_t* buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = auth_random();
    }
}

/* Simple hash function */
static void simple_hash(const uint8_t* data, uint32_t len, uint8_t* hash) {
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

/* Hash password with salt */
static void hash_password(const char* password, const uint8_t* salt, uint8_t* hash) {
    uint8_t data[128];
    size_t pass_len = strlen(password);
    
    /* Concatenate password and salt */
    memcpy(data, password, pass_len);
    memcpy(data + pass_len, salt, 32);
    
    /* Multiple iterations for key stretching */
    uint8_t tmp[32];
    memcpy(tmp, data, pass_len + 32);
    for (int i = 0; i < 10000; i++) {
        simple_hash(tmp, pass_len + 32, tmp);
    }
    
    memcpy(hash, tmp, 64);
}

/* TOTP implementation */
static uint32_t get_time_step(void) {
    /* Simplified - in production use RTC or NTP */
    static uint32_t fake_time = 1000000;
    fake_time += 30;
    return fake_time / 30;
}

static uint32_t hotp(const uint8_t* secret, uint32_t secret_len, uint32_t counter) {
    /* Simplified HOTP - in production use HMAC-SHA1 */
    uint8_t data[8];
    data[0] = (counter >> 56) & 0xFF;
    data[1] = (counter >> 48) & 0xFF;
    data[2] = (counter >> 40) & 0xFF;
    data[3] = (counter >> 32) & 0xFF;
    data[4] = (counter >> 24) & 0xFF;
    data[5] = (counter >> 16) & 0xFF;
    data[6] = (counter >> 8) & 0xFF;
    data[7] = counter & 0xFF;
    
    uint8_t hash[32];
    simple_hash(data, 8, hash);
    
    /* Truncate to 6 digits */
    uint32_t offset = hash[19] & 0x0F;
    uint32_t code = ((hash[offset] & 0x7F) << 24) |
                    ((hash[offset + 1] & 0xFF) << 16) |
                    ((hash[offset + 2] & 0xFF) << 8) |
                    (hash[offset + 3] & 0xFF);
    
    return code % 1000000;
}

/* Audit logging */
static void audit_log_event(uint32_t uid, uint8_t event_type, const char* details) {
    if (audit_offset >= AUDIT_LOG_SIZE - 128) {
        audit_offset = 0; /* Rotate */
    }
    
    /* Simple timestamp */
    static uint32_t log_time = 0;
    log_time++;
    
    uint32_t hours = (log_time / 3600) % 24;
    uint32_t minutes = (log_time / 60) % 60;
    uint32_t seconds = log_time % 60;
    
    char time_str[32];
    time_str[0] = '0' + (hours / 10);
    time_str[1] = '0' + (hours % 10);
    time_str[2] = ':';
    time_str[3] = '0' + (minutes / 10);
    time_str[4] = '0' + (minutes % 10);
    time_str[5] = ':';
    time_str[6] = '0' + (seconds / 10);
    time_str[7] = '0' + (seconds % 10);
    time_str[8] = '\0';
    
    /* Event type string */
    const char* event_str;
    switch (event_type) {
        case AUDIT_EVENT_LOGIN:           event_str = "LOGIN"; break;
        case AUDIT_EVENT_LOGOUT:          event_str = "LOGOUT"; break;
        case AUDIT_EVENT_LOGIN_FAILED:    event_str = "LOGIN_FAILED"; break;
        case AUDIT_EVENT_PASSWORD_CHANGE: event_str = "PASS_CHANGE"; break;
        case AUDIT_EVENT_MFA_SETUP:       event_str = "MFA_SETUP"; break;
        case AUDIT_EVENT_USER_CREATE:     event_str = "USER_CREATE"; break;
        case AUDIT_EVENT_USER_DELETE:     event_str = "USER_DELETE"; break;
        case AUDIT_EVENT_LOCKOUT:         event_str = "LOCKOUT"; break;
        case AUDIT_EVENT_PRIVILEGE_ESC:   event_str = "PRIV_ESC"; break;
        default:                          event_str = "UNKNOWN"; break;
    }
    
    /* Write to log */
    const char* p = time_str;
    while (*p) audit_log[audit_offset++] = *p++;
    audit_log[audit_offset++] = ' ';
    
    p = event_str;
    while (*p) audit_log[audit_offset++] = *p++;
    audit_log[audit_offset++] = ' ';
    
    /* UID */
    char uid_str[16];
    int pos = 0;
    uint32_t u = uid;
    if (u == 0) { uid_str[pos++] = '0'; }
    else {
        char tmp[16];
        int t = 0;
        while (u > 0) { tmp[t++] = '0' + (u % 10); u /= 10; }
        while (t > 0) uid_str[pos++] = tmp[--t];
    }
    uid_str[pos] = '\0';
    
    p = uid_str;
    while (*p) audit_log[audit_offset++] = *p++;
    audit_log[audit_offset++] = ' ';
    
    p = details;
    while (*p && audit_offset < AUDIT_LOG_SIZE - 2) audit_log[audit_offset++] = *p++;
    audit_log[audit_offset++] = '\n';
}

/* Find user by username */
static user_t* find_user(const char* username) {
    for (uint32_t i = 0; i < auth_ctx.user_count; i++) {
        if (strcmp(auth_ctx.users[i].username, username) == 0) {
            return &auth_ctx.users[i];
        }
    }
    return (user_t*)0;
}

/* Initialize authentication system */
void auth_init(void) {
    memset(&auth_ctx, 0, sizeof(auth_ctx_t));
    memset(audit_log, 0, AUDIT_LOG_SIZE);
    audit_offset = 0;
    
    /* Default password policy */
    auth_ctx.policy.min_length = MIN_PASSWORD_LENGTH;
    auth_ctx.policy.max_length = MAX_PASSWORD_LENGTH;
    auth_ctx.policy.required_complexity = PASSComplexity_UPPERCASE | PASSComplexity_LOWERCASE |
                                          PASSComplexity_DIGITS | PASSComplexity_SYMBOLS;
    auth_ctx.policy.expiry_days = PASSWORD_EXPIRY_DAYS;
    auth_ctx.policy.history_size = PASSWORD_HISTORY_SIZE;
    auth_ctx.policy.require_mfa = 0;
    auth_ctx.policy.max_login_attempts = MAX_LOGIN_ATTEMPTS;
    auth_ctx.policy.lockout_duration = LOCKOUT_DURATION;
    
    auth_ctx.next_uid = 1000;
    auth_ctx.next_session_id = 1;
    
    println("Authentication system initialized");
}

/* Create user */
int auth_create_user(const char* username, const char* password, uint32_t flags) {
    if (!username || !password) {
        println("Error: username and password required");
        return -1;
    }
    
    if (auth_ctx.user_count >= MAX_USERS) {
        println("Error: maximum users reached");
        return -1;
    }
    
    if (find_user(username)) {
        println("Error: user already exists");
        return -1;
    }
    
    if (auth_check_password_policy(password) != 0) {
        println("Error: password does not meet policy");
        return -1;
    }
    
    user_t* user = &auth_ctx.users[auth_ctx.user_count++];
    memset(user, 0, sizeof(user_t));
    
    user->uid = auth_ctx.next_uid++;
    strcpy(user->username, username);
    
    /* Generate salt and hash password */
    auth_random_bytes(user->salt, 32);
    hash_password(password, user->salt, user->password_hash);
    
    user->flags = flags;
    user->password_changed = 1000000; /* Fake current time */
    
    audit_log_event(user->uid, AUDIT_EVENT_USER_CREATE, username);
    
    print("User created: ");
    println(username);
    return 0;
}

/* Delete user */
int auth_delete_user(const char* username) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    audit_log_event(user->uid, AUDIT_EVENT_USER_DELETE, username);
    
    /* Remove user by shifting array */
    uint32_t idx = user - auth_ctx.users;
    for (uint32_t i = idx; i < auth_ctx.user_count - 1; i++) {
        auth_ctx.users[i] = auth_ctx.users[i + 1];
    }
    auth_ctx.user_count--;
    
    print("User deleted: ");
    println(username);
    return 0;
}

/* Check password policy */
int auth_check_password_policy(const char* password) {
    if (!password) return -1;
    
    size_t len = strlen(password);
    
    if (len < auth_ctx.policy.min_length) {
        println("Password too short");
        return -1;
    }
    
    if (len > auth_ctx.policy.max_length) {
        println("Password too long");
        return -1;
    }
    
    uint8_t found = 0;
    const char* p = password;
    while (*p) {
        if (*p >= 'A' && *p <= 'Z') found |= PASSComplexity_UPPERCASE;
        else if (*p >= 'a' && *p <= 'z') found |= PASSComplexity_LOWERCASE;
        else if (*p >= '0' && *p <= '9') found |= PASSComplexity_DIGITS;
        else found |= PASSComplexity_SYMBOLS;
        p++;
    }
    
    if ((found & auth_ctx.policy.required_complexity) != auth_ctx.policy.required_complexity) {
        println("Password lacks required complexity");
        return -1;
    }
    
    return 0;
}

/* Login */
int auth_login(const char* username, const char* password) {
    if (!username || !password) {
        println("Error: username and password required");
        return -1;
    }
    
    user_t* user = find_user(username);
    if (!user) {
        audit_log_event(0, AUDIT_EVENT_LOGIN_FAILED, username);
        println("Error: invalid credentials");
        return -1;
    }
    
    /* Check lockout */
    if (auth_is_locked_out(username)) {
        audit_log_event(user->uid, AUDIT_EVENT_LOCKOUT, username);
        println("Error: account locked out");
        return -1;
    }
    
    /* Verify password */
    uint8_t hash[64];
    hash_password(password, user->salt, hash);
    
    if (memcmp(hash, user->password_hash, 64) != 0) {
        user->login_attempts++;
        if (user->login_attempts >= auth_ctx.policy.max_login_attempts) {
            user->lockout_until = 1000000 + auth_ctx.policy.lockout_duration;
            user->login_attempts = 0;
            audit_log_event(user->uid, AUDIT_EVENT_LOCKOUT, username);
            println("Error: account locked due to too many failed attempts");
        } else {
            audit_log_event(user->uid, AUDIT_EVENT_LOGIN_FAILED, username);
            println("Error: invalid credentials");
        }
        return -1;
    }
    
    /* Reset login attempts */
    user->login_attempts = 0;
    user->lockout_until = 0;
    
    /* Create session */
    if (auth_ctx.session_count >= MAX_SESSIONS) {
        println("Error: maximum sessions reached");
        return -1;
    }
    
    session_t* session = &auth_ctx.sessions[auth_ctx.session_count++];
    memset(session, 0, sizeof(session_t));
    
    session->session_id = auth_ctx.next_session_id++;
    session->uid = user->uid;
    session->created = 1000000;
    session->last_activity = session->created;
    session->expires = session->created + 3600; /* 1 hour */
    session->mfa_verified = (user->mfa_type == MFA_TYPE_NONE) ? 1 : 0;
    
    user->session_id = session->session_id;
    
    audit_log_event(user->uid, AUDIT_EVENT_LOGIN, username);
    
    if (user->mfa_type != MFA_TYPE_NONE) {
        println("Login successful. MFA verification required.");
        print("Session ID: ");
        print_int(session->session_id);
        println("");
        return 1; /* Requires MFA */
    }
    
    println("Login successful.");
    print("Session ID: ");
    print_int(session->session_id);
    println("");
    return 0;
}

/* Verify MFA token */
int auth_verify_mfa(uint32_t session_id, const char* token) {
    session_t* session = (session_t*)0;
    user_t* user = (user_t*)0;
    
    for (uint32_t i = 0; i < auth_ctx.session_count; i++) {
        if (auth_ctx.sessions[i].session_id == session_id) {
            session = &auth_ctx.sessions[i];
            break;
        }
    }
    
    if (!session) {
        println("Error: invalid session");
        return -1;
    }
    
    for (uint32_t i = 0; i < auth_ctx.user_count; i++) {
        if (auth_ctx.users[i].uid == session->uid) {
            user = &auth_ctx.users[i];
            break;
        }
    }
    
    if (!user || user->mfa_type == MFA_TYPE_NONE) {
        println("Error: MFA not configured");
        return -1;
    }
    
    /* Verify TOTP */
    uint32_t time_step = get_time_step();
    uint32_t expected = hotp(user->mfa_seed, 32, time_step);
    
    /* Convert token string to number */
    uint32_t code = 0;
    const char* p = token;
    while (*p) {
        if (*p >= '0' && *p <= '9') {
            code = code * 10 + (*p - '0');
        }
        p++;
    }
    
    /* Check current and adjacent time steps for clock skew */
    if (code == expected || code == hotp(user->mfa_seed, 32, time_step - 1) ||
        code == hotp(user->mfa_seed, 32, time_step + 1)) {
        session->mfa_verified = 1;
        audit_log_event(user->uid, AUDIT_EVENT_LOGIN, "MFA verified");
        println("MFA verification successful.");
        return 0;
    }
    
    audit_log_event(user->uid, AUDIT_EVENT_LOGIN_FAILED, "MFA failed");
    println("Error: invalid MFA code");
    return -1;
}

/* Logout */
int auth_logout(uint32_t session_id) {
    for (uint32_t i = 0; i < auth_ctx.session_count; i++) {
        if (auth_ctx.sessions[i].session_id == session_id) {
            uint32_t uid = auth_ctx.sessions[i].uid;
            
            /* Remove session */
            for (uint32_t j = i; j < auth_ctx.session_count - 1; j++) {
                auth_ctx.sessions[j] = auth_ctx.sessions[j + 1];
            }
            auth_ctx.session_count--;
            
            audit_log_event(uid, AUDIT_EVENT_LOGOUT, "session ended");
            println("Logged out successfully.");
            return 0;
        }
    }
    
    println("Error: invalid session");
    return -1;
}

/* Change password */
int auth_change_password(const char* username, const char* old_pass, const char* new_pass) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    /* Verify old password */
    uint8_t hash[64];
    hash_password(old_pass, user->salt, hash);
    if (memcmp(hash, user->password_hash, 64) != 0) {
        println("Error: invalid current password");
        return -1;
    }
    
    /* Check new password policy */
    if (auth_check_password_policy(new_pass) != 0) {
        return -1;
    }
    
    /* Check password history */
    for (uint32_t i = 0; i < auth_ctx.policy.history_size; i++) {
        uint8_t old_hash[64];
        hash_password(new_pass, user->salt, old_hash);
        if (memcmp(old_hash, user->password_history[i], 64) == 0) {
            println("Error: password was recently used");
            return -1;
        }
    }
    
    /* Store old password in history */
    uint32_t hist_idx = user->password_changed % auth_ctx.policy.history_size;
    memcpy(user->password_history[hist_idx], user->password_hash, 64);
    
    /* Set new password */
    auth_random_bytes(user->salt, 32);
    hash_password(new_pass, user->salt, user->password_hash);
    user->password_changed = 1000000;
    
    audit_log_event(user->uid, AUDIT_EVENT_PASSWORD_CHANGE, username);
    println("Password changed successfully.");
    return 0;
}

/* Reset password (admin) */
int auth_reset_password(const char* username, const char* new_pass) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    if (auth_check_password_policy(new_pass) != 0) {
        return -1;
    }
    
    auth_random_bytes(user->salt, 32);
    hash_password(new_pass, user->salt, user->password_hash);
    user->password_changed = 1000000;
    user->lockout_until = 0;
    user->login_attempts = 0;
    
    audit_log_event(user->uid, AUDIT_EVENT_PASSWORD_CHANGE, "admin reset");
    println("Password reset successfully.");
    return 0;
}

/* Check if user is locked out */
int auth_is_locked_out(const char* username) {
    user_t* user = find_user(username);
    if (!user) return 0;
    
    if (user->lockout_until > 1000000) {
        return 1;
    }
    
    return 0;
}

/* Get user info */
int auth_get_user(const char* username, user_t* user_out) {
    user_t* user = find_user(username);
    if (!user) return -1;
    memcpy(user_out, user, sizeof(user_t));
    return 0;
}

/* Get session info */
int auth_get_session(uint32_t session_id, session_t* session_out) {
    for (uint32_t i = 0; i < auth_ctx.session_count; i++) {
        if (auth_ctx.sessions[i].session_id == session_id) {
            memcpy(session_out, &auth_ctx.sessions[i], sizeof(session_t));
            return 0;
        }
    }
    return -1;
}

/* Setup MFA */
int auth_setup_mfa(const char* username, uint8_t mfa_type) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    user->mfa_type = mfa_type;
    auth_random_bytes(user->mfa_seed, 32);
    user->mfa_counter = 0;
    
    audit_log_event(user->uid, AUDIT_EVENT_MFA_SETUP, username);
    println("MFA configured successfully.");
    return 0;
}

/* Disable MFA */
int auth_disable_mfa(const char* username) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    user->mfa_type = MFA_TYPE_NONE;
    memset(user->mfa_seed, 0, 32);
    
    println("MFA disabled.");
    return 0;
}

/* Lock user */
int auth_lock_user(const char* username) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    user->flags |= USER_FLAG_LOCKED;
    user->lockout_until = 0xFFFFFFFF; /* Permanent */
    
    audit_log_event(user->uid, AUDIT_EVENT_LOCKOUT, "admin lock");
    println("User locked.");
    return 0;
}

/* Unlock user */
int auth_unlock_user(const char* username) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    user->flags &= ~USER_FLAG_LOCKED;
    user->lockout_until = 0;
    user->login_attempts = 0;
    
    println("User unlocked.");
    return 0;
}

/* Set MFA required */
int auth_set_mfa_required(const char* username, int required) {
    user_t* user = find_user(username);
    if (!user) {
        println("Error: user not found");
        return -1;
    }
    
    if (required) {
        user->flags |= USER_FLAG_MFA_REQUIRED;
    } else {
        user->flags &= ~USER_FLAG_MFA_REQUIRED;
    }
    
    return 0;
}

/* Set policy */
void auth_set_policy(const password_policy_t* policy) {
    memcpy(&auth_ctx.policy, policy, sizeof(password_policy_t));
    println("Password policy updated.");
}

/* Get policy */
void auth_get_policy(password_policy_t* policy_out) {
    memcpy(policy_out, &auth_ctx.policy, sizeof(password_policy_t));
}

/* Show status */
void auth_status(void) {
    println("");
    println("=== Authentication System Status ===");
    println("");
    print("  Users:       ");
    print_int(auth_ctx.user_count);
    println("");
    print("  Sessions:    ");
    print_int(auth_ctx.session_count);
    println("");
    print("  MFA Required: ");
    println(auth_ctx.policy.require_mfa ? "Yes" : "No");
    println("");
    
    println("  Password Policy:");
    print("    Min Length:      ");
    print_int(auth_ctx.policy.min_length);
    println("");
    print("    Max Length:      ");
    print_int(auth_ctx.policy.max_length);
    println("");
    print("    Expiry Days:     ");
    print_int(auth_ctx.policy.expiry_days);
    println("");
    print("    Max Attempts:    ");
    print_int(auth_ctx.policy.max_login_attempts);
    println("");
    print("    Lockout (sec):   ");
    print_int(auth_ctx.policy.lockout_duration);
    println("");
}

/* List users */
void auth_list_users(void) {
    println("");
    println("=== Users ===");
    println("");
    
    if (auth_ctx.user_count == 0) {
        println("  No users");
    } else {
        println("  UID   USERNAME         FLAGS        MFA");
        println("  ---   --------         -----        ---");
        for (uint32_t i = 0; i < auth_ctx.user_count; i++) {
            user_t* u = &auth_ctx.users[i];
            print("  ");
            print_int(u->uid);
            print("     ");
            print(u->username);
            /* Padding */
            for (int p = strlen(u->username); p < 17; p++) print(" ");
            println(u->flags & USER_FLAG_LOCKED ? "LOCKED" : "ACTIVE");
        }
    }
    println("");
}

/* List sessions */
void auth_list_sessions(void) {
    println("");
    println("=== Active Sessions ===");
    println("");
    
    if (auth_ctx.session_count == 0) {
        println("  No active sessions");
    } else {
        for (uint32_t i = 0; i < auth_ctx.session_count; i++) {
            session_t* s = &auth_ctx.sessions[i];
            print("  Session ");
            print_int(s->session_id);
            print(" - UID: ");
            print_int(s->uid);
            print(" MFA: ");
            println(s->mfa_verified ? "Yes" : "No");
        }
    }
    println("");
}

/* Cleanup expired sessions */
void auth_cleanup_sessions(void) {
    uint32_t current_time = 1000000; /* Fake current time */
    uint32_t removed = 0;
    
    for (int i = auth_ctx.session_count - 1; i >= 0; i--) {
        if (auth_ctx.sessions[i].expires < current_time) {
            for (uint32_t j = i; j < auth_ctx.session_count - 1; j++) {
                auth_ctx.sessions[j] = auth_ctx.sessions[j + 1];
            }
            auth_ctx.session_count--;
            removed++;
        }
    }
    
    if (removed > 0) {
        print("Cleaned up ");
        print_int(removed);
        println(" expired sessions");
    }
}

/* Show audit log */
void auth_show_audit_log(void) {
    println("");
    println("=== Authentication Audit Log ===");
    println("");
    
    if (audit_offset == 0) {
        println("  No events logged");
    } else {
        for (uint32_t i = 0; i < audit_offset; i++) {
            putchar(audit_log[i]);
        }
    }
    println("");
}

/* Self-test */
int auth_selftest(void) {
    println("Running authentication self-test...");
    
    int errors = 0;
    
    /* Test password policy */
    if (auth_check_password_policy("short") == 0) {
        println("  Password policy:  FAILED (should reject short)");
        errors++;
    } else {
        println("  Password policy:  OK");
    }
    
    /* Test user creation */
    if (auth_create_user("testuser", "Test123!@#abc", 0) == 0) {
        println("  User creation:    OK");
        
        /* Test login */
        if (auth_login("testuser", "Test123!@#abc") >= 0) {
            println("  Login:            OK");
        } else {
            println("  Login:            FAILED");
            errors++;
        }
        
        /* Test failed login */
        if (auth_login("testuser", "wrongpassword") == -1) {
            println("  Failed login:     OK");
        } else {
            println("  Failed login:     FAILED");
            errors++;
        }
        
        /* Cleanup */
        auth_delete_user("testuser");
    } else {
        println("  User creation:    FAILED");
        errors++;
    }
    
    println("");
    if (errors == 0) {
        println("All authentication self-tests passed!");
    } else {
        print_int(errors);
        println(" self-tests failed");
    }
    
    return errors;
}
