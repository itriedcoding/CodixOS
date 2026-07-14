/*
 * CodixOS Service Manager
 * Service management utilities
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Service directory */
#define SERVICE_DIR "/etc/init.d"
#define PID_DIR     "/var/run"

/* Service structure */
typedef struct {
    char name[64];
    char script[256];
    pid_t pid;
    int status;
} service_info_t;

/* Get service status */
int get_service_status(const char* name) {
    char pidfile[256];
    FILE* file;
    pid_t pid;
    
    snprintf(pidfile, sizeof(pidfile), "%s/%s.pid", PID_DIR, name);
    file = fopen(pidfile, "r");
    if (!file) {
        return 0; /* Not running */
    }
    
    if (fscanf(file, "%d", &pid) == 1) {
        /* Check if process is running */
        if (kill(pid, 0) == 0) {
            fclose(file);
            return 1; /* Running */
        }
    }
    
    fclose(file);
    return 0;
}

/* Start service */
int service_start(const char* name) {
    char script[256];
    char pidfile[256];
    pid_t pid;
    
    snprintf(script, sizeof(script), "%s/%s", SERVICE_DIR, name);
    
    /* Check if script exists */
    if (access(script, X_OK) != 0) {
        printf(COLOR_RED "Service script not found: %s\n" COLOR_RESET, script);
        return 1;
    }
    
    /* Check if already running */
    if (get_service_status(name)) {
        printf(COLOR_YELLOW "Service %s is already running\n" COLOR_RESET, name);
        return 0;
    }
    
    /* Start service */
    pid = fork();
    if (pid == 0) {
        /* Child process */
        execl(script, name, "start", NULL);
        perror("exec failed");
        exit(1);
    } else if (pid > 0) {
        /* Parent process */
        snprintf(pidfile, sizeof(pidfile), "%s/%s.pid", PID_DIR, name);
        FILE* file = fopen(pidfile, "w");
        if (file) {
            fprintf(file, "%d", pid);
            fclose(file);
        }
        
        printf(COLOR_GREEN "Started service: %s\n" COLOR_RESET, name);
        return 0;
    } else {
        perror("fork failed");
        return 1;
    }
}

/* Stop service */
int service_stop(const char* name) {
    char script[256];
    char pidfile[256];
    pid_t pid;
    
    snprintf(script, sizeof(script), "%s/%s", SERVICE_DIR, name);
    
    /* Check if script exists */
    if (access(script, X_OK) != 0) {
        printf(COLOR_RED "Service script not found: %s\n" COLOR_RESET, script);
        return 1;
    }
    
    /* Check if running */
    if (!get_service_status(name)) {
        printf(COLOR_YELLOW "Service %s is not running\n" COLOR_RESET, name);
        return 0;
    }
    
    /* Stop service */
    snprintf(script, sizeof(script), "%s/%s", SERVICE_DIR, name);
    pid = fork();
    if (pid == 0) {
        /* Child process */
        execl(script, name, "stop", NULL);
        perror("exec failed");
        exit(1);
    } else if (pid > 0) {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
        
        /* Remove pidfile */
        snprintf(pidfile, sizeof(pidfile), "%s/%s.pid", PID_DIR, name);
        unlink(pidfile);
        
        printf(COLOR_YELLOW "Stopped service: %s\n" COLOR_RESET, name);
        return 0;
    } else {
        perror("fork failed");
        return 1;
    }
}

/* Restart service */
int service_restart(const char* name) {
    printf("Restarting service: %s\n", name);
    service_stop(name);
    sleep(1);
    return service_start(name);
}

/* List services */
void list_services() {
    DIR* dir;
    struct dirent* entry;
    
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Services ===\n" COLOR_RESET);
    printf("\n");
    printf("  Name                 Status\n");
    printf("  ----                 ------\n");
    
    dir = opendir(SERVICE_DIR);
    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {
                char script[256];
                snprintf(script, sizeof(script), "%s/%s", SERVICE_DIR, entry->d_name);
                
                struct stat st;
                if (stat(script, &st) == 0 && (st.st_mode & S_IXUSR)) {
                    int status = get_service_status(entry->d_name);
                    printf("  %-20s %s\n",
                           entry->d_name,
                           status ? COLOR_GREEN "running" COLOR_RESET : COLOR_RED "stopped" COLOR_RESET);
                }
            }
        }
        closedir(dir);
    }
    
    printf("\n");
}

/* Show service status */
void show_service_status(const char* name) {
    int status = get_service_status(name);
    
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Service: %s ===\n" COLOR_RESET, name);
    printf("\n");
    printf("  Status: %s\n", status ? COLOR_GREEN "running" COLOR_RESET : COLOR_RED "stopped" COLOR_RESET);
    
    if (status) {
        char pidfile[256];
        snprintf(pidfile, sizeof(pidfile), "%s/%s.pid", PID_DIR, name);
        FILE* file = fopen(pidfile, "r");
        if (file) {
            pid_t pid;
            if (fscanf(file, "%d", &pid) == 1) {
                printf("  PID: %d\n", pid);
            }
            fclose(file);
        }
    }
    
    printf("\n");
}

/* Print help */
void print_help() {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Service Manager ===\n" COLOR_RESET);
    printf("\n");
    printf("Usage: service [COMMAND] [SERVICE]\n");
    printf("\n");
    printf(COLOR_GREEN "Commands:\n" COLOR_RESET);
    printf("  start   <service>   Start a service\n");
    printf("  stop    <service>   Stop a service\n");
    printf("  restart <service>   Restart a service\n");
    printf("  status  <service>   Show service status\n");
    printf("  list                List all services\n");
    printf("  help                Show this help\n");
    printf("\n");
}

/* Main function */
int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 0;
    }
    
    if (strcmp(argv[1], "start") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: service start <service>\n");
            return 1;
        }
        return service_start(argv[2]);
    } else if (strcmp(argv[1], "stop") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: service stop <service>\n");
            return 1;
        }
        return service_stop(argv[2]);
    } else if (strcmp(argv[1], "restart") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: service restart <service>\n");
            return 1;
        }
        return service_restart(argv[2]);
    } else if (strcmp(argv[1], "status") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: service status <service>\n");
            return 1;
        }
        show_service_status(argv[2]);
    } else if (strcmp(argv[1], "list") == 0) {
        list_services();
    } else if (strcmp(argv[1], "help") == 0) {
        print_help();
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_help();
        return 1;
    }
    
    return 0;
}
