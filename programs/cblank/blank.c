#include "BasicOS.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
 
    print("Hello from cblank. \n");
    char x;

    void* ptr = malloc(100);
    if (ptr) {
        print("Allocation succeeded.\n");
        free(ptr);
    }

    print("Ptr fred\n");
    printf("Testing a numerical %i %s\n", 192, "input");

    print("You may type:\n");

    while(1) {
        x = getkey();
        if (x != 0x00U) {
            putchar((int) x);
        }
    };

    return 0;
}