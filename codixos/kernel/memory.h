/*
 * CodixOS Memory Management
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "kernel.h"

/* Memory regions */
#define KERNEL_MEMORY_START 0x100000
#define KERNEL_MEMORY_END   0x8000000
#define USER_MEMORY_START   0x8000000
#define USER_MEMORY_END     0xF000000

/* Memory block */
typedef struct memory_block {
    uint32_t start;
    uint32_t size;
    int used;
    struct memory_block* next;
} memory_block_t;

/* Functions */
void init_memory();
void* kmalloc(uint32_t size);
void kfree(void* ptr);
void* calloc(uint32_t count, uint32_t size);
uint32_t get_free_memory();
uint32_t get_used_memory();
uint32_t get_total_memory();

#endif /* MEMORY_H */
