#ifndef _KERNEL_PAGING_H_
#define _KERNEL_PAGING_H_

#include <stddef.h>
#include <stdint.h>
#include "../heap/kheap.h"
#include "../../kernel.h"
#include "../../status.h"

#define PAGING_MASKS_CACHE_DISABLE        0b00010000
#define PAGING_MASKS_CACHE_WRITE_THROUGH  0b00001000
#define PAGING_MASKS_ACCESS_ALL           0b00000100
#define PAGING_MASKS_IS_WRITABLE          0b00000010
#define PAGING_MASKS_IS_PRESENT           0b00000001

#define PAGING_TOTAL_TABLE_ENTRIES_PER_DIRECTORY    0x400U
#define PAGING_TOTAL_ENTRIES_PER_TABLE              0x400U
#define PAGING_PAGE_SIZE                            0x1000U

struct paging_4gb_memory_map {
    uint32_t* d_entry;
};

extern void enable_paging(void);
struct paging_4gb_memory_map* _gen_paging_4gb(unsigned char flags);
void paging_switch(uint32_t* p_directory);
int paging_set(uint32_t* directory, void* v_addr, uint32_t val);

#endif