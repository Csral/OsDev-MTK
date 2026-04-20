#include "../../includes/memory/paging/paging.h"

extern void paging_load_directory(uint32_t* p_directory);

static uint32_t* current_directory = 0;
struct paging_4gb_memory_map* _gen_paging_4gb(unsigned char flags) {

    uint32_t* directory = kzalloc(sizeof(unsigned int) * PAGING_TOTAL_TABLE_ENTRIES_PER_DIRECTORY);

    if (!directory)
        kernel_panic("Page directory allocation returned zero as allocated address.\n");

    int offset = 0;
    for (size_t i = 0; i < PAGING_TOTAL_TABLE_ENTRIES_PER_DIRECTORY; i++) {
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

        for (size_t j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++)
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;

        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
        directory[i] = ((unsigned long) entry) | flags | PAGING_MASKS_IS_WRITABLE;
    }

    struct paging_4gb_memory_map* paging_4gb_chunk = kzalloc(sizeof(struct paging_4gb_memory_map));
    
    if (!paging_4gb_chunk)
        kernel_panic("Paging memory map allocation returned zero as allocated address.\n");

    paging_4gb_chunk->d_entry = directory;
    return paging_4gb_chunk;

};

void paging_switch(uint32_t* p_directory) {
    paging_load_directory(p_directory);
    current_directory = p_directory;
}

inline __attribute__((__always_inline__)) uint32_t* paging_4gb_get_directory_ref(struct paging_4gb_memory_map* pconf) {
    return pconf->d_entry;
};

inline __attribute__((__always_inline__)) uint8_t paging_is_aligned(void* addr) {
    return ( (unsigned long) addr % PAGING_PAGE_SIZE == 0);
}

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out) {

    //* Returns the directory and table entry in which this virtual address resides. 
    //* It may point to some place different.

    int res = 0;

    if (!paging_is_aligned(virtual_address)) {
        res = -EINVARG;
        goto out;
    }

    *directory_index_out = ( (unsigned long) virtual_address / (PAGING_TOTAL_TABLE_ENTRIES_PER_DIRECTORY * PAGING_PAGE_SIZE) );
    *table_index_out = ( ((unsigned long) virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)) / PAGING_PAGE_SIZE );

    out:
    return res;


}

int paging_set(uint32_t* directory, void* v_addr, uint32_t val) {

    int res = 0;

    if (!paging_is_aligned(v_addr)) {
        res = -EINVARG;
        goto out;
    }

    uint32_t d_index = 0;
    uint32_t t_index = 0;
    res = paging_get_indexes(v_addr, &d_index, &t_index);
    if (res < 0) goto out;

    uint32_t entry = directory[d_index];
    uint32_t* table = (uint32_t*) (entry & 0xFFFFF000);
    table[t_index] = val;

    out:
    return res;

}