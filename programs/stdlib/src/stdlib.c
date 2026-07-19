#include "stdlib.h"
#include "BasicOS.h"

// User programs do not need to concern themselves with this routine
// Implemented in kernel.asm
extern void* basic_os_kernel_malloc(size_t size);
// User programs do not need to concern themselves with this routine
// Implemented in kernel.asm
extern void basic_os_kernel_free(void* ptr);

// Allocates memory of size 'size' on the heap.
void* malloc(size_t size) {
    return basic_os_kernel_malloc(size);
}

void free(void* ptr) {
    basic_os_kernel_free(ptr);
    return;
}

// I want to remove the 'static' in this function. Maybe will do later.
char* itoa(int num) {

    static char text[12];
    int loc = 11;
    text[loc] = 0x00;
    unsigned char neg = 0x01;

    if (num >= 0) {
        neg = 0x00;
        num = -num;
    }

    while (num) {

        text[--loc] = '0' - (num % 10);
        num /= 10;

    }

    if (loc == 11)
        text[--loc] = '0';

    if (neg)
        text[--loc] = '-';

    return &text[loc];

}