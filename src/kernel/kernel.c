#include "kernel.h"
#include "includes/idt.h"
#include "includes/io.h"
#include "includes/math/math.h"
#include "includes/memory/memory.h"
#include "includes/memory/heap/kheap.h"
#include "includes/memory/paging/paging.h"
#include "includes/disk.h"
#include "includes/string/string.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "fs/file.h"

unsigned int terminal_x, terminal_y;
unsigned char terminal_fg_color, terminal_bg_color;

extern void problem();

static struct paging_4gb_memory_map* kernel_paging_chunk = 0;

void kernel_main(void) {

    disable_interrupts();
    terminal_init();
    kheap_init();
    fs_init();
    disk_search_and_init();
    idt_init();
    kernel_paging_chunk = _gen_paging_4gb(PAGING_MASKS_IS_WRITABLE | PAGING_MASKS_IS_PRESENT | PAGING_MASKS_ACCESS_ALL);
    paging_switch(kernel_paging_chunk->d_entry);
    enable_paging();
    enable_interrupts();

    print("\nKernel Setup finished.\n");

    int fd = fopen("0:/message.txt", "r");
    (fd == 0) ? print((const char*) 48 + fd) : printint(fd);
    if (fd) {
        struct file_stat s;
        fstat(fd, &s);

        fclose(fd);
        print("\nClosed message.txt\n");
    }

    while(1);

};

__attribute__((noreturn)) void kernel_panic(const char* msg) {

    if (terminal_y >= TEXT_MODE_VGA_HEIGHT) terminal_clear();

    print("\nKernel panic: ");
    print(msg);
    __panic();

};

void print(const char* str) {

    terminal_fg_color = TEXT_MODE_COLORS_WHITE;
    terminal_bg_color = TEXT_MODE_COLORS_BLACK;
    terminal_write(str);

}

void print_hex_byte(const uint8_t byte) {

    const char* hex = "0123456789ABCDEF";

    terminal_puts(hex[(byte >> 4) & 0xF]);  // high nibble
    terminal_puts(hex[byte & 0xF]);         // low nibble
    terminal_puts(' ');

}

void printc(const char* str, const unsigned char color, const unsigned char bg_color) {

    terminal_fg_color = color;
    terminal_bg_color = bg_color;
    terminal_write(str);
}

void printint(int num) {

    if (num < 0) {
        terminal_puts('-');
        num *= -1;
    }

    int32_t bkp = num;
    size_t num_len = 0;
    long int divres = 0;

    while (num) {
        num /= 10;
        num_len++;
    }

    for (size_t i = 0; i < num_len; i++) {
        
        divres = pow(10,num_len-i-1);
        
        terminal_puts(
            48 + bkp / divres
        );
        bkp = bkp % divres;
    }

}

unsigned short VGA_make_char(const char ch, const unsigned char color, const unsigned char bg_color) {
    return (((bg_color << 4) | (color & TEXT_MODE_BIT_MASK_FG_COLOR)) << 8) | ch;
};

unsigned int VGA_get_offset(const unsigned char x, const unsigned char y) {
    return (y * TEXT_MODE_CHARACTERS_PER_LINE) + x;
}

void terminal_init(void) {
    terminal_clear();
    terminal_fg_color = TEXT_MODE_COLORS_WHITE;
    terminal_bg_color = TEXT_MODE_COLORS_BLACK;
}

void terminal_puts(const char ch) {

    // Little endian 
    unsigned short tmp_ch = (((terminal_bg_color << 4) | (terminal_fg_color & TEXT_MODE_BIT_MASK_FG_COLOR)) << 8) | ch;
    *((volatile unsigned short*) TEXT_MODE_COLOR_BASE_ADDR + (terminal_y * TEXT_MODE_CHARACTERS_PER_LINE) + terminal_x) = tmp_ch;
    terminal_x++;

}

void terminal_write(const char* str) {

    unsigned long int ctr = 0;

    while (str[ctr]) {

        //* Handle special characters

        if (terminal_x >= TEXT_MODE_CHARACTERS_PER_LINE) {
            terminal_x = 0;
            terminal_y++;
        }

        if (str[ctr] == '\n') {
            terminal_x = 0;
            terminal_y++;
            ctr++;
            continue;
        } else if (str[ctr] == '\b') {
            
            if (terminal_x > 0) {
                terminal_x--;
                terminal_puts_raw(' ', TEXT_MODE_COLORS_BLACK, TEXT_MODE_COLORS_BLACK, VGA_get_offset(terminal_x, terminal_y));
            }

            ctr++;
            continue;
        } else if (str[ctr] == '\r') {
            terminal_x = 0;
            ctr++;
            continue;
        } else if (str[ctr] == '\t') {
            terminal_x = (terminal_x + 4) & ~3; // align to 4-char space
            ctr++;
            continue;
        }

        terminal_puts(str[ctr]);
        ctr++;

    }

}

void terminal_puts_raw(const char ch, const unsigned char color, const unsigned char bg_color, const unsigned int offset) {

    // Little endian 
    unsigned short tmp_ch = (((bg_color << 4) | (color & TEXT_MODE_BIT_MASK_FG_COLOR)) << 8) | ch;
    *((volatile unsigned short*) TEXT_MODE_COLOR_BASE_ADDR + offset) = tmp_ch;

}

void terminal_clear(void) {

    for (unsigned long int y = 0; y < TEXT_MODE_VGA_HEIGHT; y++)
        for (unsigned long int x = 0; x < TEXT_MODE_VGA_WIDTH; x++)
            terminal_puts_raw(' ', TEXT_MODE_COLORS_BLACK, TEXT_MODE_COLORS_BLACK, (y * TEXT_MODE_CHARACTERS_PER_LINE) + x);

    terminal_x = 0;
    terminal_y = 0;
    
}