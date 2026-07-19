#include "BasicOS.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char** argv) {
 
    print("\nHello from cblank. \n");
    print("Printing arguments:\n");

    for (int i = 0; i < argc; i++) {
        printf("%i: %s\n", i, argv[i]);
    }

    while(1) {
    };

    return 0;
}