#ifndef _KERNEL_H_
#define _KERNEL_H_

#define TEXT_MODE_COLOR_BASE_ADDR 0xB8000

#define TEXT_MODE_COLORS_BLACK 0
#define TEXT_MODE_COLORS_BLUE 1
#define TEXT_MODE_COLORS_GREEN 2
#define TEXT_MODE_COLORS_CYAN 3
#define TEXT_MODE_COLORS_RED 4
#define TEXT_MODE_COLORS_MAGENTA 5
#define TEXT_MODE_COLORS_BROWN 6
#define TEXT_MODE_COLORS_LIGHT_GRAY 7
#define TEXT_MODE_COLORS_DARK_GRAY 8
#define TEXT_MODE_COLORS_LIGHT_BLUE 9
#define TEXT_MODE_COLORS_LIGHT_GREEN 10
#define TEXT_MODE_COLORS_LIGHT_CYAN 11
#define TEXT_MODE_COLORS_LIGHT_RED 12
#define TEXT_MODE_COLORS_LIGHT_MAGENTA 13
#define TEXT_MODE_COLORS_YELLOW 14
#define TEXT_MODE_COLORS_WHITE 15

#define TEXT_MODE_BIT_MASK_BG_COLOR 0b11110000
#define TEXT_MODE_BIT_MASK_FG_COLOR 0b00001111

#define TEXT_MODE_CHARACTERS_PER_LINE 80

void kernel_main(void);
unsigned long int print(unsigned char* str, unsigned char color, unsigned char bg_color, unsigned int offset);
unsigned long int print_pos(unsigned char* str, unsigned char color, unsigned char bg_color, unsigned char x, unsigned char y);
void print_char(unsigned char ch, unsigned char color, unsigned char bg_color, unsigned int offset);
void print_char_pos(unsigned char ch, unsigned char color, unsigned char bg_color, unsigned char x, unsigned char y);

#endif