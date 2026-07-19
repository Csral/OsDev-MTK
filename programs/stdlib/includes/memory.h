#ifndef BASIC_OS_MEMORY_H
#define BASIC_OS_MEMORY_H

#include <stddef.h>

void* memset(void* ptr, int c, size_t size);
int memcmp(void* s1, void* s2, size_t size);
void* memcpy(void* dst, void* src, size_t size);

#endif