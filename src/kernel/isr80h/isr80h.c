#include "interrupts.h"
#include "isr80h/isr80h.h"
#include "isr80h/misc.h"
#include "isr80h/io.h"

void isr80h_register_commands(void) {

    isr80h_register_command(SYS_SUM, sys_sum);
    isr80h_register_command(SYS_PRINT, isr_sys_print);
    isr80h_register_command(SYS_PRINT_EXT, isr_sys_print_ext);

}