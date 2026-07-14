/*
 * CodixOS Process Management Implementation
 */

#include "process.h"
#include "memory.h"
#include "kernel.h"

static process_t* process_list = NULL;
static process_t* current_process = NULL;
static uint32_t next_pid = 1;

void init_processes() {
    /* Create kernel process */
    process_t* kernel = create_process("kernel", 10);
    current_process = kernel;
}

process_t* create_process(const char* name, uint32_t priority) {
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if (!proc) return NULL;
    
    proc->pid = next_pid++;
    strcpy(proc->name, name);
    proc->state = PROCESS_STATE_READY;
    proc->priority = priority;
    proc->stack_pointer = 0;
    proc->page_directory = 0;
    proc->next = NULL;
    
    /* Add to process list */
    if (!process_list) {
        process_list = proc;
    } else {
        process_t* temp = process_list;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = proc;
    }
    
    return proc;
}

void destroy_process(uint32_t pid) {
    process_t* temp = process_list;
    process_t* prev = NULL;
    
    while (temp) {
        if (temp->pid == pid) {
            if (prev) {
                prev->next = temp->next;
            } else {
                process_list = temp->next;
            }
            kfree(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

process_t* find_process(uint32_t pid) {
    process_t* temp = process_list;
    
    while (temp) {
        if (temp->pid == pid) {
            return temp;
        }
        temp = temp->next;
    }
    
    return NULL;
}

void schedule() {
    if (!current_process) return;
    
    /* Simple round-robin scheduling */
    process_t* next = current_process->next;
    if (!next) {
        next = process_list;
    }
    
    current_process = next;
}

void yield() {
    schedule();
}

void exit() {
    if (current_process) {
        current_process->state = PROCESS_STATE_ZOMBIE;
        schedule();
    }
}
