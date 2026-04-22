#include "../includes/string/string.h"

unsigned long strlen(const char* str) {
    unsigned long ctr = 0;

    while (*str != '\0') {
        ctr++;
        str++;
    }

    return ctr;
}

unsigned long strnlen(const char* str, unsigned long max_len) {
    unsigned long ctr = 0;

    for (; ctr < max_len; ctr++)
        if (str[ctr] == '\0')
            break;

    return ctr;
}

char* strcpy(char* dst, const char* src) {
    char* res = dst;
    while (*src != 0)
        *dst++ = *src++;
    *dst = 0x00; // append the null terminator.
    return res;
}

unsigned char isdigit(const char c) {
    return (c > 47) && (c < 58);
};

int atoi(const char* str) {

    unsigned char is_neg = (str[0] == '-');
    int result = 0;

    if (is_neg) str++;

    //* Must expect string to be null terminated.
    //* However, any character not in range of '0' to '9' will
    //* terminate this loop.
    while ((*str != '\0') || ((unsigned char) *str) < 48 || ((unsigned char) *str) > 57 ) {
        result += (result * 10) + (( (unsigned char) *str) - 48);
        str++;
    }

    if (is_neg) result *= -1;
    return result;

}

inline __attribute__((__always_inline__)) int atoic(const char ch) {
    // A single digit cannot be negative!
    return (ch - 48);
}