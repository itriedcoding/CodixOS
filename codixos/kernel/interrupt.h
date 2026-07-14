/*
 * CodixOS Interrupt Handling
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "kernel.h"

/* Interrupt Descriptor Table */
typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

/* Functions */
void init_interrupts();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
void isr_handler(uint32_t int_no);
void irq_handler(uint32_t int_no);

/* IRQ handlers */
void timer_handler();
void keyboard_handler();

#endif /* INTERRUPT_H */
