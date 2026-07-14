/*
 * CodixOS ls - List directory contents
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

/* Options */
static int show_all = 0;
static int long_format = 0;
static int human_readable = 0;
static int color_output = 1;

/* Print file info */
void print_file_info(const char* path, const char* name) {
    struct stat st;
    char fullpath[512];
    
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);
    
    if (stat(fullpath, &st) == 0) {
        if (long_format) {
            /* Permissions */
            printf((S_ISDIR(st.st_mode)) ? "d" : "-");
            printf((st.st_mode & S_IRUSR) ? "r" : "-");
            printf((st.st_mode & S_IWUSR) ? "w" : "-");
            printf((st.st_mode & S_IXUSR) ? "x" : "-");
            printf((st.st_mode & S_IRGRP) ? "r" : "-");
            printf((st.st_mode & S_IWGRP) ? "w" : "-");
            printf((st.st_mode & S_IXGRP) ? "x" : "-");
            printf((st.st_mode & S_IROTH) ? "r" : "-");
            printf((st.st_mode & S_IWOTH) ? "w" : "-");
            printf((st.st_mode & S_IXOTH) ? "x" : "-");
            
            printf(" %3d %5d %5d ", 1, st.st_uid, st.st_gid);
            
            if (human_readable) {
                if (st.st_size < 1024) {
                    printf("%5lldB ", (long long)st.st_size);
                } else if (st.st_size < 1024 * 1024) {
                    printf("%5lldK ", (long long)st.st_size / 1024);
                } else {
                    printf("%5lldM ", (long long)st.st_size / (1024 * 1024));
                }
            } else {
                printf("%8lld ", (long long)st.st_size);
            }
            
            char timebuf[64];
            struct tm* tm = localtime(&st.st_mtime);
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
            printf("%s ", timebuf);
        }
        
        /* Name with color */
        if (color_output) {
            if (S_ISDIR(st.st_mode)) {
                printf(COLOR_BLUE COLOR_BOLD "%s/" COLOR_RESET, name);
            } else if (st.st_mode & S_IXUSR) {
                printf(COLOR_GREEN "%s*" COLOR_RESET, name);
            } else if (S_ISLNK(st.st_mode)) {
                printf(COLOR_MAGENTA "%s@" COLOR_RESET, name);
            } else {
                printf("%s", name);
            }
        } else {
            printf("%s", name);
        }
        
        printf("\n");
    }
}

/* Compare function for qsort */
int compare_names(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

/* List directory */
int codix_ls(int argc, char** argv) {
    const char* path = ".";
    int i;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
                show_all = 1;
            } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--long") == 0) {
                long_format = 1;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--human") == 0) {
                human_readable = 1;
            } else if (strcmp(argv[i], "--color") == 0) {
                color_output = 1;
            } else if (strcmp(argv[i], "--no-color") == 0) {
                color_output = 0;
            } else {
                fprintf(stderr, "ls: unknown option '%s'\n", argv[i]);
                return 1;
            }
        } else {
            path = argv[i];
        }
    }
    
    DIR* dir = opendir(path);
    if (!dir) {
        perror("ls");
        return 1;
    }
    
    /* Read all entries */
    struct dirent* entry;
    int count = 0;
    char** names = NULL;
    
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }
        
        names = realloc(names, sizeof(char*) * (count + 1));
        names[count] = strdup(entry->d_name);
        count++;
    }
    
    closedir(dir);
    
    /* Sort names */
    qsort(names, count, sizeof(char*), compare_names);
    
    /* Print entries */
    for (i = 0; i < count; i++) {
        print_file_info(path, names[i]);
        free(names[i]);
    }
    
    free(names);
    return 0;
}

/* Main function */
int main(int argc, char** argv) {
    return codix_ls(argc, argv);
}
