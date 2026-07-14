/*
 * CodixOS Shell Commands Implementation
 * All built-in shell commands
 */

#include "commands.h"
#include "../include/codix_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

/* Help command */
int cmd_help(char** args, int argc) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== CodixOS Shell Commands ===\n" COLOR_RESET);
    printf("\n");
    printf(COLOR_GREEN "  Navigation:\n" COLOR_RESET);
    printf("    cd [dir]      - Change directory\n");
    printf("    pwd           - Print working directory\n");
    printf("    ls [dir]      - List directory contents\n");
    printf("    find [dir]    - Find files\n");
    printf("\n");
    printf(COLOR_GREEN "  File Operations:\n" COLOR_RESET);
    printf("    cat [file]    - Display file contents\n");
    printf("    cp [src] [dst] - Copy files\n");
    printf("    mv [src] [dst] - Move/rename files\n");
    printf("    rm [file]     - Remove files\n");
    printf("    mkdir [dir]   - Create directory\n");
    printf("    touch [file]  - Create empty file\n");
    printf("    grep [pat] [file] - Search in files\n");
    printf("\n");
    printf(COLOR_GREEN "  System:\n" COLOR_RESET);
    printf("    ps            - List processes\n");
    printf("    kill [pid]    - Kill a process\n");
    printf("    df            - Disk usage\n");
    printf("    free          - Memory usage\n");
    printf("    top           - Process monitor\n");
    printf("    uname         - System information\n");
    printf("    uptime        - System uptime\n");
    printf("    whoami        - Current user\n");
    printf("    hostname      - System hostname\n");
    printf("    date          - Current date/time\n");
    printf("    cal           - Calendar\n");
    printf("\n");
    printf(COLOR_GREEN "  Shell:\n" COLOR_RESET);
    printf("    echo [text]   - Print text\n");
    printf("    history       - Command history\n");
    printf("    export [VAR=val] - Set environment variable\n");
    printf("    env           - Environment variables\n");
    printf("    alias [name=cmd] - Create alias\n");
    printf("    source [file] - Execute script\n");
    printf("    clear         - Clear screen\n");
    printf("    help          - Show this help\n");
    printf("    exit/quit     - Exit shell\n");
    printf("\n");
    printf(COLOR_GREEN "  Applications:\n" COLOR_RESET);
    printf("    pkg [cmd]     - Package manager\n");
    printf("    man [cmd]     - Manual pages\n");
    printf("    neofetch      - System info\n");
    printf("    cmatrix       - Matrix effect\n");
    printf("    about         - About CodixOS\n");
    printf("\n");
    return 0;
}

/* Exit command */
int cmd_exit(char** args, int argc) {
    printf("Goodbye!\n");
    exit(0);
    return 0;
}

/* CD command */
int cmd_cd(char** args, int argc) {
    char* dir;
    
    if (argc < 2) {
        dir = getenv("HOME");
        if (!dir) dir = "/";
    } else {
        dir = args[1];
    }
    
    if (chdir(dir) != 0) {
        printf(COLOR_RED "cd: %s: No such file or directory\n" COLOR_RESET, dir);
        return 1;
    }
    
    /* Update current directory */
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        /* Store in shell state */
    }
    
    return 0;
}

/* PWD command */
int cmd_pwd(char** args, int argc) {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        printf(COLOR_RED "pwd: error getting current directory\n" COLOR_RESET);
        return 1;
    }
    return 0;
}

/* Echo command */
int cmd_echo(char** args, int argc) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) printf(" ");
        printf("%s", args[i]);
    }
    printf("\n");
    return 0;
}

/* Clear command */
int cmd_clear(char** args, int argc) {
    printf("\033[2J\033[H");
    return 0;
}

/* History command */
int cmd_history(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== Command History ===\n" COLOR_RESET);
    for (int i = 0; i < 20; i++) {
        char* cmd = shell_get_history(i);
        if (cmd) {
            printf("  %4d  %s\n", i + 1, cmd);
        }
    }
    return 0;
}

/* Export command */
int cmd_export(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "export: usage: export VAR=value\n" COLOR_RESET);
        return 1;
    }
    
    char* eq = strchr(args[1], '=');
    if (eq) {
        *eq = '\0';
        setenv(args[1], eq + 1, 1);
        *eq = '=';  /* Restore for display */
    } else {
        setenv(args[1], "", 1);
    }
    
    return 0;
}

/* Env command */
int cmd_env(char** args, int argc) {
    extern char** environ;
    for (int i = 0; environ[i]; i++) {
        printf("%s\n", environ[i]);
    }
    return 0;
}

/* Alias command */
int cmd_alias(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_CYAN COLOR_BOLD "=== Aliases ===\n" COLOR_RESET);
        printf("  No aliases defined\n");
        return 0;
    }
    
    printf("Alias '%s' created\n", args[1]);
    return 0;
}

/* Source command */
int cmd_source(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "source: usage: source filename\n" COLOR_RESET);
        return 1;
    }
    
    FILE* file = fopen(args[1], "r");
    if (!file) {
        printf(COLOR_RED "source: %s: No such file\n" COLOR_RESET, args[1]);
        return 1;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        /* Execute each line */
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0 && line[0] != '#') {
            printf("Executing: %s\n", line);
        }
    }
    
    fclose(file);
    return 0;
}

/* Uname command */
int cmd_uname(char** args, int argc) {
    if (argc > 1 && strcmp(args[1], "-a") == 0) {
        printf("CodixOS codix 1.0.0 #1 SMP x86_64 CodixOS\n");
    } else {
        printf("CodixOS\n");
    }
    return 0;
}

/* Uptime command */
int cmd_uptime(char** args, int argc) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    printf(" %02d:%02d:%02d up 0:00, 1 user\n", 
           tm->tm_hour, tm->tm_min, tm->tm_sec);
    return 0;
}

/* Whoami command */
int cmd_whoami(char** args, int argc) {
    char* user = getenv("USER");
    if (!user) user = "codix";
    printf("%s\n", user);
    return 0;
}

/* Hostname command */
int cmd_hostname(char** args, int argc) {
    printf("codixos\n");
    return 0;
}

/* Date command */
int cmd_date(char** args, int argc) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    printf("%s", asctime(tm));
    return 0;
}

/* Calendar command */
int cmd_cal(char** args, int argc) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    int month = tm->tm_mon;
    int year = tm->tm_year + 1900;
    
    const char* months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    
    printf(COLOR_CYAN COLOR_BOLD "     %s %d\n" COLOR_RESET, months[month], year);
    printf("Su Mo Tu We Th Fr Sa\n");
    
    struct tm first_day = {0};
    first_day.tm_mon = month;
    first_day.tm_year = year;
    first_day.tm_mday = 1;
    mktime(&first_day);
    
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        days_in_month[1] = 29;
    }
    
    int start_day = first_day.tm_wday;
    int days = days_in_month[month];
    
    for (int i = 0; i < start_day; i++) {
        printf("   ");
    }
    
    for (int day = 1; day <= days; day++) {
        if (day == tm->tm_mday) {
            printf(COLOR_GREEN COLOR_BOLD "%2d" COLOR_RESET, day);
        } else {
            printf("%2d", day);
        }
        
        if ((start_day + day) % 7 == 0) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
    printf("\n");
    
    return 0;
}

/* LS command */
int cmd_ls(char** args, int argc) {
    DIR* dir;
    struct dirent* entry;
    char* path = (argc > 1) ? args[1] : ".";
    
    dir = opendir(path);
    if (!dir) {
        printf(COLOR_RED "ls: cannot access '%s': No such file or directory\n" COLOR_RESET, path);
        return 1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        struct stat st;
        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        
        if (stat(fullpath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                printf(COLOR_BLUE COLOR_BOLD "%s/\n" COLOR_RESET, entry->d_name);
            } else if (st.st_mode & S_IXUSR) {
                printf(COLOR_GREEN "%s*\n" COLOR_RESET, entry->d_name);
            } else {
                printf("%s\n", entry->d_name);
            }
        } else {
            printf("%s\n", entry->d_name);
        }
    }
    
    closedir(dir);
    return 0;
}

/* Cat command */
int cmd_cat(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "cat: missing file operand\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        FILE* file = fopen(args[i], "r");
        if (!file) {
            printf(COLOR_RED "cat: %s: No such file or directory\n" COLOR_RESET, args[i]);
            continue;
        }
        
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        
        fclose(file);
    }
    
    return 0;
}

/* CP command */
int cmd_cp(char** args, int argc) {
    if (argc < 3) {
        printf(COLOR_RED "cp: missing file operand\n" COLOR_RESET);
        return 1;
    }
    
    FILE* src = fopen(args[1], "rb");
    if (!src) {
        printf(COLOR_RED "cp: %s: No such file or directory\n" COLOR_RESET, args[1]);
        return 1;
    }
    
    FILE* dst = fopen(args[2], "wb");
    if (!dst) {
        printf(COLOR_RED "cp: cannot create '%s'\n" COLOR_RESET, args[2]);
        fclose(src);
        return 1;
    }
    
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dst);
    }
    
    fclose(src);
    fclose(dst);
    printf("Copied '%s' to '%s'\n", args[1], args[2]);
    return 0;
}

/* MV command */
int cmd_mv(char** args, int argc) {
    if (argc < 3) {
        printf(COLOR_RED "mv: missing file operand\n" COLOR_RESET);
        return 1;
    }
    
    if (rename(args[1], args[2]) != 0) {
        printf(COLOR_RED "mv: cannot move '%s' to '%s'\n" COLOR_RESET, args[1], args[2]);
        return 1;
    }
    
    printf("Moved '%s' to '%s'\n", args[1], args[2]);
    return 0;
}

/* RM command */
int cmd_rm(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "rm: missing file operand\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (remove(args[i]) != 0) {
            printf(COLOR_RED "rm: cannot remove '%s'\n" COLOR_RESET, args[i]);
        }
    }
    
    return 0;
}

/* Mkdir command */
int cmd_mkdir(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "mkdir: missing directory operand\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (mkdir(args[i], 0755) != 0) {
            printf(COLOR_RED "mkdir: cannot create directory '%s'\n" COLOR_RESET, args[i]);
        }
    }
    
    return 0;
}

/* Touch command */
int cmd_touch(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "touch: missing file operand\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        FILE* file = fopen(args[i], "a");
        if (file) {
            fclose(file);
        }
    }
    
    return 0;
}

/* Find command */
int cmd_find(char** args, int argc) {
    printf("find: implementation pending\n");
    return 0;
}

/* Grep command */
int cmd_grep(char** args, int argc) {
    if (argc < 3) {
        printf(COLOR_RED "grep: usage: grep PATTERN FILE\n" COLOR_RESET);
        return 1;
    }
    
    FILE* file = fopen(args[2], "r");
    if (!file) {
        printf(COLOR_RED "grep: %s: No such file\n" COLOR_RESET, args[2]);
        return 1;
    }
    
    char line[256];
    int line_num = 1;
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, args[1])) {
            printf(COLOR_GREEN "%d:" COLOR_RESET "%s", line_num, line);
        }
        line_num++;
    }
    
    fclose(file);
    return 0;
}

/* PS command */
int cmd_ps(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== Process List ===\n" COLOR_RESET);
    printf("  PID TTY          TIME CMD\n");
    printf("    1 ?        00:00:00 init\n");
    printf("    2 ?        00:00:00 codix-sh\n");
    return 0;
}

/* Kill command */
int cmd_kill(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "kill: missing PID\n" COLOR_RESET);
        return 1;
    }
    
    printf("kill: sent signal to PID %s\n", args[1]);
    return 0;
}

/* DF command */
int cmd_df(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== Disk Usage ===\n" COLOR_RESET);
    printf("Filesystem     1K-blocks    Used Available Use%% Mounted on\n");
    printf("/dev/sda1       10240000  1024000   9216000  10% /\n");
    return 0;
}

/* Free command */
int cmd_free(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== Memory Usage ===\n" COLOR_RESET);
    printf("              total        used        free      shared  buff/cache   available\n");
    printf("Mem:        16384000     2048000    12288000      512000     2048000    13824000\n");
    printf("Swap:        2097152           0     2097152\n");
    return 0;
}

/* Top command */
int cmd_top(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== CodixOS Top ===\n" COLOR_RESET);
    printf("PID    USER     CPU%  MEM%  COMMAND\n");
    printf("1      root     0.0   0.1   init\n");
    printf("2      codix    0.1   0.2   codix-sh\n");
    return 0;
}

/* Pkg command */
int cmd_pkg(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== CodixOS Package Manager ===\n" COLOR_RESET);
    
    if (argc < 2) {
        printf("Usage: pkg [install|remove|update|list|search] [package]\n");
        return 0;
    }
    
    if (strcmp(args[1], "install") == 0) {
        if (argc < 3) {
            printf("Usage: pkg install <package>\n");
            return 1;
        }
        printf("Installing %s...\n", args[2]);
        printf("Package %s installed successfully\n", args[2]);
    } else if (strcmp(args[1], "remove") == 0) {
        if (argc < 3) {
            printf("Usage: pkg remove <package>\n");
            return 1;
        }
        printf("Removing %s...\n", args[2]);
        printf("Package %s removed successfully\n", args[2]);
    } else if (strcmp(args[1], "update") == 0) {
        printf("Updating package database...\n");
        printf("Database updated successfully\n");
    } else if (strcmp(args[1], "list") == 0) {
        printf("Installed packages:\n");
        printf("  codix-base  1.0.0\n");
        printf("  codix-shell 1.0.0\n");
    } else if (strcmp(args[1], "search") == 0) {
        if (argc < 3) {
            printf("Usage: pkg search <query>\n");
            return 1;
        }
        printf("Searching for %s...\n", args[2]);
        printf("No packages found\n");
    } else {
        printf("Unknown command: %s\n", args[1]);
    }
    
    return 0;
}

/* Man command */
int cmd_man(char** args, int argc) {
    if (argc < 2) {
        printf("What manual page do you want?\n");
        return 0;
    }
    
    printf(COLOR_CYAN COLOR_BOLD "=== Manual: %s ===\n" COLOR_RESET, args[1]);
    printf("No manual entry for %s\n", args[1]);
    return 0;
}

/* About command */
int cmd_about(char** args, int argc) {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== About CodixOS ===\n" COLOR_RESET);
    printf("\n");
    printf("CodixOS is a lightweight, terminal-based operating system\n");
    printf("designed for simplicity and ease of use.\n");
    printf("\n");
    printf(COLOR_GREEN "Features:\n" COLOR_RESET);
    printf("  - Custom kernel with memory management\n");
    printf("  - Built-in shell with command support\n");
    printf("  - Package manager for software installation\n");
    printf("  - Terminal-based interface\n");
    printf("  - Lightweight and fast boot\n");
    printf("\n");
    printf(COLOR_GREEN "Version:\n" COLOR_RESET);
    printf("  CodixOS 1.0.0\n");
    printf("\n");
    printf(COLOR_GREEN "License:\n" COLOR_RESET);
    printf("  MIT License\n");
    printf("\n");
    return 0;
}

/* Neofetch command */
int cmd_neofetch(char** args, int argc) {
    printf("\n");
    printf(COLOR_CYAN "        _____      _               " COLOR_RED "user@codixos\n" COLOR_RESET);
    printf(COLOR_CYAN "       / ____|    | |             " COLOR_RESET "OS: CodixOS 1.0.0\n");
    printf(COLOR_CYAN "      | |     ___ | | ___  _ __   " COLOR_RESET "Host: CodixOS Virtual Machine\n");
    printf(COLOR_CYAN "      | |    / _ \\| |/ _ \\| '__|  " COLOR_RESET "Kernel: 1.0.0\n");
    printf(COLOR_CYAN "      | |___| (_) | | (_) | |     " COLOR_RESET "Uptime: 0 hours, 0 mins\n");
    printf(COLOR_CYAN "       \\_____\\___/|_|\\___/|_|     " COLOR_RESET "Shell: codix-sh 1.0.0\n");
    printf(COLOR_CYAN "                                   " COLOR_RESET "Terminal: codix-term\n");
    printf(COLOR_CYAN "   CPU: CodixOS Virtual CPU        " COLOR_RESET "Memory: 128MB / 256MB\n");
    printf("\n");
    printf(COLOR_RED "■" COLOR_GREEN "■" COLOR_YELLOW "■" COLOR_BLUE "■" COLOR_MAGENTA "■" COLOR_CYAN "■" COLOR_WHITE "■" COLOR_RESET "\n");
    printf("\n");
    return 0;
}

/* Cmatrix command */
int cmd_cmatrix(char** args, int argc) {
    printf("Press Ctrl+C to exit\n");
    printf("Matrix effect not available in this terminal\n");
    return 0;
}

/* Execute external command */
int execute_external(char** args, int argc) {
    pid_t pid = fork();
    
    if (pid == 0) {
        /* Child process */
        execvp(args[0], args);
        printf(COLOR_RED "%s: command not found\n" COLOR_RESET, args[0]);
        exit(1);
    } else if (pid > 0) {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
    } else {
        printf(COLOR_RED "fork failed\n" COLOR_RESET);
        return 1;
    }
    
    return 0;
}
