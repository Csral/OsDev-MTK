#include "BasicOS.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
 
    print("Hello from cblank. \n");
    // char x;

    void* ptr = malloc(100);
    if (ptr) {
        print("Allocation succeeded.\n");
        free(ptr);
    }

    print("Ptr fred\n");
    printf("Testing a numerical %i %s\n", 192, "input");

    char buf[1024];
    int r_ctr = terminal_readline(buf, 1024, true);
    
    printf("\nRead %i bytes from terminal.\nOutput: %s\n", r_ctr, buf);

    while(1) {
    };

    return 0;
}