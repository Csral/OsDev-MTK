#include "../includes/interrupts.h"
#include "../includes/task/task.h"
#include "config.h"

static ISR80H_COMMAND isr80h_commands[BasicOS_MAX_SYSCALL_HANDLE_COMMANDS];

void int_zero(void) {
    print("Divide by zero error!\n\0");
}

void idt_invalid_opcode_fault_handler(void) {
    kernel_panic("\nUndefined instruction (or reserved op-code) executed.\n");
}

void int_gp_fault(struct interrupt_frame* stack_frame) {
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

void no_interrupt_routine_handler(void) {
    _ACK_IRQ_EOI
}

void isr80h_register_command(int command_id, ISR80H_COMMAND handler) {

    if (command_id < 0 || command_id >= BasicOS_MAX_SYSCALL_HANDLE_COMMANDS) {
        kernel_panic("Syscall handler exceeds bounds!\n");
    }

    if (isr80h_commands[command_id]) {
        kernel_panic("Syscall already registered.\n");
    }

    isr80h_commands[command_id] = handler;

}

void* isr80h_handle_command(int command, struct interrupt_frame* frame) {

    void* res = 0;

    if (command < 0 || command >= BasicOS_MAX_SYSCALL_HANDLE_COMMANDS) {
        return 0;
    }

    ISR80H_COMMAND handler = isr80h_commands[command];
    if (!handler) {
        // invalid command
        // maybe the program is written for later kernels etc? So not an error. We just ignore
        return 0;
    }

    res = handler(frame);
    return res;

}

void* isr80h_handler(int command, struct interrupt_frame* frame) {

    void* res = 0;
    kernel_page();

    // save current task's registers.
    task_current_save_state(frame);
    res = isr80h_handle_command(command, frame);

    task_page();

    out:
    return res;

}