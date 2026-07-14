/*
 * CodixOS Init System
 * PID 1 - System initialization and service management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

/* Colors */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

/* Service structure */
typedef struct {
    char name[64];
    char path[256];
    pid_t pid;
    int running;
    int auto_restart;
    int restart_count;
} service_t;

/* Service table */
#define MAX_SERVICES 32
static service_t services[MAX_SERVICES];
static int service_count = 0;
static int running = 1;

/* Signal handler */
void signal_handler(int sig) {
    switch (sig) {
        case SIGTERM:
        case SIGINT:
            printf("\nShutdown signal received\n");
            running = 0;
            break;
        case SIGCHLD:
            /* Child process terminated */
            while (waitpid(-1, NULL, WNOHANG) > 0);
            break;
    }
}

/* Print banner */
void print_banner() {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD);
    printf("        _____      _               ____   _____ \n");
    printf("       / ____|    | |             |  _ \\ / ____|\n");
    printf("      | |     ___ | | ___  _ __   | |_) | (___  \n");
    printf("      | |    / _ \\| |/ _ \\| '__|  |  _ < \\___ \\ \n");
    printf("      | |___| (_) | | (_) | |     | |_) |____) |\n");
    printf("       \\_____\\___/|_|\\___/|_|     |____/|_____/ \n");
    printf(COLOR_RESET);
    printf("\n");
    printf(COLOR_GREEN "  CodixOS Init System v1.0.0\n" COLOR_RESET);
    printf("  Starting system services...\n\n");
}

/* Initialize system */
void init_system() {
    print_banner();
    
    /* Mount filesystems */
    printf(COLOR_YELLOW "  Mounting filesystems...\n" COLOR_RESET);
    system("mount -t proc proc /proc");
    system("mount -t sysfs sysfs /sys");
    system("mount -t devtmpfs devtmpfs /dev");
    
    /* Set hostname */
    printf(COLOR_YELLOW "  Setting hostname...\n" COLOR_RESET);
    sethostname("codixos", 6);
    
    /* Initialize network */
    printf(COLOR_YELLOW "  Initializing network...\n" COLOR_RESET);
    system("ip link set lo up");
    
    /* Set clock */
    printf(COLOR_YELLOW "  Setting clock...\n" COLOR_RESET);
    system("hwclock -s");
    
    printf(COLOR_GREEN "  System initialized\n\n" COLOR_RESET);
}

/* Start service */
int start_service(const char* name, const char* path, int auto_restart) {
    if (service_count >= MAX_SERVICES) {
        printf(COLOR_RED "Error: Too many services\n" COLOR_RESET);
        return -1;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        execl(path, name, NULL);
        perror("exec failed");
        exit(1);
    } else if (pid > 0) {
        /* Parent process */
        service_t* svc = &services[service_count++];
        strcpy(svc->name, name);
        strcpy(svc->path, path);
        svc->pid = pid;
        svc->running = 1;
        svc->auto_restart = auto_restart;
        svc->restart_count = 0;
        
        printf(COLOR_GREEN "  Started service: %s (PID %d)\n" COLOR_RESET, name, pid);
        return 0;
    } else {
        perror("fork failed");
        return -1;
    }
}

/* Stop service */
int stop_service(const char* name) {
    for (int i = 0; i < service_count; i++) {
        if (strcmp(services[i].name, name) == 0) {
            if (services[i].running) {
                kill(services[i].pid, SIGTERM);
                services[i].running = 0;
                printf(COLOR_YELLOW "  Stopped service: %s\n" COLOR_RESET, name);
                return 0;
            }
        }
    }
    printf(COLOR_RED "Service not found: %s\n" COLOR_RESET, name);
    return -1;
}

/* Start all services */
void start_services() {
    printf(COLOR_YELLOW "  Starting services...\n" COLOR_RESET);
    
    /* Start essential services */
    start_service("udev", "/sbin/udevd --daemon", 1);
    start_service("dbus", "/usr/bin/dbus-daemon --system", 1);
    start_service("network", "/etc/init.d/networking start", 1);
    start_service("cron", "/usr/sbin/crond", 1);
    start_service("syslog", "/usr/sbin/syslogd", 1);
    
    printf(COLOR_GREEN "\n  All services started\n\n" COLOR_RESET);
}

/* Stop all services */
void stop_services() {
    printf(COLOR_YELLOW "\n  Stopping services...\n" COLOR_RESET);
    
    for (int i = service_count - 1; i >= 0; i--) {
        if (services[i].running) {
            stop_service(services[i].name);
        }
    }
    
    printf(COLOR_GREEN "  All services stopped\n" COLOR_RESET);
}

/* Check services */
void check_services() {
    for (int i = 0; i < service_count; i++) {
        if (services[i].running) {
            int status;
            pid_t result = waitpid(services[i].pid, &status, WNOHANG);
            
            if (result == services[i].pid) {
                /* Service died */
                services[i].running = 0;
                printf(COLOR_RED "  Service %s died\n" COLOR_RESET, services[i].name);
                
                if (services[i].auto_restart && services[i].restart_count < 5) {
                    printf(COLOR_YELLOW "  Restarting %s...\n" COLOR_RESET, services[i].name);
                    start_service(services[i].name, services[i].path, 1);
                    services[i].restart_count++;
                }
            }
        }
    }
}

/* Shutdown system */
void shutdown_system(int reboot_flag) {
    printf(COLOR_YELLOW "  System is going down for %s NOW!\n" COLOR_RESET,
           reboot_flag ? "reboot" : "halt");
    
    stop_services();
    
    /* Unmount filesystems */
    printf(COLOR_YELLOW "  Unmounting filesystems...\n" COLOR_RESET);
    system("umount -a");
    
    if (reboot_flag) {
        printf(COLOR_GREEN "  Rebooting...\n" COLOR_RESET);
        reboot(LINUX_REBOOT_CMD_RESTART);
    } else {
        printf(COLOR_GREEN "  Halting...\n" COLOR_RESET);
        reboot(LINUX_REBOOT_CMD_POWER_OFF);
    }
}

/* Print help */
void print_help() {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Init System Commands ===\n" COLOR_RESET);
    printf("\n");
    printf("  status      - Show service status\n");
    printf("  start NAME  - Start a service\n");
    printf("  stop NAME   - Stop a service\n");
    printf("  restart NAME - Restart a service\n");
    printf("  reboot      - Reboot system\n");
    printf("  halt        - Shutdown system\n");
    printf("  help        - Show this help\n");
    printf("  exit        - Exit to shell\n");
    printf("\n");
}

/* Print status */
void print_status() {
    printf("\n");
    printf(COLOR_CYAN COLOR_BOLD "=== Service Status ===\n" COLOR_RESET);
    printf("\n");
    printf("  PID  Status  Name\n");
    printf("  ---  ------  ----\n");
    
    for (int i = 0; i < service_count; i++) {
        printf("  %4d  %s  %s\n",
               services[i].pid,
               services[i].running ? "running" : "stopped",
               services[i].name);
    }
    
    printf("\n");
}

/* Main function */
int main(int argc, char* argv[]) {
    /* Check if we are PID 1 */
    if (getpid() != 1) {
        fprintf(stderr, "Error: init must be run as PID 1\n");
        return 1;
    }
    
    /* Set up signal handlers */
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, signal_handler);
    
    /* Initialize system */
    init_system();
    
    /* Start services */
    start_services();
    
    /* Main loop */
    char input[256];
    while (running) {
        /* Check services */
        check_services();
        
        /* Sleep briefly */
        usleep(100000); /* 100ms */
    }
    
    /* Shutdown */
    shutdown_system(0);
    
    return 0;
}
