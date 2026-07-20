#ifndef BASIC_OS_H
#define BASIC_OS_H

#include <stddef.h>

struct command_argument {
    char argument[512];
    struct command_argument* next;
};

struct process_arguments {
    int argc;
    char** argv;
};

extern void exit(void);
extern void print(const char* msg);
char getkey(void);

extern int basic_os_kernel_start_new_process(const char* filename);
extern void basic_os_kernel_get_process_arguments(struct process_arguments* arguments);

int basic_os_system_run(const char* command);

struct command_argument* basic_os_parse_command(const char* command, int max);

#endif