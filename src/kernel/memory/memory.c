#include "../includes/memory/memory.h"

void* memset(void* ptr, int c, size_t size) {
    
    char* c_ptr = (char*) ptr;

    for (size_t i = 0; i < size; i++) {
        c_ptr[i] = c;
    }
    
    return ptr;
}

void* memcpy(void* dst, void* src, size_t size) {

    for (size_t i = 0; i < size; i++)
        *((unsigned char*) dst + i) = *((unsigned char*) src + i);

    return dst;

}