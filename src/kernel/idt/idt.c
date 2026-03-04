#include "../includes/idt.h"
#include "../includes/memory/memory.h"

struct IDT_Descriptor idt_descriptors[IDT_MAX_ENTRIES]; //* The literal IDT table
struct IDTR_Descriptor idtr_descriptor;

void idt_init(void) {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));

    idtr_descriptor.size = sizeof(idt_descriptors) - 1;
    idtr_descriptor.offset = (uint32_t) idt_descriptors;

    // load the idt
    idt_load(&idtr_descriptor);

    /* Sets IDT table */
    idt_set(0, &idt_int_zero_handler, KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_USER_SPACE);
    idt_set(13, &general_protection_fault, KERNEL_CODE_SELECTOR, INTERRUPT_32_BIT_INTERRUPT_GATE_KERNEL_SPACE);

}

void idt_set(int interrupt_number, void* addr, uint16_t selector, uint8_t type_attr) {

    struct IDT_Descriptor* descriptor = &idt_descriptors[interrupt_number];
    descriptor->low_offset = (uint32_t) addr & IDT_ENTRY_MASK_LOW_OFFSET;
    descriptor->selector = selector;
    descriptor->reserved = 0x00;
    descriptor->type_attr = type_attr;
    descriptor->high_offset = (uint32_t) addr >> 16;

}

void int_zero(void) {
    print("Divide by zero error!\n\0");
}

void int_gp_fault(uintptr_t address, uint32_t err_code) {
    terminal_clear();
    print("General Protection Fault (#GP) raised at address: ");
    printint(address);
    print("\nError Code: ");
    printint(err_code);
    print("\n INS:");

    for (uint8_t i = 0; i < 8; i++) {
        print_hex_byte(*((uint8_t*) address + i));
    }

    print("\nCPU Halted.");
}