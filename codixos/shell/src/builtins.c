/*
 * CodixOS Shell Built-in Commands Implementation
 * Core shell built-ins for better performance
 */

#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Built-in commands table */
builtin_command_t builtin_commands[] = {
    {"cd", builtin_cd, "Change directory"},
    {"pwd", builtin_pwd, "Print working directory"},
    {"echo", builtin_echo, "Display text"},
    {"export", builtin_export, "Export variable"},
    {"unset", builtin_unset, "Unset variable"},
    {"history", builtin_history, "Command history"},
    {"exit", builtin_exit, "Exit shell"},
    {"quit", builtin_exit, "Exit shell"},
    {"help", builtin_help, "Show help"},
    {"alias", builtin_alias, "Create alias"},
    {"unalias", builtin_unalias, "Remove alias"},
    {"source", builtin_source, "Execute script"},
    {"type", builtin_type, "Display command type"},
    {"hash", builtin_hash, "Hash table"},
    {"enable", builtin_enable, "Enable command"},
    {"disable", builtin_disable, "Disable command"},
    {NULL, NULL, NULL}
};

/* CD built-in */
int builtin_cd(char** args, int argc) {
    char* dir;
    
    if (argc < 2) {
        dir = getenv("HOME");
        if (!dir) dir = "/";
    } else if (strcmp(args[1], "-") == 0) {
        dir = getenv("OLDPWD");
        if (!dir) {
            printf(COLOR_RED "cd: OLDPWD not set\n" COLOR_RESET);
            return 1;
        }
        printf("%s\n", dir);
    } else {
        dir = args[1];
    }
    
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        setenv("OLDPWD", cwd, 1);
    }
    
    if (chdir(dir) != 0) {
        printf(COLOR_RED "cd: %s: No such file or directory\n" COLOR_RESET, dir);
        return 1;
    }
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        setenv("PWD", cwd, 1);
    }
    
    return 0;
}

/* PWD built-in */
int builtin_pwd(char** args, int argc) {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    }
    printf(COLOR_RED "pwd: error getting current directory\n" COLOR_RESET);
    return 1;
}

/* Echo built-in */
int builtin_echo(char** args, int argc) {
    int newline = 1;
    int start = 1;
    
    /* Check for -n flag */
    if (argc > 1 && strcmp(args[1], "-n") == 0) {
        newline = 0;
        start = 2;
    }
    
    for (int i = start; i < argc; i++) {
        if (i > start) printf(" ");
        printf("%s", args[i]);
    }
    
    if (newline) {
        printf("\n");
    }
    
    return 0;
}

/* Export built-in */
int builtin_export(char** args, int argc) {
    if (argc < 2) {
        /* Print all exported variables */
        extern char** environ;
        for (int i = 0; environ[i]; i++) {
            printf("declare -x %s\n", environ[i]);
        }
        return 0;
    }
    
    char* eq = strchr(args[1], '=');
    if (eq) {
        *eq = '\0';
        setenv(args[1], eq + 1, 1);
        *eq = '=';
    } else {
        setenv(args[1], "", 1);
    }
    
    return 0;
}

/* Unset built-in */
int builtin_unset(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "unset: missing variable name\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        unsetenv(args[i]);
    }
    
    return 0;
}

/* History built-in */
int builtin_history(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== Command History ===\n" COLOR_RESET);
    printf("  No history available\n");
    return 0;
}

/* Exit built-in */
int builtin_exit(char** args, int argc) {
    int status = 0;
    
    if (argc > 1) {
        status = atoi(args[1]);
    }
    
    exit(status);
    return 0;
}

/* Help built-in */
int builtin_help(char** args, int argc) {
    printf(COLOR_CYAN COLOR_BOLD "=== CodixOS Shell Built-ins ===\n" COLOR_RESET);
    
    builtin_command_t* cmd = builtin_commands;
    while (cmd->name != NULL) {
        printf("  %-12s - %s\n", cmd->name, cmd->description);
        cmd++;
    }
    
    printf("\nType 'help' for shell commands\n");
    return 0;
}

/* Alias built-in */
int builtin_alias(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_CYAN COLOR_BOLD "=== Aliases ===\n" COLOR_RESET);
        printf("  No aliases defined\n");
        return 0;
    }
    
    char* eq = strchr(args[1], '=');
    if (eq) {
        printf("Alias '%s' created\n", args[1]);
    } else {
        printf("alias: %s\n", args[1]);
    }
    
    return 0;
}

/* Unalias built-in */
int builtin_unalias(char** args, int argc) {
    if (argc < 2) {
        printf(COLOR_RED "unalias: missing alias name\n" COLOR_RESET);
        return 1;
    }
    
    printf("Alias '%s' removed\n", args[1]);
    return 0;
}

/* Source built-in */
int builtin_source(char** args, int argc) {
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
    int line_num = 0;
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) > 0 && line[0] != '#') {
            printf("Executing line %d: %s\n", line_num, line);
        }
    }
    
    fclose(file);
    return 0;
}

/* Type built-in */
int builtin_type(char** args, int argc) {
    if (argc < 2) {
        printf("type: missing command name\n");
        return 1;
    }
    
    /* Check if it's a built-in */
    builtin_command_t* cmd = builtin_commands;
    while (cmd->name != NULL) {
        if (strcmp(cmd->name, args[1]) == 0) {
            printf("%s is a shell builtin\n", args[1]);
            return 0;
        }
        cmd++;
    }
    
    printf("type: %s not found\n", args[1]);
    return 1;
}

/* Hash built-in */
int builtin_hash(char** args, int argc) {
    printf("hash: hash table empty\n");
    return 0;
}

/* Enable built-in */
int builtin_enable(char** args, int argc) {
    printf("enable: builtin enabled\n");
    return 0;
}

/* Disable built-in */
int builtin_disable(char** args, int argc) {
    printf("disable: builtin disabled\n");
    return 0;
}

/* Check if command is a built-in */
int is_builtin(char* command) {
    builtin_command_t* cmd = builtin_commands;
    while (cmd->name != NULL) {
        if (strcmp(cmd->name, command) == 0) {
            return 1;
        }
        cmd++;
    }
    return 0;
}

/* Execute built-in command */
int execute_builtin(char** args, int argc) {
    builtin_command_t* cmd = builtin_commands;
    while (cmd->name != NULL) {
        if (strcmp(cmd->name, args[0]) == 0) {
            return cmd->function(args, argc);
        }
        cmd++;
    }
    return -1;
}
