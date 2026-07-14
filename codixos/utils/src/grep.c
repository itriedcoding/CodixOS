/*
 * CodixOS grep - Search for patterns in files
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

/* Options */
static int ignore_case = 0;
static int invert_match = 0;
static int show_line_numbers = 0;
static int count_only = 0;
static int files_with_matches = 0;
static int print_filename = 0;
static int max_count = 0;

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BOLD    "\033[1m"

/* Search pattern in file */
int search_file(const char* filename, const char* pattern, int show_filename) {
    FILE* file;
    char line[4096];
    int line_num = 0;
    int match_count = 0;
    
    /* Open file */
    if (strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            perror("grep");
            return 1;
        }
    }
    
    /* Compile regex */
    regex_t regex;
    int flags = REG_EXTENDED;
    if (ignore_case) {
        flags |= REG_ICASE;
    }
    
    int ret = regcomp(&regex, pattern, flags);
    if (ret != 0) {
        char error_msg[256];
        regerror(ret, &regex, error_msg, sizeof(error_msg));
        fprintf(stderr, "grep: %s\n", error_msg);
        if (file != stdin) fclose(file);
        return 1;
    }
    
    /* Search lines */
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        /* Remove newline */
        line[strcspn(line, "\n")] = 0;
        
        /* Check for match */
        ret = regexec(&regex, line, 0, NULL, 0);
        
        if ((ret == 0 && !invert_match) || (ret != 0 && invert_match)) {
            match_count++;
            
            if (max_count > 0 && match_count > max_count) {
                break;
            }
            
            if (files_with_matches) {
                printf("%s\n", filename);
                break;
            }
            
            if (count_only) {
                continue;
            }
            
            /* Print line */
            if (show_filename || print_filename) {
                printf(COLOR_GREEN "%s" COLOR_RESET ":", filename);
            }
            
            if (show_line_numbers) {
                printf COLOR_GREEN "%d:" COLOR_RESET ":", line_num);
            }
            
            printf("%s\n", line);
        }
    }
    
    /* Cleanup */
    regfree(&regex);
    if (file != stdin) fclose(file);
    
    if (count_only) {
        if (print_filename) {
            printf("%s:", filename);
        }
        printf("%d\n", match_count);
    }
    
    return (match_count > 0) ? 0 : 1;
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: grep [OPTION]... PATTERN [FILE]...\n");
    fprintf(stderr, "Search for PATTERNs in each FILE.\n\n");
    fprintf(stderr, "  -i, --ignore-case         ignore case distinctions\n");
    fprintf(stderr, "  -v, --invert-match        select non-matching lines\n");
    fprintf(stderr, "  -n, --line-number         prefix each line with line number\n");
    fprintf(stderr, "  -c, --count               print only a count of matching lines\n");
    fprintf(stderr, "  -l, --files-with-matches  print only names of FILEs containing matches\n");
    fprintf(stderr, "  -h, --no-filename         suppress the prefixing filename on output\n");
    fprintf(stderr, "  -H, --with-filename       print the filename for each match\n");
    fprintf(stderr, "  -m, --max-count=NUM       stop after NUM matches\n");
    fprintf(stderr, "  --help                    display this help and exit\n");
}

/* Main function */
int codix_grep(int argc, char** argv) {
    int i;
    char* pattern = NULL;
    int file_count = 0;
    int result = 0;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--ignore-case") == 0) {
                ignore_case = 1;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--invert-match") == 0) {
                invert_match = 1;
            } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--line-number") == 0) {
                show_line_numbers = 1;
            } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0) {
                count_only = 1;
            } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--files-with-matches") == 0) {
                files_with_matches = 1;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--no-filename") == 0) {
                print_filename = 0;
            } else if (strcmp(argv[i], "-H") == 0 || strcmp(argv[i], "--with-filename") == 0) {
                print_filename = 1;
            } else if (strncmp(argv[i], "-m=", 3) == 0 || strncmp(argv[i], "--max-count=", 12) == 0) {
                if (argv[i][2] == '=') {
                    max_count = atoi(argv[i] + 3);
                } else {
                    max_count = atoi(argv[i] + 12);
                }
            } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max-count") == 0) {
                if (i + 1 < argc) {
                    max_count = atoi(argv[++i]);
                } else {
                    fprintf(stderr, "grep: option requires an argument -- 'm'\n");
                    return 1;
                }
            } else if (strcmp(argv[i], "-iv") == 0 || strcmp(argv[i], "-vi") == 0) {
                ignore_case = 1;
                invert_match = 1;
            } else if (strcmp(argv[i], "-in") == 0 || strcmp(argv[i], "-ni") == 0) {
                ignore_case = 1;
                show_line_numbers = 1;
            } else if (strcmp(argv[i], "-vn") == 0 || strcmp(argv[i], "-nv") == 0) {
                invert_match = 1;
                show_line_numbers = 1;
            } else {
                fprintf(stderr, "grep: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            if (pattern == NULL) {
                pattern = argv[i];
            } else {
                file_count++;
            }
        }
    }
    
    if (pattern == NULL) {
        fprintf(stderr, "grep: missing pattern\n");
        fprintf(stderr, "Try 'grep --help' for more information.\n");
        return 1;
    }
    
    if (file_count == 0) {
        /* Search stdin */
        return search_file("-", pattern, 0);
    }
    
    /* Search files */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (pattern && strcmp(argv[i], pattern) != 0) {
                if (search_file(argv[i], pattern, file_count > 1) != 0) {
                    result = 1;
                }
            }
        }
    }
    
    return result;
}

int main(int argc, char** argv) {
    return codix_grep(argc, argv);
}
