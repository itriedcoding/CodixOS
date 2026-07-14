/*
 * CodixOS echo - Display a line of text
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Options */
static int no_newline = 0;
static int enable_escape = 0;
static int enable_backslash = 0;

/* Process escape sequences */
void process_escape(const char* str) {
    while (*str) {
        if (*str == '\\' && enable_escape) {
            str++;
            switch (*str) {
                case 'n':
                    putchar('\n');
                    break;
                case 't':
                    putchar('\t');
                    break;
                case 'r':
                    putchar('\r');
                    break;
                case '\\':
                    putchar('\\');
                    break;
                case '"':
                    putchar('"');
                    break;
                case '\'':
                    putchar('\'');
                    break;
                case '0':
                    /* Null character */
                    break;
                default:
                    putchar('\\');
                    putchar(*str);
                    break;
            }
        } else if (*str == '\\' && enable_backslash) {
            str++;
            switch (*str) {
                case 'n':
                    putchar('\n');
                    break;
                case 't':
                    putchar('\t');
                    break;
                case 'r':
                    putchar('\r');
                    break;
                case '\\':
                    putchar('\\');
                    break;
                case '"':
                    putchar('"');
                    break;
                case '\'':
                    putchar('\'');
                    break;
                default:
                    putchar('\\');
                    putchar(*str);
                    break;
            }
        } else {
            putchar(*str);
        }
        str++;
    }
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: echo [OPTION]... [STRING]...\n");
    fprintf(stderr, "Display the STRING(s) to standard output.\n\n");
    fprintf(stderr, "  -n             do not output the trailing newline\n");
    fprintf(stderr, "  -e             enable interpretation of backslash escapes\n");
    fprintf(stderr, "  -E             disable interpretation of backslash escapes (default)\n");
    fprintf(stderr, "  --help         display this help and exit\n");
}

/* Main function */
int codix_echo(int argc, char** argv) {
    int i;
    int start = 1;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-n") == 0) {
            no_newline = 1;
            start = i + 1;
        } else if (strcmp(argv[i], "-e") == 0) {
            enable_escape = 1;
            enable_backslash = 0;
            start = i + 1;
        } else if (strcmp(argv[i], "-E") == 0) {
            enable_escape = 0;
            enable_backslash = 0;
            start = i + 1;
        } else if (strcmp(argv[i], "-ne") == 0 || strcmp(argv[i], "-en") == 0) {
            no_newline = 1;
            enable_escape = 1;
            enable_backslash = 0;
            start = i + 1;
        } else if (strcmp(argv[i], "-nE") == 0 || strcmp(argv[i], "-En") == 0) {
            no_newline = 1;
            enable_escape = 0;
            enable_backslash = 0;
            start = i + 1;
        } else if (argv[i][0] == '-') {
            /* Unknown option */
            fprintf(stderr, "echo: unknown option '%s'\n", argv[i]);
            return 1;
        } else {
            break;
        }
    }
    
    /* Print arguments */
    for (i = start; i < argc; i++) {
        if (i > start) {
            putchar(' ');
        }
        process_escape(argv[i]);
    }
    
    /* Print newline */
    if (!no_newline) {
        putchar('\n');
    }
    
    return 0;
}

int main(int argc, char** argv) {
    return codix_echo(argc, argv);
}
