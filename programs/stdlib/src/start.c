#include "BasicOS.h"

extern int main(int argc, char** argv);

void _cstart(void) {
    
    struct process_arguments arguments;
    basic_os_kernel_get_process_arguments(&arguments);

    int res = main(arguments.argc, arguments.argv);

    if (res == 0);
    return;

}