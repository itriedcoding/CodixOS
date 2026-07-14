/*
 * CodixOS Shell Header
 */

#ifndef CODIX_SHELL_H
#define CODIX_SHELL_H

/* Shell configuration */
#define MAX_INPUT_LENGTH 1024
#define MAX_HISTORY_SIZE 100
#define MAX_ARGS 64
#define SHELL_VERSION "1.0.0"

/* Shell state */
typedef struct {
    char* history[MAX_HISTORY_SIZE];
    int history_count;
    int history_index;
    char current_dir[256];
    char username[32];
    int running;
} shell_state_t;

/* Function prototypes */
void shell_init();
void shell_run();
void shell_cleanup();
void shell_print_prompt();
int shell_read_line(char* buffer, int max_len);
void shell_parse_line(char* line, char** args, int* argc);
int shell_execute(char** args, int argc);
void shell_add_history(const char* line);
char* shell_get_history(int index);

#endif /* CODIX_SHELL_H */
