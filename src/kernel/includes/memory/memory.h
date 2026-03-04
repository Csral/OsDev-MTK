#ifndef _KERNEL_MEMORY_H_
#define _KERNEL_MEMORY_H_

#include <stddef.h>
void* memset(void* ptr, int c, size_t size);
void* memcpy(void* dst, void* src, size_t size);

#endif