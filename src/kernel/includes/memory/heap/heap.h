#ifndef _KERNEL_HEAP_H_
#define _KERNEL_HEAP_H_

#include "../../config.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_MASKS_ALLOCATION_EXTENDS_NEXT_BLOCK 0x80
#define HEAP_MASKS_ALLOCATION_IS_FIRST_ENTRY 0x40
#define HEAP_MASKS_IS_BLOCK_ALLOCATED 0x0F

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

typedef unsigned char heap_entry_t;

struct heap_table {
    heap_entry_t* entries;
    size_t total_entries;
};

struct heap {
    struct heap_table* table;
    void* base_addr;
};

void heap_mark_blocks_as_taken(struct heap* heap, int start_block, int total_blocks);
int heap_get_start_block(struct heap* heap, size_t total_blocks);

int make_heap(struct heap* heap, void* ptr, void* end, struct heap_table* table);
void* heap_alloc_blocks(struct heap* heap, size_t total_blocks);
void* heap_alloc(struct heap* heap, size_t size);
void heap_free(struct heap* heap, void* ptr);

#endif