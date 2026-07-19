#ifndef BASIC_OS_STDLIB_H
#define BASIC_OS_STDLIB_H

#include <stddef.h>
void* malloc(size_t size);
void free(void* ptr);

char* itoa(int num);

#endif