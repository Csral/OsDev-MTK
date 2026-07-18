#include "../includes/interrupts.h"
#include "../includes/task/task.h"
#include "config.h"

static ISR80H_COMMAND isr80h_commands[BasicOS_MAX_SYSCALL_HANDLE_COMMANDS];

void int_zero(struct interrupt_frame* stack_frame) {
    print("Divide by zero error!\n\0");
}

void invalid_opcode_fault_handler(struct interrupt_frame* stack_frame) {
    kernel_panic("\nUndefined instruction (or reserved op-code) executed.\n");
}

void int_gp_fault(struct interrupt_frame* stack_frame) {
    terminal_clear();
    print("General Protection Fault (#GP).");
    print("\nRaised at address: ");
    printint(stack_frame->eip);
    print("\nError Code: ");
    printint(stack_frame->err_code);
    
    task_page();
    unsigned long page_entry = paging_get(task_current()->page_directory->d_entry, (void*) stack_frame->eip);

    if (page_entry & 0x1) {
        print("\n INS:");
        for (uint8_t i = 0; i < 8; i++) {
            print_hex_byte(*((uint8_t*) stack_frame->eip + i));
        }
    }
    kernel_page();

    print("\nProcessor CS value: ");
    printint(stack_frame->cs);

    print("\nProcessor eflags: ");
    printint(stack_frame->eflags);

    kernel_panic("CPU Halted.");

}

void page_fault_h(struct interrupt_frame* stack_frame) {

    terminal_clear();
    print("Page Fault (#PF).");

    print("\nFaulting address (CR2): ");
    uint32_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    printint(cr2);

    print("\nRaised at address: ");
    printint(stack_frame->eip);

    print("\nError Code: ");
    printint(stack_frame->err_code);

    // Error code bit breakdown (bit 0=Present, 1=Write, 2=User, 3=Reserved, 4=Instr fetch)
    print("\n  Is Present: ");
    print(stack_frame->err_code & 0x1 ? "Y (protection violation)" : "N (not present)");
    print("\n  Write Attempted: ");
    print(stack_frame->err_code & 0x2 ? "Y" : "N (read)");
    print("\n  By Privilege: ");
    print(stack_frame->err_code & 0x4 ? "User" : "Kernel");

    task_page();
    unsigned long page_entry = paging_get(task_current()->page_directory->d_entry, (void*) stack_frame->eip);

    if (page_entry & 0x1) {
        // page is present
        print("\n INS:");
        for (uint8_t i = 0; i < 8; i++) {
            print_hex_byte(*((uint8_t*) stack_frame->eip + i));
        }
    }
    kernel_page();

    print("\nProcessor CS value: ");
    printint(stack_frame->cs);

    print("\nProcessor eflags: ");
    printint(stack_frame->eflags);

    kernel_panic("CPU Halted.");

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