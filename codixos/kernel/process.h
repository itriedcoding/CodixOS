/*
 * CodixOS Process Management
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "kernel.h"

/* Process states */
#define PROCESS_STATE_RUNNING  0
#define PROCESS_STATE_READY    1
#define PROCESS_STATE_BLOCKED  2
#define PROCESS_STATE_STOPPED  3
#define PROCESS_STATE_ZOMBIE   4

/* Process structure */
typedef struct process {
    uint32_t pid;
    char name[32];
    uint8_t state;
    uint32_t priority;
    uint32_t stack_pointer;
    uint32_t page_directory;
    struct process* next;
} process_t;

/* Functions */
void init_processes();
process_t* create_process(const char* name, uint32_t priority);
void destroy_process(uint32_t pid);
process_t* find_process(uint32_t pid);
void schedule();
void yield();
void exit();

#endif /* PROCESS_H */
