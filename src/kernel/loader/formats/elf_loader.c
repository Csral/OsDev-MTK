#include "loader/formats/elf_loader.h"
#include "fs/file.h"
#include "status.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "string/string.h"
#include "memory/paging/paging.h"
#include "kernel.h"
#include "config.h"

#include "stdbool.h"

const char ELF_SIGNATURE[] = {0x7F, 'E', 'L', 'F'};

static uint8_t elf_valid_signature(void* buffer) {
    return memcmp(buffer, (void*) ELF_SIGNATURE, sizeof(ELF_SIGNATURE)) == 0;
}

static uint8_t elf_valid_class(struct elf_header* header) {
    // 64-bit long mode not supported.
    return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

static uint8_t elf_valid_encoding(struct elf_header* header) {
    // MSB not supported.
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

static uint8_t elf_is_executable(struct elf_header* header) {
    return header->e_type == ET_EXEC && header->e_entry >= BKE_TASK_PROGRAM_VIRTUAL_ADDR;
}

static uint8_t elf_has_program_header(struct elf_header* header) {
    return header->e_phoff != 0;
}

void* elf_memory(struct elf_file* file) {
    return file->elf_memory;
}

struct elf_header* elf_header(struct elf_file* file) {
    return file->elf_memory;
};

struct elf32_shdr* elf_sheader(struct elf_header* header) {
    return (struct elf32_shdr*)( (int)header + header->e_shoff );
}

struct elf32_phdr* elf_pheader(struct elf_header* header) {
    return (elf_has_program_header(header)) ? (struct elf32_phdr*)((int) header + header->e_phoff) : 0;
}

struct elf32_phdr* elf_program_header(struct elf_header* header, int idx) {
    return &elf_pheader(header)[idx];
}

struct elf32_shdr* elf_section(struct elf_header* header, int idx) {
    return &elf_sheader(header)[idx];
}

char* elf_str_table(struct elf_header* header) {
    return (char*) header + elf_section(header, header->e_shstrndx)->sh_offset;
}

void* elf_virtual_base(struct elf_file* file) {
    return file->virtual_base_addr;
}

void* elf_virtual_end(struct elf_file* file) {
    return file->virtual_end_addr;
}

void* elf_physical_base(struct elf_file* file) {
    return file->physical_base_addr;
}

void* elf_physical_end(struct elf_file* file) {
    return file->physical_end_addr;
}

void* elf_phdr_compute_physical_address(struct elf_file* file, struct elf32_phdr* phdr) {
    return elf_memory(file) + phdr->p_offset;
}

int elf_validate_loaded(struct elf_header* header) {
    return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) && elf_has_program_header(header)) ? 
            NE : -EINVFORMAT;
}

int elf_process_phdr_pt_load(struct elf_file* elf_file, struct elf32_phdr* phdr) {

    if (elf_file->virtual_base_addr >= (void*) phdr->p_vaddr || elf_file->virtual_base_addr == 0x00) {
        elf_file->virtual_base_addr = (void*) phdr->p_vaddr;
        elf_file->physical_base_addr = elf_memory(elf_file) + phdr->p_offset;
    }

    unsigned int end_virtual_address = phdr->p_vaddr + phdr->p_filesz;
    if (elf_file->virtual_end_addr <= (void*) (end_virtual_address) || elf_file->virtual_end_addr == 0x00) {
        elf_file->virtual_end_addr = (void*) end_virtual_address;
        elf_file->physical_end_addr = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }

    return 0;

}

int elf_process_pheader(struct elf_file* elf_file, struct elf32_phdr* phdr) {

    int res = 0;

    switch (phdr->p_type) {
        case PT_LOAD:
            res = elf_process_phdr_pt_load(elf_file, phdr);
            break;
        default:
            res = -EIO;
            break;
    }

    return res;

}

int elf_process_pheaders(struct elf_file* elf_file) {

    int res = 0;

    struct elf_header* header = elf_header(elf_file);
    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr* phdr = elf_program_header(header, i);

        res = elf_process_pheader(elf_file, phdr);
        if (res < 0) break;

    }

    out:
    return res;

}

int elf_process_loaded(struct elf_file* elf_file) {

    int res = 0;
    
    struct elf_header* header = elf_header(elf_file);
    res = elf_validate_loaded(header);
    if (res < 0) goto out;

    res = elf_process_pheaders(elf_file);
    if (res < 0) goto out;

    out:
    return res;

}

int elf_load(const char* filename, struct elf_file** file_out) {

    int res = 0;
    struct elf_file* elf_file = kzalloc(sizeof(struct elf_file));

    if (!elf_file) {
        res = -ENOMEM;
        goto out;
    }

    int fd = 0;
    
    res = fopen(filename, "r");
    if (res <= 0) goto out;

    fd = res;

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res < 0) goto out;

    elf_file->elf_memory = kzalloc(stat.file_size);

    if (!elf_file->elf_memory) {
        res = -ENOMEM;
        goto out;
    }

    res = fread(elf_file->elf_memory, stat.file_size, 1, fd);
    if (res < 0)
        goto out;

    res = elf_process_loaded(elf_file);
    if (res < 0) goto out;

    *file_out = elf_file;

    out:
    fclose(fd);
    return res;

}

void elf_close(struct elf_file* elf_file) {
    if (!elf_file)
        return;

    kfree(elf_file->elf_memory);
    kfree(elf_file);
}