#include "io.h"
#include "interrupts.h"
#include "task/task.h"
#include "kernel.h"
#include "status.h"

#include "memory/heap/kheap.h"

void* isr_sys_print(struct interrupt_frame* frame) {
    
    void* user_space_msg_buf = task_get_stack_item(task_current(), 0);
    char buf[1024];
    copy_string_from_task(task_current(), user_space_msg_buf, buf, sizeof(buf));
    print(buf);

    return 0;
}

void* isr_sys_print_ext(struct interrupt_frame* frame) {

    int size_requested = (int) task_get_stack_item(task_current(), 0);
    void* msg_buf_addr = task_get_stack_item(task_current(), 1);
    
    // negative and zero values not allowed 
    if (size_requested <= 0) return (void*) -1;
    else if (size_requested < 1025) return isr_sys_print(frame);

    if (size_requested > 0x1000) {
        return (void*) -2;
        // prefer sending many 0x1000 size bytes instead.
    }
    
    int res = 0;
    char* buf = (char*) kzalloc(size_requested);
    
    if (!buf) {
        res = -ENOMEM;
        goto out;
    }

    unsigned long ctr = 0;

    while (size_requested) {
        
        if (size_requested < 1024) {

            copy_string_from_task(task_current(), msg_buf_addr + ctr, buf + ctr, size_requested);
            size_requested = 0;

        } else {
            copy_string_from_task(task_current(), msg_buf_addr + ctr, buf + ctr, 1024);
            size_requested -= 1024;
        }

        // on the last loop, even though ctr overflows by X bytes, the loop itself would no longer run. So it's all good.
        ctr += 1024;

    }

    print(buf);
    kfree(buf);

    out:
    return (void*) res;

}