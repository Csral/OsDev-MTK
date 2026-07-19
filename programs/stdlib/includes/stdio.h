#ifndef BASIC_OS_STDIO
#define BASIC_OS_STDIO

#include <stdbool.h>

int putchar(int ch);
int printf(const char* format, ...);
int terminal_readline(char* out, int max, bool output_while_typing);

#endif