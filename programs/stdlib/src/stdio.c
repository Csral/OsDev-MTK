#include "stdio.h"
#include "stdlib.h"
#include "BasicOS.h"
#include <stdarg.h>

// User programs do not need to concern themselves with this routine
// Implemented in kernel.as
extern void basic_os_kernel_putchar(char ch);

// Writes a single character to stdout. Currently writes onto terminal screen.
int putchar(int ch) {
    basic_os_kernel_putchar((char) ch);
    return ch;
}

int printf(const char* format, ...) {

    va_list args;
    const char* p;
    char* sval;
    int ival;

    va_start(args, format);

    for (p = format; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }

        switch (*(++p)) {
            case 'i': // %i

                ival = va_arg(args, int);
                print(itoa(ival));
                break;

            case 's': // %s
                sval = va_arg(args, char*);
                print(sval);
                break;
            
            default:
                putchar(*p);
                break;

        }

    }

    va_end(args);
    // We can keep a running count to return number of characters printed.
    return 0;

}