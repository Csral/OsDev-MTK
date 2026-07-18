#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>

#define PF_X 0x01
#define PF_W 0x02
#define PF_R 0x04

#define PT_NULL 0x00
#define PT_LOAD 0x01
#define PT_DYNAMIC 0x02
#define PT_INTERP 0x03
#define PT_NOTE 0x04
#define PT_SHLIB 0x05
#define PT_PHDR 0x06

#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

#define SHT_NULL 0x00
#define SHT_PROGBITS 0x01
#define SHT_SYMTAB 0x02
#define SHT_STRTAB 0x03
#define SHT_RELA 0x04
#define SHT_HASH 0x05
#define SHT_DYNAMIC 0x06
#define SHT_NOTE 0x07
#define SHT_NOBITS 0x08
#define SHT_REL 0x09
#define SHT_SHLIB 0x0A
#define SHT_DYNSYM 0x0B
#define SHT_LOPROC 0x0C
#define SHT_HIPROC 0x0D
#define SHT_LOUSER 0x0E
#define SHT_HIUSER 0x0F

#define ET_NONE 0x00
#define ET_REL 0x01
#define ET_EXEC 0x02
#define ET_DYN 0x03
#define ET_CORE 0x04

#define EI_NIDENT 0x10
#define EI_CLASS 0x04
#define EI_DATA 0x05

#define ELFCLASSNONE 0x00
#define ELFCLASS32 0x01
#define ELFCLASS64 0x02

#define ELFDATANONE 0x00
#define ELFDATA2LSB 0x01
#define ELFDATA2MSB 0x02

#define SHN_UNDEF 0x00

typedef uint16_t elf32_half;
typedef uint32_t elf32_word;
typedef int32_t elf32_sword;
typedef uint32_t elf32_addr;
typedef int32_t elf32_off;

struct elf32_phdr {

    elf32_word p_type;
    elf32_off p_offset;
    elf32_addr p_vaddr;
    elf32_addr p_paddr;
    elf32_word p_filesz;
    elf32_word p_memsz;
    elf32_word p_flags;
    elf32_word p_align;

} __attribute__((packed));

struct elf32_shdr {

    elf32_word sh_name;
    elf32_word sh_type;
    elf32_word sh_flags;
    elf32_addr sh_addr;
    elf32_off sh_offset;
    elf32_word sh_size;
    elf32_word sh_link;
    elf32_word sh_info;
    elf32_word sh_addralign;
    elf32_word sh_entsize;

} __attribute__((packed));

struct elf_header {

    unsigned char e_ident[EI_NIDENT];
    elf32_half e_type;
    elf32_half e_machine;
    elf32_word e_version;
    elf32_addr e_entry;
    elf32_off e_phoff;
    elf32_off e_shoff;
    elf32_word e_flags;
    elf32_half e_ehsize;
    elf32_half e_phentsize;
    elf32_half e_phnum;
    elf32_half e_shentsize;
    elf32_half e_shnum;
    elf32_half e_shstrndx;

} __attribute__((packed));

struct elf32_dyn {
    elf32_sword d_tag;
    union {
        elf32_word d_val;
        elf32_addr d_ptr;
    } d_un;
} __attribute__((packed));

struct elf32_sym {

    elf32_word st_name;
    elf32_addr st_value;
    elf32_word st_size;
    unsigned char st_info;
    unsigned char st_other;
    elf32_half st_shndx;

} __attribute__((packed));

void* elf_get_entry_ptr(struct elf_header* elf_header);
uint32_t elf_get_entry(struct elf_header* elf_header);

#endif