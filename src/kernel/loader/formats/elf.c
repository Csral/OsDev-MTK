#include "loader/formats/elf.h"

inline void* elf_get_entry_ptr(struct elf_header* elf_header) {
    return (void*) elf_header->e_entry;
}

inline uint32_t elf_get_entry(struct elf_header* elf_header) {
    return elf_header->e_entry;
}