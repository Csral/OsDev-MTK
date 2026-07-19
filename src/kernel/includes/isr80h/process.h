#ifndef ISR80_PROCESS_H
#define ISR80_PROCESS_H

struct interrupt_frame;
void* sys_process_load_start(struct interrupt_frame* frame);

#endif