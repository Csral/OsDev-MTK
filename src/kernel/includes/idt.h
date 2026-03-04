#ifndef _KERNEL_IDT_H_
#define _KERNEL_IDT_H_

#include <stdint.h>
#include "kernel.h"
#include "config.h"

#define IDT_ENTRY_MASK_GATE_TYPE 0b00001111
#define IDT_ENTRY_MASK_DPL 0b01100000
#define IDT_ENTRY_MASK_PRESENT 0b10000000
#define IDT_ENTRY_MASK_LOW_OFFSET 0x0000FFFF
#define IDT_ENTRY_MASK_HIGH_OFFSET 0xFFFF0000

#define INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE 0x8E
#define INTERRUPT_32_BIT_INTERRUPT_GATE_R1_SPACE 0xAE
#define INTERRUPT_32_BIT_INTERRUPT_GATE_R2_SPACE 0xCE
#define INTERRUPT_32_BIT_INTERRUPT_GATE_USER_SPACE 0xEE

#define INTERRUPT_32_BIT_TRAP_GATE_USER_SPACE 0xEE

struct IDT_Descriptor  {

    uint16_t low_offset; // 0 - 15
    uint16_t selector; // Segment selector
    uint8_t reserved; // 8 - 0 bytes
    uint8_t type_attr; // gate type (4), 0, dpl (2), P 
    uint16_t high_offset; // 16-31

} __attribute__((packed));

struct IDTR_Descriptor {

    uint16_t size; // size of descriptor table
    uint32_t offset; // address of the table

} __attribute__((packed)) ;

void idt_init(void);
void idt_set(int interrupt_number, void* addr, uint16_t selector, uint8_t type_attr);
void int_zero(void);
void int_gp_fault(uintptr_t address, uint32_t err_code);
void unhandled_interrupts_handler_basic(void);

/* Loaders and Handlers from idt.asm */
extern void idt_load(struct IDTR_Descriptor* ptr);

/* Interrupt handlers */
extern void idt_int_zero_handler(void);
extern void general_protection_fault(void);

/* Unhandled interrupts */
extern void unhandled_interrupts(void);

#endif