#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

/*
Define all interrupts here.
*/

#include <stdint.h>
#include "kernel.h"
#include "io.h"

#define _ACK_IRQ_EOI outb(0x20, 0x20);

struct GP_Handler_stack {
    unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned long err_code;
    unsigned long eip;
    unsigned long cs;
    unsigned long eflags;
};

struct interrupt_frame {

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved; // technically esp but not the one we are concerned with
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;

} __attribute__((__packed__));

typedef void*(*ISR80H_COMMAND)(struct interrupt_frame* frame);

void int_zero(void);
void idt_invalid_opcode_fault_handler(void);
void int_gp_fault(struct GP_Handler_stack* stack_frame);
void unhandled_interrupts_handler_basic(void);

/* Interrupt handlers */
extern void idt_int_zero_handler(void);
extern void invalid_opcode_fault_handler(void);

extern void idt_20_h(void);
extern void int_21_h(void);
extern void general_protection_fault(void);
/* Syscall handler */
extern void isr80h_wrapper();

/* IRQs */
void timer_handler(void);
void int_21_handler(void);
void no_interrupt_routine_handler(void);

/* Unhandled interrupts */
extern void unhandled_interrupts(void);
extern void no_interrupt_routine(void); // IRQs

/* Utils */
extern unsigned short get_current_cs(void);
void isr80h_register_command(int command_id, ISR80H_COMMAND handler);

#endif