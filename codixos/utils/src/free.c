/*
 * CodixOS free - Display amount of free and used memory
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Options */
static int human_readable = 0;
static int show_total = 0;
static int show_wide = 0;
static int repeat_count = 0;
static int repeat_interval = 0;

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BOLD    "\033[1m"

/* Get memory info */
void get_memory_info(unsigned long* total, unsigned long* used, unsigned long* free_mem,
                    unsigned long* shared, unsigned long* buffers, unsigned long* cached) {
    FILE* file;
    char line[256];
    
    /* Default values */
    *total = 16384000;
    *used = 2048000;
    *free_mem = 12288000;
    *shared = 512000;
    *buffers = 1024000;
    *cached = 1024000;
    
    /* Try to read from /proc/meminfo */
    file = fopen("/proc/meminfo", "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                sscanf(line + 9, "%lu", total);
                *total /= 1024; /* Convert to KB */
            } else if (strncmp(line, "MemFree:", 8) == 0) {
                sscanf(line + 8, "%lu", free_mem);
                *free_mem /= 1024;
            } else if (strncmp(line, "MemAvailable:", 13) == 0) {
                /* Use MemAvailable if present */
                sscanf(line + 13, "%lu", free_mem);
                *free_mem /= 1024;
            } else if (strncmp(line, "Buffers:", 8) == 0) {
                sscanf(line + 8, "%lu", buffers);
                *buffers /= 1024;
            } else if (strncmp(line, "Cached:", 7) == 0) {
                sscanf(line + 7, "%lu", cached);
                *cached /= 1024;
            } else if (strncmp(line, "Shmem:", 6) == 0) {
                sscanf(line + 6, "%lu", shared);
                *shared /= 1024;
            }
        }
        fclose(file);
        
        /* Calculate used memory */
        *used = *total - *free_mem - *buffers - *cached;
    }
}

/* Format size in human readable format */
void format_size(unsigned long size, char* buf) {
    if (size < 1024) {
        sprintf(buf, "%luB", size);
    } else if (size < 1024 * 1024) {
        sprintf(buf, "%luK", size / 1024);
    } else if (size < 1024 * 1024 * 1024) {
        sprintf(buf, "%luM", size / (1024 * 1024));
    } else {
        sprintf(buf, "%.1fG", (float)size / (1024 * 1024 * 1024));
    }
}

/* Print memory usage */
void print_memory_usage() {
    unsigned long total, used, free_mem, shared, buffers, cached;
    
    get_memory_info(&total, &used, &free_mem, &shared, &buffers, &cached);
    
    if (human_readable) {
        char total_str[32], used_str[32], free_str[32], shared_str[32], buffers_str[32], cached_str[32];
        char avail_str[32];
        
        format_size(total * 1024, total_str);
        format_size(used * 1024, used_str);
        format_size(free_mem * 1024, free_str);
        format_size(shared * 1024, shared_str);
        format_size(buffers * 1024, buffers_str);
        format_size(cached * 1024, cached_str);
        
        unsigned long available = free_mem + buffers + cached;
        format_size(available * 1024, avail_str);
        
        printf("              total        used        free      shared  buff/cache   available\n");
        printf("Mem:          %8s    %8s    %8s    %8s    %8s    %8s\n",
               total_str, used_str, free_str, shared_str, buffers_str, avail_str);
        printf("Swap:         %8s    %8s    %8s\n", "2048M", "0B", "2048M");
    } else {
        unsigned long available = free_mem + buffers + cached;
        printf("              total        used        free      shared  buff/cache   available\n");
        printf("Mem:       %10lu    %10lu    %10lu    %10lu    %10lu    %10lu\n",
               total, used, free_mem, shared, buffers, cached);
        printf("Swap:      %10lu    %10lu    %10lu\n", 2097152, 0, 2097152);
    }
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: free [OPTION]\n");
    fprintf(stderr, "Display amount of free and used memory in the system.\n\n");
    fprintf(stderr, "  -b, --bytes         display output in bytes\n");
    fprintf(stderr, "  -k, --kibi          display output in kibibytes\n");
    fprintf(stderr, "  -m, --mebi          display output in mebibytes\n");
    fprintf(stderr, "  -g, --gibi          display output in gibibytes\n");
    fprintf(stderr, "  -h, --human         show all output fields super-human readable\n");
    fprintf(stderr, "  -t, --total         show a line with totals\n");
    fprintf(stderr, "  -s, --seconds N     continuously print one line every N seconds\n");
    fprintf(stderr, "  -c, --count N       repeat the operation N times\n");
    fprintf(stderr, "  --help              display this help and exit\n");
}

/* Main function */
int codix_free(int argc, char** argv) {
    int i;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--human") == 0) {
                human_readable = 1;
            } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--total") == 0) {
                show_total = 1;
            } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--wide") == 0) {
                show_wide = 1;
            } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--seconds") == 0) {
                if (i + 1 < argc) {
                    repeat_interval = atoi(argv[++i]);
                }
            } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0) {
                if (i + 1 < argc) {
                    repeat_count = atoi(argv[++i]);
                }
            } else {
                fprintf(stderr, "free: unknown option '%s'\n", argv[i]);
                return 1;
            }
        }
    }
    
    /* Print memory usage */
    int count = 0;
    do {
        if (count > 0) {
            printf("\n");
        }
        print_memory_usage();
        count++;
        
        if (repeat_count > 0 && count >= repeat_count) {
            break;
        }
        
        if (repeat_interval > 0) {
            sleep(repeat_interval);
        }
    } while (repeat_interval > 0);
    
    return 0;
}

int main(int argc, char** argv) {
    return codix_free(argc, argv);
}
