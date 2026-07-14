/*
 * CodixOS Shell Built-in Commands
 */

#ifndef BUILTINS_H
#define BUILTINS_H

/* Built-in command structure */
typedef struct {
    char* name;
    int (*function)(char** args, int argc);
    char* description;
} builtin_command_t;

/* Built-in commands table */
extern builtin_command_t builtin_commands[];

/* Built-in functions */
int builtin_cd(char** args, int argc);
int builtin_pwd(char** args, int argc);
int builtin_echo(char** args, int argc);
int builtin_export(char** args, int argc);
int builtin_unset(char** args, int argc);
int builtin_history(char** args, int argc);
int builtin_exit(char** args, int argc);
int builtin_help(char** args, int argc);
int builtin_alias(char** args, int argc);
int builtin_unalias(char** args, int argc);
int builtin_source(char** args, int argc);
int builtin_type(char** args, int argc);
int builtin_hash(char** args, int argc);
int builtin_enable(char** args, int argc);
int builtin_disable(char** args, int argc);

/* Utility functions */
int is_builtin(char* command);
int execute_builtin(char** args, int argc);

#endif /* BUILTINS_H */
