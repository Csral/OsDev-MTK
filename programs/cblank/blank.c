#include "BasicOS.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char** argv) {
 
    print("\nHello from cblank. \n");
    
    char* ptr = malloc(20);
    strcpy(ptr, "Hello World!\n");
    print(ptr);
    free(ptr);
    print("Finale.");

    while(1) {
    };

    return 0;
}