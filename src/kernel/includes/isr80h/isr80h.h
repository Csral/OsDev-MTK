#ifndef KERNEL_INTERRUPTS_ISR80H_H
#define KERNEL_INTERRUPTS_ISR80H_H

enum SYSCALL_IDS {
    SYS_SUM,
    SYS_PRINT,
    SYS_PRINT_EXT,
    SYS_GET_KET,
    SYS_PUT_CHAR,
};

void isr80h_register_commands(void);

#endif