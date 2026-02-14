#include "includes/kernel.h"

void kernel_main(void) {

    print_pos( (unsigned char*) "Hello World! Made by Csral :D - Ignore this: \0", TEXT_MODE_COLORS_WHITE, TEXT_MODE_COLORS_BLACK, 0, 10);

};

void print_char(unsigned char ch, unsigned char color, unsigned char bg_color, unsigned int offset) {

    unsigned short tmp_ch = ((bg_color << 4) | (color & TEXT_MODE_BIT_MASK_FG_COLOR));
    tmp_ch = ch | (tmp_ch << 8);
    *((volatile unsigned short*) TEXT_MODE_COLOR_BASE_ADDR + offset) = tmp_ch;

};

void print_char_pos(unsigned char ch, unsigned char color, unsigned char bg_color, unsigned char x, unsigned char y) {

    unsigned short tmp_ch = ((bg_color << 4) | (color & TEXT_MODE_BIT_MASK_FG_COLOR));
    tmp_ch = ch | (tmp_ch << 8);
    *((volatile unsigned short*) TEXT_MODE_COLOR_BASE_ADDR + (y * TEXT_MODE_CHARACTERS_PER_LINE) + x) = tmp_ch;

}

unsigned long int print(unsigned char* str, unsigned char color, unsigned char bg_color, unsigned int offset) {

    unsigned long int ctr = 0;

    while (str[ctr] != '\0') {

        print_char(str[ctr], color, bg_color, offset + ctr);
        ctr++;

    }

    return ctr;

};

unsigned long int print_pos(unsigned char* str, unsigned char color, unsigned char bg_color, unsigned char x, unsigned char y) {

    unsigned long int ctr = 0;
    unsigned int offset = (y * TEXT_MODE_CHARACTERS_PER_LINE) + x;

    while (str[ctr] != '\0') {

        print_char(str[ctr], color, bg_color, offset + ctr);
        ctr++;

    }

    return ctr;

}