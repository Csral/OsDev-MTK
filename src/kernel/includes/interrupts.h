#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

/*
Define all interrupts here.
*/

#include <stdint.h>
#include "kernel.h"
#include "io.h"

#define _ACK_IRQ_EOI outb(0x20, 0x20);

void int_zero(void);
void idt_invalid_opcode_fault_handler(void);
void int_gp_fault(unsigned long address, uint32_t err_code);
void unhandled_interrupts_handler_basic(void);

/* Interrupt handlers */
extern void idt_int_zero_handler(void);
extern void invalid_opcode_fault_handler(void);

extern void idt_20_h(void);
extern void int_21_h(void);
extern void general_protection_fault(void);

/* IRQs */
void timer_handler(void);
void int_21_handler(void);
void no_interrupt_routine_handler(void);

/* Unhandled interrupts */
extern void unhandled_interrupts(void);
extern void no_interrupt_routine(void); // IRQs

#endif