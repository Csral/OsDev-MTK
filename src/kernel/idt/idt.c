#include "../includes/idt.h"
#include "../includes/memory/memory.h"

struct IDT_Descriptor idt_descriptors[IDT_MAX_ENTRIES]; //* The literal IDT table
struct IDTR_Descriptor idtr_descriptor;

void idt_init(void) {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));

    idtr_descriptor.size = sizeof(idt_descriptors) - 1;
    idtr_descriptor.offset = (uint32_t) idt_descriptors;

    idt_set(0, int_zero);

    // load the idt
    idt_load(&idtr_descriptor);

}

void idt_set(int interrupt_number, void* addr) {

    struct IDT_Descriptor* descriptor = &idt_descriptors[interrupt_number];
    descriptor->low_offset = (uint32_t) addr & IDT_ENTRY_MASK_LOW_OFFSET;
    descriptor->selector = KERNEL_CODE_SELECTOR;
    descriptor->reserved = 0x00;
    descriptor->type_attr = INTERRUPT_32_BIT_INTERRUPT_GATE_USER_SPACE;
    descriptor->high_offset = (uint32_t) addr >> 16;

}

void int_zero(void) {
    print( (unsigned char*) "Divide by zero error!\n\0");
}