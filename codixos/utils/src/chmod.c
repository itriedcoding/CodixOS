/*
 * CodixOS chmod - Change file permissions
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Options */
static int verbose = 0;
static int recursive = 0;

/* Parse permission string */
mode_t parse_mode(const char* mode_str) {
    mode_t mode = 0;
    
    /* Check if numeric mode */
    if (mode_str[0] >= '0' && mode_str[0] <= '7') {
        return strtol(mode_str, NULL, 8);
    }
    
    /* Parse symbolic mode */
    const char* p = mode_str;
    char who = 'a';
    
    while (*p) {
        /* Parse who */
        if (*p == 'u' || *p == 'g' || *p == 'o' || *p == 'a') {
            who = *p;
            p++;
            continue;
        }
        
        /* Parse operator */
        if (*p == '+' || *p == '-' || *p == '=') {
            char op = *p;
            p++;
            
            /* Parse permissions */
            while (*p && *p != ',' && *p != '+' && *p != '-' && *p != '=') {
                mode_t perm = 0;
                
                switch (*p) {
                    case 'r': perm = S_IRUSR | S_IRGRP | S_IROTH; break;
                    case 'w': perm = S_IWUSR | S_IWGRP | S_IWOTH; break;
                    case 'x': perm = S_IXUSR | S_IXGRP | S_IXOTH; break;
                    case 's': perm = S_ISUID | S_ISGID; break;
                    case 't': perm = S_ISVTX; break;
                    default:
                        fprintf(stderr, "chmod: invalid permission: '%c'\n", *p);
                        return 0;
                }
                
                /* Apply who mask */
                switch (who) {
                    case 'u': perm &= (S_IRUSR | S_IWUSR | S_IXUSR | S_ISUID); break;
                    case 'g': perm &= (S_IRGRP | S_IWGRP | S_IXGRP | S_ISGID); break;
                    case 'o': perm &= (S_IROTH | S_IWOTH | S_IXOTH | S_ISVTX); break;
                    case 'a': break;
                }
                
                /* Apply operation */
                switch (op) {
                    case '+': mode |= perm; break;
                    case '-': mode &= ~perm; break;
                    case '=': mode = perm; break;
                }
                
                p++;
            }
        } else {
            p++;
        }
    }
    
    return mode;
}

/* Change permissions recursively */
int chmod_recursive(const char* path, mode_t mode) {
    struct stat st;
    
    if (stat(path, &st) != 0) {
        perror("chmod");
        return 1;
    }
    
    if (chmod(path, mode) != 0) {
        perror("chmod");
        return 1;
    }
    
    if (verbose) {
        printf("changed permissions of '%s'\n", path);
    }
    
    if (S_ISDIR(st.st_mode) && recursive) {
        DIR* dir = opendir(path);
        if (!dir) {
            perror("chmod");
            return 1;
        }
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            
            char child_path[512];
            snprintf(child_path, sizeof(child_path), "%s/%s", path, entry->d_name);
            chmod_recursive(child_path, mode);
        }
        
        closedir(dir);
    }
    
    return 0;
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: chmod [OPTION]... MODE FILE...\n");
    fprintf(stderr, "Change the mode of each FILE to MODE.\n\n");
    fprintf(stderr, "  -c, --changes          like verbose but report only when a change is made\n");
    fprintf(stderr, "  -f, --silent, --quiet  suppress most error messages\n");
    fprintf(stderr,  "  -v, --verbose          output a diagnostic for every file processed\n");
    fprintf(stderr, "  -R, --recursive        change files and directories recursively\n");
    fprintf(stderr, "  --help                 display this help and exit\n");
}

/* Main function */
int codix_chmod(int argc, char** argv) {
    int i;
    char* mode_str = NULL;
    int result = 0;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--changes") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--silent") == 0 || 
                       strcmp(argv[i], "--quiet") == 0) {
                /* Quiet mode */
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "--recursive") == 0) {
                recursive = 1;
            } else if (strcmp(argv[i], "-cf") == 0 || strcmp(argv[i], "-fc") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-Rv") == 0 || strcmp(argv[i], "-vR") == 0) {
                recursive = 1;
                verbose = 1;
            } else {
                fprintf(stderr, "chmod: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            if (mode_str == NULL) {
                mode_str = argv[i];
            } else {
                mode_t mode = parse_mode(mode_str);
                if (mode == 0 && strcmp(mode_str, "0") != 0) {
                    fprintf(stderr, "chmod: invalid mode: '%s'\n", mode_str);
                    return 1;
                }
                
                if (recursive) {
                    if (chmod_recursive(argv[i], mode) != 0) {
                        result = 1;
                    }
                } else {
                    if (chmod(argv[i], mode) != 0) {
                        perror("chmod");
                        result = 1;
                    } else if (verbose) {
                        printf("changed permissions of '%s'\n", argv[i]);
                    }
                }
            }
        }
    }
    
    if (mode_str == NULL) {
        fprintf(stderr, "chmod: missing operand\n");
        fprintf(stderr, "Try 'chmod --help' for more information.\n");
        return 1;
    }
    
    return result;
}

int main(int argc, char** argv) {
    return codix_chmod(argc, argv);
}
