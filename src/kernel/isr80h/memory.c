#include "isr80h/memory.h"
#include "task/task.h"
#include "task/process.h"
#include <stddef.h>

void* sys_malloc(struct interrupt_frame* frame) {
    // void* sys_malloc(size_t size);
    struct task* task = task_current();
    size_t size = (size_t) task_get_stack_item(task, 0);
    return process_malloc(task->process, size);
}

void* isr_sys_free(struct interrupt_frame* frame) {
    // void sys_free(void* ptr);
    struct task* task = task_current();
    void* ptr = task_get_stack_item(task, 0);
    process_free(task->process, ptr);
    return 0;
}