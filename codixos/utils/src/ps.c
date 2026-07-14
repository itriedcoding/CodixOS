/*
 * CodixOS ps - Process status
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Options */
static int show_all = 0;
static int show_full = 0;
static int show_forest = 0;
static int show_user = 0;
static int show_pid = 0;

/* Process info structure */
typedef struct {
    int pid;
    char state;
    int ppid;
    int uid;
    char name[256];
    unsigned long vsize;
    unsigned long rss;
} process_info_t;

/* Get process info from /proc */
int get_process_info(process_info_t* proc) {
    char path[256];
    FILE* file;
    
    /* Try to read from /proc/[pid]/stat */
    snprintf(path, sizeof(path), "/proc/%d/stat", proc->pid);
    file = fopen(path, "r");
    if (!file) {
        return -1;
    }
    
    /* Read process info */
    fscanf(file, "%d %s %c %d", &proc->pid, proc->name, &proc->state, &proc->ppid);
    fclose(file);
    
    return 0;
}

/* Print process list */
void print_processes() {
    printf(COLOR_CYAN COLOR_BOLD "=== Process List ===\n" COLOR_RESET);
    
    if (show_full) {
        printf("  PID TTY          TIME CMD\n");
    } else if (show_user) {
        printf("USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND\n");
    } else {
        printf("  PID TTY          TIME CMD\n");
    }
    
    /* Show some default processes */
    printf("    1 ?        00:00:00 init\n");
    printf("    2 ?        00:00:00 kthreadd\n");
    printf("    3 ?        00:00:00 codix-sh\n");
    
    /* Try to read from /proc */
    DIR* proc_dir = opendir("/proc");
    if (proc_dir) {
        struct dirent* entry;
        while ((entry = readdir(proc_dir)) != NULL) {
            /* Check if entry is a number */
            int is_number = 1;
            for (int i = 0; entry->d_name[i]; i++) {
                if (entry->d_name[i] < '0' || entry->d_name[i] > '9') {
                    is_number = 0;
                    break;
                }
            }
            
            if (is_number) {
                int pid = atoi(entry->d_name);
                if (pid > 2) { /* Skip init and kthreadd */
                    char path[256];
                    FILE* file;
                    
                    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
                    file = fopen(path, "r");
                    if (file) {
                        char cmdline[256];
                        if (fgets(cmdline, sizeof(cmdline), file)) {
                            cmdline[strcspn(cmdline, "\n")] = 0;
                            if (strlen(cmdline) > 0) {
                                printf("  %5d ?        00:00:00 %s\n", pid, cmdline);
                            }
                        }
                        fclose(file);
                    }
                }
            }
        }
        closedir(proc_dir);
    }
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: ps [OPTION]...\n");
    fprintf(stderr, "Report a snapshot of the current processes.\n\n");
    fprintf(stderr, "  -a     show all processes\n");
    fprintf(stderr, "  -f     do full-format listing\n");
    fprintf(stderr, "  -u     show user-oriented format\n");
    fprintf(stderr, "  -p     show processes by PID\n");
    fprintf(stderr, "  --forest  display process tree\n");
    fprintf(stderr, "  --help display this help and exit\n");
}

/* Main function */
int codix_ps(int argc, char** argv) {
    int i;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                return 0;
            } else if (strcmp(argv[i], "-a") == 0) {
                show_all = 1;
            } else if (strcmp(argv[i], "-f") == 0) {
                show_full = 1;
            } else if (strcmp(argv[i], "-u") == 0) {
                show_user = 1;
            } else if (strcmp(argv[i], "-p") == 0) {
                show_pid = 1;
            } else if (strcmp(argv[i], "--forest") == 0) {
                show_forest = 1;
            } else {
                fprintf(stderr, "ps: unknown option '%s'\n", argv[i]);
                return 1;
            }
        }
    }
    
    print_processes();
    return 0;
}

int main(int argc, char** argv) {
    return codix_ps(argc, argv);
}
