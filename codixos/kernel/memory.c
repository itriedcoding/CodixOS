/*
 * CodixOS Memory Management Implementation
 */

#include "memory.h"
#include "io.h"

static memory_block_t* memory_list = NULL;
static uint32_t total_memory = 0;
static uint32_t used_memory = 0;

void init_memory() {
    /* Detect memory using BIOS INT 15h, E820 */
    total_memory = 128 * 1024 * 1024; /* Default 128MB */
    used_memory = KERNEL_MEMORY_END - KERNEL_MEMORY_START;
    
    /* Create initial memory block */
    memory_list = (memory_block_t*)KERNEL_MEMORY_START;
    memory_list->start = KERNEL_MEMORY_END;
    memory_list->size = total_memory - KERNEL_MEMORY_END;
    memory_list->used = 0;
    memory_list->next = NULL;
}

void* kmalloc(uint32_t size) {
    memory_block_t* block = memory_list;
    
    while (block) {
        if (!block->used && block->size >= size) {
            if (block->size > size + sizeof(memory_block_t)) {
                /* Split block */
                memory_block_t* new_block = (memory_block_t*)(block->start + size);
                new_block->start = block->start + size;
                new_block->size = block->size - size;
                new_block->used = 0;
                new_block->next = block->next;
                block->next = new_block;
            }
            block->used = 1;
            block->size = size;
            used_memory += size;
            return (void*)block->start;
        }
        block = block->next;
    }
    
    return NULL;
}

void kfree(void* ptr) {
    memory_block_t* block = memory_list;
    
    while (block) {
        if ((uint32_t)ptr == block->start) {
            block->used = 0;
            used_memory -= block->size;
            
            /* Merge with next block if free */
            if (block->next && !block->next->used) {
                block->size += block->next->size;
                block->next = block->next->next;
            }
            return;
        }
        block = block->next;
    }
}

void* calloc(uint32_t count, uint32_t size) {
    uint32_t total = count * size;
    void* ptr = kmalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

uint32_t get_free_memory() {
    return total_memory - used_memory;
}

uint32_t get_used_memory() {
    return used_memory;
}

uint32_t get_total_memory() {
    return total_memory;
}
