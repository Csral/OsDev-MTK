#include "../../includes/memory/heap/heap.h"
#include "../../includes/kernel.h"
#include "../../includes/status.h"
#include "../../includes/memory/memory.h"

static unsigned char heap_validate_table(void* ptr, void* end, struct heap_table* table) {

    int res = 0;
    size_t t_size = (size_t) (end - ptr);
    size_t total_blocks = t_size / HEAP_BLOCK_SIZE;

    if (table->total_entries != total_blocks) {
        res = -EINVARG;
        goto out;
    }

    out:
    return res;

}

static unsigned char heap_validate_alignment(void* ptr) {
    return ( (unsigned long) ptr % HEAP_BLOCK_SIZE == 0);
}

int make_heap(struct heap* heap, void* ptr, void* end, struct heap_table* table) {

    int res = 0;
    
    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end)) {
        res = -EINVARG;
        goto out;
    }

    memset(ptr, 0, sizeof(struct heap));
    heap->base_addr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr, end, table);

    if (res < 0) goto out;

    size_t t_size = sizeof(heap_entry_t) * table->total_entries;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, t_size);

    out:
    return res;
};

inline static size_t heap_align_size(size_t size) {
    return (size % HEAP_BLOCK_SIZE == 0) ? size : (size + (HEAP_BLOCK_SIZE - (size % HEAP_BLOCK_SIZE)));
}

static inline __attribute__((__always_inline__)) unsigned char heap_get_entry_type(heap_entry_t entry) {
    return 0x0F & entry;
}

int heap_get_start_block(struct heap* heap, size_t total_blocks) {

    struct heap_table* table = heap->table;
    int bc = 0; // current block
    int bs = -1; // block start (first block that's free)

    for (size_t i = 0; i < table->total_entries; i++) {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            bc = 0;
            bs = -1;
            continue;
        }
        
        // if this is the first block
        if (bs == -1) bs = i;
        bc++;

        if (bc == total_blocks) break;
        
    }

    if (bs == -1) return -ENOMEM;
    return bs;

}

inline __attribute__((__always_inline__)) void* heap_block_to_address(struct heap* heap, size_t block) {
    return (void*) ( (unsigned char*) heap->base_addr + (block * HEAP_BLOCK_SIZE));
}

void heap_mark_blocks_as_taken(struct heap* heap, int start_block, int total_blocks) {
    int end_block = (start_block + total_blocks) - 1;

    heap_entry_t entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_MASKS_ALLOCATION_IS_FIRST_ENTRY;
    
    if (total_blocks > 1)
        entry |= HEAP_MASKS_ALLOCATION_EXTENDS_NEXT_BLOCK;

    for (int i = start_block; i <= end_block; i++) {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1) entry |= HEAP_MASKS_ALLOCATION_EXTENDS_NEXT_BLOCK;
    }

}

void heap_mark_blocks_as_free();

inline __attribute__((__always_inline__)) int heap_address_to_block(struct heap* heap, void* addr) {
    return ((unsigned long) (addr - heap->base_addr)) / HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_as_free(struct heap* heap, size_t starting_block) {

    struct heap_table* table = heap->table;
    for (size_t i = starting_block; i < (size_t) table->total_entries; i++) {
        heap_entry_t entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        if (!(entry & HEAP_MASKS_ALLOCATION_EXTENDS_NEXT_BLOCK)) break;
    }

}

void* heap_alloc_blocks(struct heap* heap, size_t total_blocks) {

    void* address = (void*) 0;

    unsigned int start_block = heap_get_start_block(heap, total_blocks); // find enough room for these blocks.
    if (start_block < 0) {
        goto out;
    }

    address = heap_block_to_address(heap, start_block); // find abs. address
    
    //mark these blocks as taken.
    heap_mark_blocks_as_taken(heap, start_block, total_blocks);

    out:
    return address;

}

void* heap_alloc(struct heap* heap, size_t size) {

    size_t aligned_size = heap_align_size(size);
    size_t total_blocks = aligned_size / HEAP_BLOCK_SIZE;
    return heap_alloc_blocks(heap, total_blocks);

}

void heap_free(struct heap* heap, void* ptr) {
    heap_mark_blocks_as_free(heap, heap_address_to_block(heap, ptr));
}