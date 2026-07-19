#include "BasicOS.h"

// User programs do not need to concern themselves with this routine
// Implemented in kernel.as
extern char basic_os_kernel_getkey(void);

char getkey(void) {
    
    char val = 0x00U;
    do {
        val = basic_os_kernel_getkey();
    } while (val == 0x00);
    return val;

}