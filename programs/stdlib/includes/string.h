#ifndef BASIC_OS_STRING_H
#define BASIC_OS_STRING_H

unsigned long strlen(const char* str);
unsigned long strnlen(const char* str, unsigned long max_len);
int strncmp(const char* str1, const char* str2, unsigned long size);
unsigned char tolower(unsigned char ch);
int istrncmp(const char* str1, const char* str2, unsigned long max);
unsigned long strnlen_terminator(const char* str, unsigned long max, const char terminator);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, unsigned long size);
unsigned char isdigit(const char c);
int atoi(const char* str);
int atoic(const char ch);
char* strtok(char* str, const char* delimiters);

#endif