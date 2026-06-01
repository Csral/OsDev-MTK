#include "../includes/interrupts.h"

void int_zero(void) {
    print("Divide by zero error!\n\0");
}

void idt_invalid_opcode_fault_handler(void) {
    kernel_panic("\nUndefined instruction (or reserved op-code) executed.\n");
}

void int_gp_fault(struct GP_Handler_stack* stack_frame) {
    terminal_clear();
    print("General Protection Fault (#GP).");
    print("\nRaised at address: ");
    printint(stack_frame->eip);
    print("\nError Code: ");
    printint(stack_frame->err_code);
    print("\n INS:");

    for (uint8_t i = 0; i < 8; i++) {
        print_hex_byte(*((uint8_t*) stack_frame->eip + i));
    }

    print("\nProcessor CS value: ");
    printint(stack_frame->cs);

    print("\nProcessor eflags: ");
    printint(stack_frame->eflags);

    kernel_panic("CPU Halted.");

}

void unhandled_interrupts_handler_basic(void) {
    kernel_panic("\nUnhandled interrupt triggered!\tHalting CPU.\n");
}

void timer_handler(void) {
    _ACK_IRQ_EOI
}

void int_21_handler(void) {
    print("\nKeyboard pressed!\n");
    _ACK_IRQ_EOI
}

void no_interrupt_routine_handler(void) {
    _ACK_IRQ_EOI
}