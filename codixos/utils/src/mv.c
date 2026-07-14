/*
 * CodixOS mv - Move/rename files
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Options */
static int force = 0;
static int verbose = 0;
static int no_clobber = 0;

/* Move file */
int move_file(const char* src, const char* dst) {
    struct stat src_stat, dst_stat;
    
    /* Check if source exists */
    if (stat(src, &src_stat) != 0) {
        fprintf(stderr, "mv: cannot stat '%s': No such file or directory\n", src);
        return 1;
    }
    
    /* Check if destination exists */
    if (stat(dst, &dst_stat) == 0) {
        if (no_clobber) {
            fprintf(stderr, "mv: cannot overwrite '%s'\n", dst);
            return 1;
        }
        
        /* If destination is a directory, move source into it */
        if (S_ISDIR(dst_stat.st_mode)) {
            char new_dst[512];
            const char* filename = strrchr(src, '/');
            filename = filename ? filename + 1 : src;
            snprintf(new_dst, sizeof(new_dst), "%s/%s", dst, filename);
            dst = new_dst;
        } else if (!force) {
            fprintf(stderr, "mv: overwrite '%s'? ", dst);
            char response[10];
            if (fgets(response, sizeof(response), stdin) == NULL || 
                (response[0] != 'y' && response[0] != 'Y')) {
                return 0;
            }
        }
    }
    
    /* Try rename first */
    if (rename(src, dst) == 0) {
        if (verbose) {
            printf("'%s' -> '%s'\n", src, dst);
        }
        return 0;
    }
    
    /* If rename fails, copy and delete */
    FILE* source = fopen(src, "rb");
    if (!source) {
        perror("mv");
        return 1;
    }
    
    FILE* dest = fopen(dst, "wb");
    if (!dest) {
        perror("mv");
        fclose(source);
        return 1;
    }
    
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytes, dest) != bytes) {
            perror("mv");
            fclose(source);
            fclose(dest);
            return 1;
        }
    }
    
    fclose(source);
    fclose(dest);
    
    /* Remove source */
    if (unlink(src) != 0) {
        perror("mv");
        return 1;
    }
    
    if (verbose) {
        printf("'%s' -> '%s'\n", src, dst);
    }
    
    return 0;
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: mv [OPTION]... SOURCE DEST\n");
    fprintf(stderr, "  or: mv [OPTION]... SOURCE... DIRECTORY\n");
    fprintf(stderr, "Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n\n");
    fprintf(stderr, "  -f, --force              do not prompt before overwriting\n");
    fprintf(stderr, "  -n, --no-clobber         do not overwrite an existing file\n");
    fprintf(stderr, "  -v, --verbose            explain what is being done\n");
    fprintf(stderr, "  --help                   display this help and exit\n");
}

/* Main function */
int codix_mv(int argc, char** argv) {
    int i;
    int source_count = 0;
    char* dest = NULL;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0) {
                force = 1;
            } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--no-clobber") == 0) {
                no_clobber = 1;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-fn") == 0 || strcmp(argv[i], "-nf") == 0) {
                force = 1;
                no_clobber = 1;
            } else if (strcmp(argv[i], "-fv") == 0 || strcmp(argv[i], "-vf") == 0) {
                force = 1;
                verbose = 1;
            } else {
                fprintf(stderr, "mv: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            source_count++;
            dest = argv[i];
        }
    }
    
    if (source_count < 2) {
        fprintf(stderr, "mv: missing file operand\n");
        fprintf(stderr, "Try 'mv --help' for more information.\n");
        return 1;
    }
    
    /* Move each source to destination */
    for (i = 1; i < argc - 1; i++) {
        if (argv[i][0] != '-') {
            if (move_file(argv[i], dest) != 0) {
                return 1;
            }
        }
    }
    
    return 0;
}

int main(int argc, char** argv) {
    return codix_mv(argc, argv);
}
