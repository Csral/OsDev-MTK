#ifndef _STRING_H_
#define _STRING_H_

unsigned long strlen(const char* str);
unsigned long strnlen(const char* str, unsigned long max_len);
unsigned char isdigit(const char c);
int atoi(const char* str);
int atoic(const char ch);

#endif