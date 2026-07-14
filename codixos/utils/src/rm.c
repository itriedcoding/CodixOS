/*
 * CodixOS rm - Remove files
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

/* Options */
static int force = 0;
static int recursive = 0;
static int verbose = 0;
static int interactive = 0;

/* Remove file or directory */
int remove_item(const char* path) {
    struct stat st;
    
    if (stat(path, &st) != 0) {
        if (!force) {
            fprintf(stderr, "rm: cannot remove '%s': No such file or directory\n", path);
            return 1;
        }
        return 0;
    }
    
    /* Check if interactive mode */
    if (interactive) {
        fprintf(stderr, "rm: remove '%s'? ", path);
        char response[10];
        if (fgets(response, sizeof(response), stdin) == NULL || 
            (response[0] != 'y' && response[0] != 'Y')) {
            return 0;
        }
    }
    
    /* Remove directory */
    if (S_ISDIR(st.st_mode)) {
        if (!recursive) {
            fprintf(stderr, "rm: cannot remove '%s': Is a directory\n", path);
            return 1;
        }
        
        DIR* dir = opendir(path);
        if (!dir) {
            perror("rm");
            return 1;
        }
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            
            char child_path[512];
            snprintf(child_path, sizeof(child_path), "%s/%s", path, entry->d_name);
            remove_item(child_path);
        }
        
        closedir(dir);
        
        if (rmdir(path) != 0) {
            perror("rm");
            return 1;
        }
    } else {
        /* Remove file */
        if (unlink(path) != 0) {
            perror("rm");
            return 1;
        }
    }
    
    if (verbose) {
        printf("removed '%s'\n", path);
    }
    
    return 0;
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: rm [OPTION]... FILE...\n");
    fprintf(stderr, "Remove (unlink) the FILE(s).\n\n");
    fprintf(stderr, "  -f, --force           ignore nonexistent files, never prompt\n");
    fprintf(stderr, "  -i                    prompt before every removal\n");
    fprintf(stderr, "  -I                    prompt once before removing more than three files\n");
    fprintf(stderr, "                        or when removing recursively\n");
    fprintf(stderr, "  -r, -R, --recursive   remove directories and their contents recursively\n");
    fprintf(stderr, "  -v, --verbose         explain what is being done\n");
    fprintf(stderr, "  --help                display this help and exit\n");
}

/* Main function */
int codix_rm(int argc, char** argv) {
    int i;
    int result = 0;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0) {
                force = 1;
                interactive = 0;
            } else if (strcmp(argv[i], "-i") == 0) {
                interactive = 1;
                force = 0;
            } else if (strcmp(argv[i], "-I") == 0) {
                interactive = 1;
            } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "-R") == 0 || 
                       strcmp(argv[i], "--recursive") == 0) {
                recursive = 1;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-rf") == 0 || strcmp(argv[i], "-fr") == 0 || 
                       strcmp(argv[i], "-Rf") == 0 || strcmp(argv[i], "-fR") == 0) {
                recursive = 1;
                force = 1;
            } else if (strcmp(argv[i], "-rv") == 0 || strcmp(argv[i], "-vr") == 0 || 
                       strcmp(argv[i], "-Rv") == 0 || strcmp(argv[i], "-vR") == 0) {
                recursive = 1;
                verbose = 1;
            } else if (strcmp(argv[i], "-ri") == 0 || strcmp(argv[i], "-ir") == 0 || 
                       strcmp(argv[i], "-Ri") == 0 || strcmp(argv[i], "-iR") == 0) {
                recursive = 1;
                interactive = 1;
            } else {
                fprintf(stderr, "rm: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            if (remove_item(argv[i]) != 0) {
                result = 1;
            }
        }
    }
    
    if (i == 1) {
        fprintf(stderr, "rm: missing file operand\n");
        fprintf(stderr, "Try 'rm --help' for more information.\n");
        return 1;
    }
    
    return result;
}

int main(int argc, char** argv) {
    return codix_rm(argc, argv);
}
