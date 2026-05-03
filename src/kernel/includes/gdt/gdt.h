#ifndef _KERNEL_GDT_H_
#define _KERNEL_GDT_H_

#include <stdint.h>
#include <stddef.h>

struct gdt_entry {

    uint16_t    segment;
    uint16_t    base_first; // 0 - 15 bits.
    uint8_t     base_16_23_bits;
    uint8_t     access_byte;
    uint8_t     high_flags;
    uint8_t     base_24_31_bits;

};

struct gdt_entry_structured {
    uint32_t base;
    uint32_t limit;
    uint8_t access_byte;
};

typedef struct gdt_entry gdt_entry_t;
typedef struct gdt_entry_structured gdt_entry_st; // use this for ease of handling.

extern void gdt_load(gdt_entry_t* gdt, int size);
void unpack_structured_gdt(gdt_entry_st* s_gdt, gdt_entry_t* gdt, size_t total_entries);

#endif