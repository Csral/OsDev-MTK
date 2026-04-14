#include "../../includes/memory/heap/kheap.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init(void) {

    kernel_heap_table.entries = (heap_entry_t*) HEAP_TABLE_ADDRESS;
    kernel_heap_table.total_entries = HEAP_MAX_HEAP_BLOCKS_ENTRIES;

    void* end = (void*) (HEAP_BASE_ADDRESS + HEAP_MAX_HEAP_SIZE);
    int res = make_heap(&kernel_heap, (void*) HEAP_BASE_ADDRESS, end, &kernel_heap_table);

    if (res < 0) {
        kernel_panic("failed to create heap!\n");
    }

    print("\nHeap initiated.");

}

void* kmalloc(size_t size) {
    return heap_alloc(&kernel_heap, size);
}

void kfree(void* ptr) {
    heap_free(&kernel_heap, ptr);
}