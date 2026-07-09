#ifndef ISR80_IO_H
#define ISR80_IO_H

struct interrupt_frame;
void* isr_sys_print(struct interrupt_frame* frame);
void* isr_sys_print_ext(struct interrupt_frame* frame);
void* sys_get_key(struct interrupt_frame* frame);
void* sys_putchar(struct interrupt_frame* frame);

#endif