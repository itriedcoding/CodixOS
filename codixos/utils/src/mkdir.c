/*
 * CodixOS mkdir - Make directories
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Options */
static int verbose = 0;
static int parents = 0;
static int mode = 0755;

/* Create directory */
int create_directory(const char* path, int mode) {
    struct stat st;
    
    /* Check if directory already exists */
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (!parents) {
                fprintf(stderr, "mkdir: cannot create directory '%s': File exists\n", path);
                return 1;
            }
            return 0;
        } else {
            fprintf(stderr, "mkdir: cannot create directory '%s': File exists\n", path);
            return 1;
        }
    }
    
    /* Create directory */
    if (mkdir(path, mode) != 0) {
        perror("mkdir");
        return 1;
    }
    
    if (verbose) {
        printf("mkdir: created directory '%s'\n", path);
    }
    
    return 0;
}

/* Create directory with parents */
int create_directory_parents(const char* path, int mode) {
    char* path_copy = strdup(path);
    char* token;
    char current_path[512] = "";
    
    /* Handle absolute path */
    if (path_copy[0] == '/') {
        strcpy(current_path, "/");
    }
    
    token = strtok(path_copy, "/");
    while (token != NULL) {
        if (strlen(current_path) > 1) {
            strcat(current_path, "/");
        }
        strcat(current_path, token);
        
        if (create_directory(current_path, mode) != 0) {
            free(path_copy);
            return 1;
        }
        
        token = strtok(NULL, "/");
    }
    
    free(path_copy);
    return 0;
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: mkdir [OPTION]... DIRECTORY...\n");
    fprintf(stderr, "Create the DIRECTORY(ies), if they do not already exist.\n\n");
    fprintf(stderr, "  -m, --mode=MODE   set file mode (as in chmod), not a=rwx - umask\n");
    fprintf(stderr, "  -p, --parents     no error if existing, make parent directories as needed\n");
    fprintf(stderr, "  -v, --verbose     print a message for each created directory\n");
    fprintf(stderr, "  --help            display this help and exit\n");
}

/* Main function */
int codix_mkdir(int argc, char** argv) {
    int i;
    int result = 0;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--parents") == 0) {
                parents = 1;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-m") == 0 || strncmp(argv[i], "--mode=", 7) == 0) {
                if (strcmp(argv[i], "-m") == 0) {
                    if (i + 1 < argc) {
                        mode = strtol(argv[++i], NULL, 8);
                    } else {
                        fprintf(stderr, "mkdir: option requires an argument -- 'm'\n");
                        return 1;
                    }
                } else {
                    mode = strtol(argv[i] + 7, NULL, 8);
                }
            } else {
                fprintf(stderr, "mkdir: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            if (parents) {
                if (create_directory_parents(argv[i], mode) != 0) {
                    result = 1;
                }
            } else {
                if (create_directory(argv[i], mode) != 0) {
                    result = 1;
                }
            }
        }
    }
    
    if (i == 1) {
        fprintf(stderr, "mkdir: missing operand\n");
        fprintf(stderr, "Try 'mkdir --help' for more information.\n");
        return 1;
    }
    
    return result;
}

int main(int argc, char** argv) {
    return codix_mkdir(argc, argv);
}
