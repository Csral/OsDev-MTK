#include "../includes/memory/memory.h"

void* memset(void* ptr, int c, size_t size) {
    
    char* c_ptr = (char*) ptr;

    for (size_t i = 0; i < size; i++) {
        c_ptr[i] = c;
    }
    
    return ptr;
}

int memcmp(void* s1, void* s2, size_t size) {

    unsigned char* c1 = (unsigned char*) s1;
    unsigned char* c2 = (unsigned char*) s2;

    while (size-- > 0)
        if (*c1++ != *c2++)
            return c1[-1] < c2[-1] ? -1 : 1;

    return 0;

}

void* memcpy(void* dst, void* src, size_t size) {

    for (size_t i = 0; i < size; i++)
        *((unsigned char*) dst + i) = *((unsigned char*) src + i);

    return dst;

}