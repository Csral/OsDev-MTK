#ifndef ISR80_MEMORY_H
#define ISR80_MEMORY_H

struct interrupt_frame;
void* sys_malloc(struct interrupt_frame* frame);
void* isr_sys_free(struct interrupt_frame* frame);

#endif