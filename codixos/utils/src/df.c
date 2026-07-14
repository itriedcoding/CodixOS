/*
 * CodixOS df - Display disk space usage
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

/* Options */
static int human_readable = 0;
static int print_type = 0;
static int print_source = 0;

/* Print disk usage */
void print_disk_usage(const char* path) {
    struct statvfs stat;
    
    if (statvfs(path, &stat) != 0) {
        perror("df");
        return;
    }
    
    unsigned long total = stat.f_blocks * stat.f_frsize;
    unsigned long free_space = stat.f_bfree * stat.f_frsize;
    unsigned long available = stat.f_bavail * stat.f_frsize;
    unsigned long used = total - free_space;
    unsigned long used_percent = (total > 0) ? (used * 100 / total) : 0;
    
    if (human_readable) {
        printf("%-20s %8luM %8luM %8luM %3lu%% %s\n",
               path,
               total / (1024 * 1024),
               used / (1024 * 1024),
               available / (1024 * 1024),
               used_percent,
               print_source ? path : "");
    } else {
        printf("%-20s %10lu %10lu %10lu %3lu%% %s\n",
               path,
               total / 1024,
               used / 1024,
               available / 1024,
               used_percent,
               print_source ? path : "");
    }
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: df [OPTION]... [FILE]...\n");
    fprintf(stderr, "Show information about the file system on which each FILE resides,\n");
    fprintf(stderr, "or all file systems by default.\n\n");
    fprintf(stderr, "  -h, --human-readable  print sizes in human readable format (e.g., 1K 234M 2G)\n");
    fprintf(stderr, "  -T, --print-type      print file system type\n");
    fprintf(stderr, "  -t, --type=TYPE       limit listing to file systems of type TYPE\n");
    fprintf(stderr, "  --help                display this help and exit\n");
}

/* Main function */
int codix_df(int argc, char** argv) {
    int i;
    int show_all = 0;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--human-readable") == 0) {
                human_readable = 1;
            } else if (strcmp(argv[i], "-T") == 0 || strcmp(argv[i], "--print-type") == 0) {
                print_type = 1;
            } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
                show_all = 1;
            } else {
                fprintf(stderr, "df: unknown option '%s'\n", argv[i]);
                return 1;
            }
        }
    }
    
    /* Print header */
    if (human_readable) {
        printf("Filesystem             Size  Used Avail Use%% Mounted on\n");
    } else {
        printf("Filesystem           1K-blocks      Used Available Use%% Mounted on\n");
    }
    
    /* Print root filesystem */
    print_disk_usage("/");
    
    /* Print other filesystems if requested */
    if (show_all) {
        print_disk_usage("/proc");
        print_disk_usage("/sys");
        print_disk_usage("/dev");
    }
    
    return 0;
}

int main(int argc, char** argv) {
    return codix_df(argc, argv);
}
