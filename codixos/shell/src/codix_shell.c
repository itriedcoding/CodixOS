/*
 * CodixOS Shell - Main Implementation
 * A powerful, lightweight shell for CodixOS
 */

#include "../include/codix_shell.h"
#include "commands.h"
#include "parser.h"
#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Shell state */
static shell_state_t shell_state;

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

/* Initialize shell */
void shell_init() {
    /* Initialize state */
    shell_state.history_count = 0;
    shell_state.history_index = -1;
    shell_state.running = 1;
    
    /* Get username */
    char* user = getenv("USER");
    if (user) {
        strcpy(shell_state.username, user);
    } else {
        strcpy(shell_state.username, "codix");
    }
    
    /* Get current directory */
    if (getcwd(shell_state.current_dir, sizeof(shell_state.current_dir)) == NULL) {
        strcpy(shell_state.current_dir, "/");
    }
    
    /* Print welcome banner */
    shell_print_banner();
}

/* Print welcome banner */
void shell_print_banner() {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD);
    printf("        _____      _               ____   _____ \n");
    printf("       / ____|    | |             |  _ \\ / ____|\n");
    printf("      | |     ___ | | ___  _ __   | |_) | (___  \n");
    printf("      | |    / _ \\| |/ _ \\| '__|  |  _ < \\___ \\ \n");
    printf("      | |___| (_) | | (_) | |     | |_) |____) |\n");
    printf("       \\_____\\___/|_|\\___/|_|     |____/|_____/ \n");
    printf(COLOR_RESET);
    printf("\n");
    printf(COLOR_GREEN "  Welcome to CodixOS Shell v%s\n", SHELL_VERSION);
    printf(COLOR_RESET);
    printf("  Type 'help' for available commands\n\n");
}

/* Print shell prompt */
void shell_print_prompt() {
    printf(COLOR_GREEN COLOR_BOLD "%s", shell_state.username);
    printf(COLOR_RESET);
    printf("@");
    printf(COLOR_CYAN COLOR_BOLD "codix");
    printf(COLOR_RESET);
    printf(":");
    printf(COLOR_YELLOW "%s", shell_state.current_dir);
    printf(COLOR_RESET);
    printf("$ ");
    fflush(stdout);
}

/* Read line from user */
int shell_read_line(char* buffer, int max_len) {
    if (fgets(buffer, max_len, stdin) == NULL) {
        return -1;
    }
    
    /* Remove newline */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }
    
    return len;
}

/* Parse command line */
void shell_parse_line(char* line, char** args, int* argc) {
    char* token;
    int count = 0;
    
    token = strtok(line, " \t");
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t");
    }
    args[count] = NULL;
    *argc = count;
}

/* Execute command */
int shell_execute(char** args, int argc) {
    if (argc == 0) return 0;
    
    /* Check for built-in commands */
    if (strcmp(args[0], "help") == 0) {
        return cmd_help(args, argc);
    } else if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
        return cmd_exit(args, argc);
    } else if (strcmp(args[0], "cd") == 0) {
        return cmd_cd(args, argc);
    } else if (strcmp(args[0], "pwd") == 0) {
        return cmd_pwd(args, argc);
    } else if (strcmp(args[0], "echo") == 0) {
        return cmd_echo(args, argc);
    } else if (strcmp(args[0], "clear") == 0) {
        return cmd_clear(args, argc);
    } else if (strcmp(args[0], "history") == 0) {
        return cmd_history(args, argc);
    } else if (strcmp(args[0], "export") == 0) {
        return cmd_export(args, argc);
    } else if (strcmp(args[0], "env") == 0) {
        return cmd_env(args, argc);
    } else if (strcmp(args[0], "alias") == 0) {
        return cmd_alias(args, argc);
    } else if (strcmp(args[0], "source") == 0) {
        return cmd_source(args, argc);
    } else if (strcmp(args[0], "uname") == 0) {
        return cmd_uname(args, argc);
    } else if (strcmp(args[0], "uptime") == 0) {
        return cmd_uptime(args, argc);
    } else if (strcmp(args[0], "whoami") == 0) {
        return cmd_whoami(args, argc);
    } else if (strcmp(args[0], "hostname") == 0) {
        return cmd_hostname(args, argc);
    } else if (strcmp(args[0], "date") == 0) {
        return cmd_date(args, argc);
    } else if (strcmp(args[0], "cal") == 0) {
        return cmd_cal(args, argc);
    } else if (strcmp(args[0], "ls") == 0) {
        return cmd_ls(args, argc);
    } else if (strcmp(args[0], "cat") == 0) {
        return cmd_cat(args, argc);
    } else if (strcmp(args[0], "cp") == 0) {
        return cmd_cp(args, argc);
    } else if (strcmp(args[0], "mv") == 0) {
        return cmd_mv(args, argc);
    } else if (strcmp(args[0], "rm") == 0) {
        return cmd_rm(args, argc);
    } else if (strcmp(args[0], "mkdir") == 0) {
        return cmd_mkdir(args, argc);
    } else if (strcmp(args[0], "touch") == 0) {
        return cmd_touch(args, argc);
    } else if (strcmp(args[0], "find") == 0) {
        return cmd_find(args, argc);
    } else if (strcmp(args[0], "grep") == 0) {
        return cmd_grep(args, argc);
    } else if (strcmp(args[0], "ps") == 0) {
        return cmd_ps(args, argc);
    } else if (strcmp(args[0], "kill") == 0) {
        return cmd_kill(args, argc);
    } else if (strcmp(args[0], "df") == 0) {
        return cmd_df(args, argc);
    } else if (strcmp(args[0], "free") == 0) {
        return cmd_free(args, argc);
    } else if (strcmp(args[0], "top") == 0) {
        return cmd_top(args, argc);
    } else if (strcmp(args[0], "pkg") == 0) {
        return cmd_pkg(args, argc);
    } else if (strcmp(args[0], "man") == 0) {
        return cmd_man(args, argc);
    } else if (strcmp(args[0], "about") == 0) {
        return cmd_about(args, argc);
    } else if (strcmp(args[0], "neofetch") == 0) {
        return cmd_neofetch(args, argc);
    } else if (strcmp(args[0], "cmatrix") == 0) {
        return cmd_cmatrix(args, argc);
    } else {
        /* Try to execute as external command */
        return execute_external(args, argc);
    }
}

/* Add command to history */
void shell_add_history(const char* line) {
    if (shell_state.history_count >= MAX_HISTORY_SIZE) {
        /* Remove oldest entry */
        free(shell_state.history[0]);
        for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
            shell_state.history[i] = shell_state.history[i + 1];
        }
        shell_state.history_count--;
    }
    
    shell_state.history[shell_state.history_count] = strdup(line);
    shell_state.history_count++;
    shell_state.history_index = shell_state.history_count;
}

/* Get history entry */
char* shell_get_history(int index) {
    if (index < 0 || index >= shell_state.history_count) {
        return NULL;
    }
    return shell_state.history[index];
}

/* Main shell loop */
void shell_run() {
    char input[MAX_INPUT_LENGTH];
    char* args[MAX_ARGS];
    int argc;
    
    while (shell_state.running) {
        shell_print_prompt();
        
        if (shell_read_line(input, sizeof(input)) == -1) {
            break;
        }
        
        /* Skip empty lines */
        if (strlen(input) == 0) continue;
        
        /* Add to history */
        shell_add_history(input);
        
        /* Parse and execute */
        shell_parse_line(input, args, &argc);
        if (argc > 0) {
            shell_execute(args, argc);
        }
    }
}

/* Cleanup shell */
void shell_cleanup() {
    for (int i = 0; i < shell_state.history_count; i++) {
        free(shell_state.history[i]);
    }
}

/* Main function */
int main(int argc, char* argv[]) {
    shell_init();
    shell_run();
    shell_cleanup();
    return 0;
}
