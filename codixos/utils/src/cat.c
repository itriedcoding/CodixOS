/*
 * CodixOS cat - Concatenate files and print
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Options */
static int show_ends = 0;
static int show_line_numbers = 0;
static int show_non_blank = 0;
static int squeeze_blank = 0;

/* Print file contents */
void print_file(const char* filename) {
    FILE* file;
    int c;
    int line_num = 1;
    int prev_blank = 0;
    
    if (strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            perror("cat");
            return;
        }
    }
    
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            if (show_ends) {
                printf("$");
            }
            
            if (squeeze_blank && prev_blank) {
                continue;
            }
            prev_blank = 1;
        } else {
            prev_blank = 0;
        }
        
        if (c == '\n') {
            if (show_line_numbers) {
                printf("%6d\t", line_num++);
            }
        }
        
        if (show_non_blank && c == '\n') {
            /* Don't print blank lines */
        } else {
            putchar(c);
        }
    }
    
    if (file != stdin) {
        fclose(file);
    }
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: cat [OPTION]... [FILE]...\n");
    fprintf(stderr, "Concatenate FILE(s) to standard output.\n\n");
    fprintf(stderr, "  -n, --number     number all output lines\n");
    fprintf(stderr, "  -b, --number-nonblank  number nonempty output lines\n");
    fprintf(stderr, "  -e               display $ at end of each line\n");
    fprintf(stderr, "  -s, --squeeze-blank  suppress repeated empty output lines\n");
    fprintf(stderr, "  --help           display this help and exit\n");
}

/* Main function */
int codix_cat(int argc, char** argv) {
    int i;
    int files_started = 0;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--number") == 0) {
                show_line_numbers = 1;
            } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--number-nonblank") == 0) {
                show_non_blank = 1;
                show_line_numbers = 1;
            } else if (strcmp(argv[i], "-e") == 0) {
                show_ends = 1;
            } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--squeeze-blank") == 0) {
                squeeze_blank = 1;
            } else if (strcmp(argv[i], "-") == 0) {
                files_started = 1;
                print_file("-");
            } else {
                fprintf(stderr, "cat: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            files_started = 1;
            print_file(argv[i]);
        }
    }
    
    if (!files_started) {
        print_file("-");
    }
    
    return 0;
}

int main(int argc, char** argv) {
    return codix_cat(argc, argv);
}
