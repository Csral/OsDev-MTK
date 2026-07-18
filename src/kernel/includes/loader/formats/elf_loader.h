#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <stdint.h>
#include <stddef.h>
#include "elf.h"
#include "config.h"

struct elf_file {
    char filename[_FS_MAX_PATH_LEN];
    int in_memory_size;
    // phy memory addr where elf file is loaded. A mmap would be better instead of loading whole elf file into memory.
    void* elf_memory;

    void* virtual_base_addr;
    void* virtual_end_addr;
    void* physical_base_addr;
    void* physical_end_addr;
};

int elf_load(const char* filename, struct elf_file** file_out);
void elf_close(struct elf_file* elf_file);

void* elf_virtual_base(struct elf_file* file);
void* elf_virtual_end(struct elf_file* file);
void* elf_physical_base(struct elf_file* file);
void* elf_physical_end(struct elf_file* file);

struct elf_header* elf_header(struct elf_file* file);
struct elf32_shdr* elf_sheader(struct elf_header* header);
void* elf_memory(struct elf_file* file);
struct elf32_phdr* elf_pheader(struct elf_header* header);
struct elf32_phdr* elf_program_header(struct elf_header* header, int idx);
struct elf32_shdr* elf_section(struct elf_header* header, int idx);
void* elf_phdr_compute_physical_address(struct elf_file* file, struct elf32_phdr* phdr);

#endif