#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

/*
Define all interrupts here.
*/

#include <stdint.h>
#include "kernel.h"
#include "io.h"

#define _ACK_IRQ_EOI outb(0x20, 0x20);

// The handler decides what values it will use.
struct interrupt_frame {

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;

    // Now since we are having generic handler, it is better to know what this is
    // useful for. So, this esp is set by 'pushad' instruction. As such, it points
    // to the HANDLER's stack immediately after priv change.
    // Simply put: the processor pushes onto stack in the order of SS of the userland
    // task, followed by ESP, EFLAGS, CS, EIP, Err code (if any applicable).
    // Or: the first value accessed by this frame is Err code, EIP, CS, EFLAGS, ESP
    // and SS.
    uint32_t handler_esp; // technically esp but not the one we are concerned with
    
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // since we generally pass the address of stack here, the layout must
    // match the exact behavior by CPU which was mentioned above.

    uint32_t err_code; // Since we are abstracting away interrupt wrappers and having a generic handler

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;

} __attribute__((__packed__));

typedef void*(*ISR80H_COMMAND)(struct interrupt_frame* frame);
typedef void(*INTERRUPT_CALLBACK_FUNCTION)();

void int_zero(void);
void idt_invalid_opcode_fault_handler(void);
void int_gp_fault(struct interrupt_frame* stack_frame);
void unhandled_interrupts_handler_basic(void);

/* Interrupt handlers */
extern void idt_int_zero_handler(void);
extern void invalid_opcode_fault_handler(void);
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