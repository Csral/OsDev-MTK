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

    // char* ptr = (char*) 0x00;
    // *ptr = 'A';
    // print("Written to random address!");

    return 0;
}