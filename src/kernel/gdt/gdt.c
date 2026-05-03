#include "gdt/gdt.h"
#include "kernel.h"

void encode_gdt_entry(uint8_t* target, gdt_entry_st src) {

    // Very specific for 32-bit Systems

    if ((src.limit > 65536) && ((src.limit & 0xFFF) != 0xFFF)) {
        kernel_panic("Encode GDT: Invalid argument.\n");
    }

    target[6] = 0x40;
    if (src.limit > 65536) {
        src.limit = src.limit >> 12;
        target[6] = 0xC0;
    }

    // Encodes the limit
    target[0] = src.limit & 0xFF;
    target[1] = (src.limit >> 8) & 0xFF;
    target[6] |= (src.limit >> 16) & 0x0F;

    // Encode the base
    target[2] = src.base & 0xFF;
    target[3] = (src.base >> 8) & 0xFF;
    target[4] = (src.base >> 16) & 0xFF;
    target[7] = (src.base >> 24) & 0xFF;

    // Set the type
    target[5] = src.access_byte;

}

void unpack_structured_gdt(gdt_entry_st* s_gdt, gdt_entry_t* gdt, size_t total_entries) {

    for (size_t i = 0; i < total_entries; i++) {
        encode_gdt_entry((uint8_t*) &gdt[i], s_gdt[i]);
    }

}