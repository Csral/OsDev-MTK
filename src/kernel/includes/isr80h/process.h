#ifndef ISR80_PROCESS_H
#define ISR80_PROCESS_H

struct interrupt_frame;
void* sys_process_load_start(struct interrupt_frame* frame);
void* sys_invoke_sys_command(struct interrupt_frame* frame);
void* sys_get_process_arguments(struct interrupt_frame* frame);

#endif