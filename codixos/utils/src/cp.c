/*
 * CodixOS cp - Copy files
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Options */
static int recursive = 0;
static int force = 0;
static int verbose = 0;
static int preserve = 0;

/* Copy file */
int copy_file(const char* src, const char* dst) {
    FILE* source, *dest;
    char buffer[4096];
    size_t bytes;
    
    /* Check if destination is a directory */
    struct stat st;
    if (stat(dst, &st) == 0 && S_ISDIR(st.st_mode)) {
        /* Append source filename to destination */
        char new_dst[512];
        const char* filename = strrchr(src, '/');
        filename = filename ? filename + 1 : src;
        snprintf(new_dst, sizeof(new_dst), "%s/%s", dst, filename);
        dst = new_dst;
    }
    
    /* Check if source exists */
    if (!file_exists(src)) {
        fprintf(stderr, "cp: cannot stat '%s': No such file or directory\n", src);
        return 1;
    }
    
    /* Check if destination exists and force is not set */
    if (!force && file_exists(dst)) {
        fprintf(stderr, "cp: overwrite '%s'? ", dst);
        char response[10];
        if (fgets(response, sizeof(response), stdin) == NULL || 
            (response[0] != 'y' && response[0] != 'Y')) {
            return 0;
        }
    }
    
    /* Open source file */
    source = fopen(src, "rb");
    if (!source) {
        perror("cp");
        return 1;
    }
    
    /* Open destination file */
    dest = fopen(dst, "wb");
    if (!dest) {
        perror("cp");
        fclose(source);
        return 1;
    }
    
    /* Copy contents */
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytes, dest) != bytes) {
            perror("cp");
            fclose(source);
            fclose(dest);
            return 1;
        }
    }
    
    fclose(source);
    fclose(dest);
    
    /* Preserve permissions if requested */
    if (preserve) {
        struct stat src_stat;
        if (stat(src, &src_stat) == 0) {
            chmod(dst, src_stat.st_mode);
        }
    }
    
    if (verbose) {
        printf("'%s' -> '%s'\n", src, dst);
    }
    
    return 0;
}

/* Copy directory recursively */
int copy_directory(const char* src, const char* dst) {
    /* Create destination directory */
    mkdir(dst, 0755);
    
    /* This is a simplified implementation */
    printf("cp: -r not fully implemented\n");
    return 0;
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: cp [OPTION]... SOURCE DEST\n");
    fprintf(stderr, "  or: cp [OPTION]... SOURCE... DIRECTORY\n");
    fprintf(stderr, "Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n\n");
    fprintf(stderr, "  -r, --recursive    copy directories recursively\n");
    fprintf(stderr, "  -f, --force        remove existing destination files\n");
    fprintf(stderr, "  -v, --verbose      explain what is being done\n");
    fprintf(stderr, "  -p, --preserve     preserve file attributes\n");
    fprintf(stderr, "  --help             display this help and exit\n");
}

/* Main function */
int codix_cp(int argc, char** argv) {
    int i;
    int source_count = 0;
    char* dest = NULL;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--recursive") == 0) {
                recursive = 1;
            } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0) {
                force = 1;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                verbose = 1;
            } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--preserve") == 0) {
                preserve = 1;
            } else if (strcmp(argv[i], "-rf") == 0 || strcmp(argv[i], "-fr") == 0) {
                recursive = 1;
                force = 1;
            } else if (strcmp(argv[i], "-rv") == 0 || strcmp(argv[i], "-vr") == 0) {
                recursive = 1;
                verbose = 1;
            } else if (strcmp(argv[i], "-fv") == 0 || strcmp(argv[i], "-vf") == 0) {
                force = 1;
                verbose = 1;
            } else {
                fprintf(stderr, "cp: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            if (source_count == 0) {
                /* This is the first source */
                source_count++;
            } else {
                /* This could be another source or the destination */
                dest = argv[i];
            }
        }
    }
    
    /* We need at least a source and destination */
    if (source_count == 0 || dest == NULL) {
        fprintf(stderr, "cp: missing file operand\n");
        fprintf(stderr, "Try 'cp --help' for more information.\n");
        return 1;
    }
    
    /* For now, handle single source */
    struct stat st;
    if (stat(argv[1], &st) == 0 && S_ISDIR(st.st_mode)) {
        return copy_directory(argv[1], dest);
    } else {
        return copy_file(argv[1], dest);
    }
}

int main(int argc, char** argv) {
    return codix_cp(argc, argv);
}
