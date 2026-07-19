#ifndef BASIC_OS_H
#define BASIC_OS_H

#include <stddef.h>

extern void print(const char* msg);
char getkey(void);

extern int basic_os_kernel_start_new_process(const char* filename);

#endif