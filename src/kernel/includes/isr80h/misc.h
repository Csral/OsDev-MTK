#ifndef ISR80_MISC_H
#define ISR80_MISC_H

struct interrupt_frame;
void* sys_sum(struct interrupt_frame* frame);

#endif