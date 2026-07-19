#include "interrupts.h"
#include "isr80h/isr80h.h"
#include "isr80h/misc.h"
#include "isr80h/io.h"
#include "isr80h/memory.h"
#include "isr80h/process.h"

void isr80h_register_commands(void) {

    isr80h_register_command(SYS_SUM, sys_sum);
    isr80h_register_command(SYS_PRINT, isr_sys_print);
    isr80h_register_command(SYS_PRINT_EXT, isr_sys_print_ext);
    isr80h_register_command(SYS_GET_KET, sys_get_key);
    isr80h_register_command(SYS_PUT_CHAR, sys_putchar);
    isr80h_register_command(SYS_MALLOC, sys_malloc);
    isr80h_register_command(SYS_FREE, isr_sys_free);
    isr80h_register_command(SYS_PROCESS_LOAD_START, sys_process_load_start);
    isr80h_register_command(SYS_INVOKE_SYS_COMMAND, sys_invoke_sys_command);
    isr80h_register_command(SYS_GET_PROCESS_ARGUMENTS, sys_get_process_arguments);

}