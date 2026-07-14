/*
 * CodixOS Shell Parser Implementation
 * Handles command line parsing, variable expansion, and quoting
 */

#include "parser.h"
#include <string.h>
#include <stdlib.h>

/* Initialize parser */
void parser_init() {
    /* Nothing to initialize */
}

/* Parse command line into arguments */
int parser_parse(char* line, char** args, int* argc) {
    char* token;
    int count = 0;
    
    /* Make a copy of the line */
    char* line_copy = strdup(line);
    
    /* Tokenize */
    token = strtok(line_copy, " \t");
    while (token != NULL && count < 64) {
        args[count++] = strdup(token);
        token = strtok(NULL, " \t");
    }
    args[count] = NULL;
    *argc = count;
    
    free(line_copy);
    return count;
}

/* Expand environment variables */
int parser_expand_variables(char* input, char* output, int max_len) {
    int out_pos = 0;
    int in_pos = 0;
    
    while (input[in_pos] != '\0' && out_pos < max_len - 1) {
        if (input[in_pos] == '$' && input[in_pos + 1] == '{') {
            /* Variable expansion: ${VAR} */
            in_pos += 2;
            char var_name[256];
            int var_pos = 0;
            
            while (input[in_pos] != '}' && input[in_pos] != '\0' && var_pos < 255) {
                var_name[var_pos++] = input[in_pos++];
            }
            var_name[var_pos] = '\0';
            
            if (input[in_pos] == '}') {
                in_pos++;
            }
            
            char* value = getenv(var_name);
            if (value) {
                int val_len = strlen(value);
                if (out_pos + val_len < max_len - 1) {
                    strcpy(&output[out_pos], value);
                    out_pos += val_len;
                }
            }
        } else if (input[in_pos] == '$' && input[in_pos + 1] == '?') {
            /* Last exit status */
            output[out_pos++] = '0';
            in_pos += 2;
        } else if (input[in_pos] == '$' && input[in_pos + 1] == '$') {
            /* Process ID */
            output[out_pos++] = '1';
            in_pos += 2;
        } else if (input[in_pos] == '~') {
            /* Home directory */
            char* home = getenv("HOME");
            if (home) {
                int home_len = strlen(home);
                if (out_pos + home_len < max_len - 1) {
                    strcpy(&output[out_pos], home);
                    out_pos += home_len;
                }
            }
            in_pos++;
        } else {
            output[out_pos++] = input[in_pos++];
        }
    }
    
    output[out_pos] = '\0';
    return out_pos;
}

/* Handle quoted strings */
int parser_handle_quotes(char* input, char* output, int max_len) {
    int out_pos = 0;
    int in_pos = 0;
    int in_single_quote = 0;
    int in_double_quote = 0;
    
    while (input[in_pos] != '\0' && out_pos < max_len - 1) {
        if (input[in_pos] == '\'' && !in_double_quote) {
            /* Single quote - no variable expansion */
            in_single_quote = !in_single_quote;
            in_pos++;
        } else if (input[in_pos] == '"' && !in_single_quote) {
            /* Double quote - allow variable expansion */
            in_double_quote = !in_double_quote;
            in_pos++;
        } else if (input[in_pos] == '\\' && !in_single_quote) {
            /* Escape character */
            in_pos++;
            if (input[in_pos] != '\0') {
                output[out_pos++] = input[in_pos++];
            }
        } else {
            output[out_pos++] = input[in_pos++];
        }
    }
    
    output[out_pos] = '\0';
    return out_pos;
}

/* Handle wildcards (simplified) */
int parser_handle_wildcards(char* pattern, char* output, int max_len) {
    /* Simple implementation - just copy the pattern */
    strncpy(output, pattern, max_len - 1);
    output[max_len - 1] = '\0';
    return strlen(output);
}
