#ifndef _COS_KHEAP_H_
#define _COS_KHEAP_H_

#include "heap.h"
#include "../../kernel.h"
#include "../memory.h"

void kheap_init(void);
void* kmalloc(size_t size);
void* kzalloc(size_t size);
void kfree(void* ptr);

#endif