/*
 * CodixOS Interrupt Handling Implementation
 */

#include "interrupt.h"
#include "io.h"
#include "kernel.h"

#define IDT_ENTRIES 256
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

/* IRQ handler function pointers */
extern void timer_callback();
extern void keyboard_handler();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void remap_pic() {
    /* Send ICW1 */
    outb(0x20, 0x11);
    io_delay();
    outb(0xA0, 0x11);
    io_delay();
    
    /* Send ICW2 */
    outb(0x21, 0x20);
    io_delay();
    outb(0xA1, 0x28);
    io_delay();
    
    /* Send ICW3 */
    outb(0x21, 0x04);
    io_delay();
    outb(0xA1, 0x02);
    io_delay();
    
    /* Send ICW4 */
    outb(0x21, 0x01);
    io_delay();
    outb(0xA1, 0x01);
    io_delay();
    
    /* Mask all interrupts */
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void init_interrupts() {
    /* Set up IDT pointer */
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    /* Clear IDT */
    memset(&idt, 0, sizeof(idt_entry_t) * IDT_ENTRIES);
    
    /* Remap PIC */
    remap_pic();
    
    /* Enable keyboard and timer IRQs */
    outb(0x21, 0xFD); /* Enable IRQ1 (keyboard) */
    outb(0xA1, 0xFF); /* Disable all slave IRQs */
    
    /* Load IDT */
    __asm__ __volatile__("lidt %0" : : "m"(idt_ptr));
    
    /* Enable interrupts */
    __asm__ __volatile__("sti");
}

void isr_handler(uint32_t int_no) {
    /* Handle CPU exceptions */
    if (int_no < 32) {
        /* Exception occurred */
        print("Exception: ");
        print_int(int_no);
        print("\n");
        
        /* Halt on exception */
        while (1) {
            __asm__ __volatile__("cli; hlt");
        }
    }
}

void irq_handler(uint32_t int_no) {
    /* Send EOI */
    if (int_no >= 8) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
    
    /* Handle IRQ */
    switch (int_no) {
        case 0: /* Timer */
            timer_callback();
            break;
        case 1: /* Keyboard */
            keyboard_handler();
            break;
    }
}

void timer_handler() {
    timer_callback();
}

void keyboard_handler() {
    /* Keyboard interrupt handled by polling in kernel_main */
}
