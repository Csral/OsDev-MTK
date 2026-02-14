#include "includes/kernel.h"

void kernel_main(void) {

    terminal_init();
    printc( (unsigned char*) "Hello World! Made by Csral :D - Ignore this: \0", TEXT_MODE_COLORS_BLACK, TEXT_MODE_COLORS_WHITE);

};

void print(unsigned char* str) {

    if (terminal_x >= TEXT_MODE_CHARACTERS_PER_LINE) {
        terminal_y++;
        terminal_x = 0;
    }

    unsigned long int len = terminal_puts(str, terminal_x, terminal_y);
    terminal_x += len;

};

void printc(unsigned char* str, unsigned char color, unsigned char bg_color) {

    if (terminal_x >= TEXT_MODE_CHARACTERS_PER_LINE) {
        terminal_y++;
        terminal_x = 0;
    }

    unsigned long int len = terminal_putsc(str, color, bg_color, terminal_x, terminal_y);
    terminal_x += len;

}

void terminal_init(void) {

    clear_screen();
    terminal_x = 0;
    terminal_y = 0;

}

unsigned short VGA_make_char(unsigned char ch, unsigned char color, unsigned char bg_color) {
    return (((bg_color << 4) | (color & TEXT_MODE_BIT_MASK_FG_COLOR)) << 8) | ch;
};

void clear_screen(void) {

    for (int y = 0; y < TEXT_MODE_VGA_HEIGHT; y++)
        for (int x = 0; x < TEXT_MODE_VGA_WIDTH; x++)
            terminal_puts_char(' ', TEXT_MODE_COLORS_BLACK, TEXT_MODE_COLORS_BLACK, x, y);

};

unsigned long int terminal_puts(const unsigned char* str, unsigned char x, unsigned char y) {

    unsigned long int ctr = 0;
    unsigned int offset = (y * TEXT_MODE_CHARACTERS_PER_LINE) + x;

    while (str[ctr] != '\0') {

        terminal_putso_char(str[ctr], TEXT_MODE_COLORS_WHITE, TEXT_MODE_COLORS_BLACK, offset + ctr);
        ctr++;

    }

    return ctr;

};

unsigned long int terminal_putsc(const unsigned char* str, unsigned char color, unsigned char bg_color, unsigned char x, unsigned char y) {

    unsigned long int ctr = 0;
    unsigned int offset = (y * TEXT_MODE_CHARACTERS_PER_LINE) + x;

    while (str[ctr] != '\0') {

        terminal_putso_char(str[ctr], color, bg_color, offset + ctr);
        ctr++;

    }

    return ctr;

}

unsigned long int terminal_putsco(const unsigned char* str, unsigned char color, unsigned char bg_color, unsigned int offset) {

    unsigned long int ctr = 0;

    while (str[ctr] != '\0') {

        terminal_putso_char(str[ctr], color, bg_color, offset + ctr);
        ctr++;

    }

    return ctr;
    
}

void terminal_puts_char(unsigned char ch, unsigned char color, unsigned char bg_color, unsigned char x, unsigned char y) {

    unsigned short tmp_ch = ((bg_color << 4) | (color & TEXT_MODE_BIT_MASK_FG_COLOR));
    tmp_ch = ch | (tmp_ch << 8);
    *((volatile unsigned short*) TEXT_MODE_COLOR_BASE_ADDR + (y * TEXT_MODE_CHARACTERS_PER_LINE) + x) = tmp_ch;

}

void terminal_putso_char(unsigned char ch, unsigned char color, unsigned char bg_color, unsigned int offset) {

    unsigned short tmp_ch = ((bg_color << 4) | (color & TEXT_MODE_BIT_MASK_FG_COLOR));
    tmp_ch = ch | (tmp_ch << 8);
    *((volatile unsigned short*) TEXT_MODE_COLOR_BASE_ADDR + offset) = tmp_ch;

}