#include "../includes/idt.h"
#include "../includes/memory/memory.h"
#include "../includes/io.h"
#include "task/task.h"
#include "config.h"
#include "status.h"

struct IDT_Descriptor idt_descriptors[IDT_MAX_ENTRIES]; //* The literal IDT table
struct IDTR_Descriptor idtr_descriptor;

extern void* interrupt_pointer_table[IDT_MAX_ENTRIES];

static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[IDT_MAX_ENTRIES];

void interrupt_handler(int interrupt, struct interrupt_frame* frame) {

    kernel_page();
    if (interrupt_callbacks[interrupt] != 0) {
        task_current_save_state(frame);
        interrupt_callbacks[interrupt](frame);
    }

    task_page();
    // if (interrupt == 13) int_gp_fault(frame);
    
    // Specifically sending ACK if needed to preserve CPU cycles and maintain correctness
    if (interrupt >= 0x20 && interrupt < 0x30) {
        // PIC needs ACK
        _ACK_IRQ_EOI
        // Master and slave scenario not handled well. We can come back to it later if needed.
    }
    
}

int idt_register_interrupt_callback(int interrupt_id, INTERRUPT_CALLBACK_FUNCTION callback) {

    if (interrupt_id < 0 || interrupt_id >= IDT_MAX_ENTRIES) {
        return -EINVARG;
    }

    interrupt_callbacks[interrupt_id] = callback;
    return NE;

}

void idt_init(void) {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));

    idtr_descriptor.size = sizeof(idt_descriptors) - 1;
    idtr_descriptor.offset = (unsigned long) idt_descriptors;

    // load the idt
    idt_load(&idtr_descriptor);

    /* Initialize all interrupt handlers as unhandled - THEY HALT THE CPU. */

    for (int i = 0; i < IDT_MAX_ENTRIES; i++)
        idt_set(i, interrupt_pointer_table[i], KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);

    // for (int i = 0; i < IDT_CPU_EXCEPTION_ENTRIES; i++)
    //     idt_set(i, interrupt_pointer_table[i], KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);

    // for (int i = IDT_IRQ_STARTING_INTERRUPT_NUMBER; i < IDT_IRQ_END_INTERRUPT_NUMBER; i++)
    //     idt_set(i, interrupt_pointer_table[i], KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);

    // for (int i = IDT_FREE_STARTING_INTERRUPT_NUMBER; i < IDT_MAX_ENTRIES; i++)
    //     idt_set(i, interrupt_pointer_table[i], KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);

    /* Set IDT table with handled interrupts */
    idt_set(0, &idt_int_zero_handler, KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_USER_SPACE);
    // idt_set(13, &general_protection_fault, KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);
    // idt_set(32, &idt_20_h, KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);
    // idt_set(33, &int_21_h, KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);
    idt_set(0x80, &isr80h_wrapper, KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_USER_SPACE);

}

void idt_set(int interrupt_number, void* addr, uint16_t selector, uint8_t type_attr) {

    struct IDT_Descriptor* descriptor = &idt_descriptors[interrupt_number];
    descriptor->low_offset = (unsigned long) addr & IDT_ENTRY_MASK_LOW_OFFSET;
    descriptor->selector = selector;
    descriptor->reserved = 0x00;
    descriptor->type_attr = type_attr;
    descriptor->high_offset = (unsigned long) addr >> 16;

}