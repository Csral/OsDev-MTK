#include "interrupts.h"
#include "task/task.h"
#include "kernel.h"
void timer_interrupt(struct interrupt_frame* frame) {

    // The process execution is fast enough (or maybe default timer is slow enough)
    // that in most executions, the timer interrupt gets triggered only once at the start.
    // However, sometimes it gets triggered twice.

    _ACK_IRQ_EOI
    // switch to next task immediately.
    task_next();
    // target remote | qemu-system-i386 -S -gdb stdio -hda ./bin/os.bin

};