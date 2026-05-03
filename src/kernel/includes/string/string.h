#ifndef _STRING_H_
#define _STRING_H_

#define _CAST_CHAR_TOLOWER(X) ((char) tolower(X))

unsigned long strlen(const char* str);
unsigned long strnlen(const char* str, unsigned long max_len);
int strncmp(const char* str1, const char* str2, unsigned long size);
unsigned long strnlen_terminator(const char* str, unsigned long max, const char terminator);
int istrncmp(const char* str1, const char* str2, unsigned long max);
unsigned char tolower(unsigned char ch);
char* strcpy(char* dst, const char* src);
unsigned char isdigit(const char c);
int atoi(const char* str);
int atoic(const char ch);

#endif