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
void paging_free_4gb(struct paging_4gb_memory_map* page);
void paging_switch(struct paging_4gb_memory_map* p_directory);
int paging_set(uint32_t* directory, void* v_addr, uint32_t val);
int paging_map(struct paging_4gb_memory_map* directory, void* v_addr, void* p_addr, int flags);
int paging_map_range(struct paging_4gb_memory_map* directory, void* v_addr, void* p_addr, unsigned long count, int flags);
int paging_map_to(struct paging_4gb_memory_map* directory, void* v_addr, void* p_addr, void* p_addr_end, int flags);
uint32_t paging_get(uint32_t* directory, void* v_addr);

unsigned long paging_align_address(unsigned long addr);
void* paging_align_to_lower_page_addr(void* addr);

#endif