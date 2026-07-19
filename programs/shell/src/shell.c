#include "shell.h"
#include "stdio.h"
#include "BasicOS.h"
#include "stdlib.h"

int main(int argc, char** argv) {
    print("BasicOS v1.0.0\n");
    char buf[1024];

    while(1) {

        print("$ ");
        terminal_readline(buf, sizeof(buf), true);
        basic_os_kernel_start_new_process(buf);

        print("\n");

    };

    return 0;
}