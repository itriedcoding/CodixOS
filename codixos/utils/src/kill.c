/*
 * CodixOS kill - Send signals to processes
 */

#include "../include/codix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/* Signal names */
static const char* signal_names[] = {
    "NULL", "HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "BUS",
    "FPE", "KILL", "USR1", "SEGV", "USR2", "PIPE", "ALRM", "TERM",
    "STKFLT", "CHLD", "CONT", "STOP", "TSTP", "TTIN", "TTOU", "URG",
    "XCPU", "XFSZ", "VTALRM", "PROF", "WINCH", "IO", "PWR", "SYS"
};

/* Get signal number from name */
int get_signal(const char* name) {
    /* Check for numeric signal */
    if (name[0] >= '0' && name[0] <= '9') {
        return atoi(name);
    }
    
    /* Check for signal name */
    if (strcmp(name, "HUP") == 0 || strcmp(name, "SIGHUP") == 0) return SIGHUP;
    if (strcmp(name, "INT") == 0 || strcmp(name, "SIGINT") == 0) return SIGINT;
    if (strcmp(name, "QUIT") == 0 || strcmp(name, "SIGQUIT") == 0) return SIGQUIT;
    if (strcmp(name, "ILL") == 0 || strcmp(name, "SIGILL") == 0) return SIGILL;
    if (strcmp(name, "TRAP") == 0 || strcmp(name, "SIGTRAP") == 0) return SIGTRAP;
    if (strcmp(name, "ABRT") == 0 || strcmp(name, "SIGABRT") == 0) return SIGABRT;
    if (strcmp(name, "BUS") == 0 || strcmp(name, "SIGBUS") == 0) return SIGBUS;
    if (strcmp(name, "FPE") == 0 || strcmp(name, "SIGFPE") == 0) return SIGFPE;
    if (strcmp(name, "KILL") == 0 || strcmp(name, "SIGKILL") == 0) return SIGKILL;
    if (strcmp(name, "USR1") == 0 || strcmp(name, "SIGUSR1") == 0) return SIGUSR1;
    if (strcmp(name, "SEGV") == 0 || strcmp(name, "SIGSEGV") == 0) return SIGSEGV;
    if (strcmp(name, "USR2") == 0 || strcmp(name, "SIGUSR2") == 0) return SIGUSR2;
    if (strcmp(name, "PIPE") == 0 || strcmp(name, "SIGPIPE") == 0) return SIGPIPE;
    if (strcmp(name, "ALRM") == 0 || strcmp(name, "SIGALRM") == 0) return SIGALRM;
    if (strcmp(name, "TERM") == 0 || strcmp(name, "SIGTERM") == 0) return SIGTERM;
    if (strcmp(name, "CHLD") == 0 || strcmp(name, "SIGCHLD") == 0) return SIGCHLD;
    if (strcmp(name, "CONT") == 0 || strcmp(name, "SIGCONT") == 0) return SIGCONT;
    if (strcmp(name, "STOP") == 0 || strcmp(name, "SIGSTOP") == 0) return SIGSTOP;
    if (strcmp(name, "TSTP") == 0 || strcmp(name, "SIGTSTP") == 0) return SIGTSTP;
    if (strcmp(name, "TTIN") == 0 || strcmp(name, "SIGTTIN") == 0) return SIGTTIN;
    if (strcmp(name, "TTOU") == 0 || strcmp(name, "SIGTTOU") == 0) return SIGTTOU;
    if (strcmp(name, "URG") == 0 || strcmp(name, "SIGURG") == 0) return SIGURG;
    if (strcmp(name, "XCPU") == 0 || strcmp(name, "SIGXCPU") == 0) return SIGXCPU;
    if (strcmp(name, "XFSZ") == 0 || strcmp(name, "SIGXFSZ") == 0) return SIGXFSZ;
    if (strcmp(name, "VTALRM") == 0 || strcmp(name, "SIGVTALRM") == 0) return SIGVTALRM;
    if (strcmp(name, "PROF") == 0 || strcmp(name, "SIGPROF") == 0) return SIGPROF;
    if (strcmp(name, "WINCH") == 0 || strcmp(name, "SIGWINCH") == 0) return SIGWINCH;
    if (strcmp(name, "PWR") == 0 || strcmp(name, "SIGPWR") == 0) return SIGPWR;
    if (strcmp(name, "SYS") == 0 || strcmp(name, "SIGSYS") == 0) return SIGSYS;
    
    return -1;
}

/* Print signal names */
void print_signals() {
    printf("Available signals:\n");
    for (int i = 1; i < 32; i++) {
        printf("  %2d - %s\n", i, signal_names[i]);
    }
}

/* Print usage */
void print_usage() {
    fprintf(stderr, "Usage: kill [-s SIGNAL | -SIGNAL] PID...\n");
    fprintf(stderr, "  or: kill -l [SIGNAL]\n");
    fprintf(stderr, "Send signals to processes.\n\n");
    fprintf(stderr, "  -s, --signal=SIGNAL  send specified signal\n");
    fprintf(stderr, "  -l, --list           list signal names\n");
    fprintf(stderr, "  -L, --table          list signal names and numbers\n");
    fprintf(stderr, "  --help               display this help and exit\n");
}

/* Main function */
int codix_kill(int argc, char** argv) {
    int i;
    int signal = SIGTERM;
    int list_signals = 0;
    int result = 0;
    
    /* Parse options */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
            list_signals = 1;
        } else if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--table") == 0) {
            list_signals = 1;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--signal") == 0) {
            if (i + 1 < argc) {
                signal = get_signal(argv[++i]);
                if (signal == -1) {
                    fprintf(stderr, "kill: invalid signal: '%s'\n", argv[i]);
                    return 1;
                }
            } else {
                fprintf(stderr, "kill: option requires an argument -- 's'\n");
                return 1;
            }
        } else if (argv[i][0] == '-') {
            /* Check for signal after dash */
            if (argv[i][1] >= '0' && argv[i][1] <= '9') {
                signal = atoi(argv[i] + 1);
            } else {
                signal = get_signal(argv[i] + 1);
                if (signal == -1) {
                    fprintf(stderr, "kill: invalid signal: '%s'\n", argv[i] + 1);
                    return 1;
                }
            }
        } else {
            /* This is a PID */
            pid_t pid = atoi(argv[i]);
            if (pid <= 0) {
                fprintf(stderr, "kill: invalid PID: '%s'\n", argv[i]);
                result = 1;
                continue;
            }
            
            if (kill(pid, signal) != 0) {
                perror("kill");
                result = 1;
            }
        }
    }
    
    if (list_signals) {
        print_signals();
        return 0;
    }
    
    return result;
}

int main(int argc, char** argv) {
    return codix_kill(argc, argv);
}
