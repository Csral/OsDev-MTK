#include "io.h"
#include "interrupts.h"
#include "task/task.h"
#include "kernel.h"
#include "status.h"
#include "keyboard/keyboard.h"

#include "memory/heap/kheap.h"

void* isr_sys_print(struct interrupt_frame* frame) {
    
    /* int sys_print(char* message) */

    void* user_space_msg_buf = task_get_stack_item(task_current(), 0);
    char buf[1024];
    copy_string_from_task(task_current(), user_space_msg_buf, buf, sizeof(buf));
    print(buf);

    return 0;
}

void* isr_sys_print_ext(struct interrupt_frame* frame) {

    /* int sys_print(int size, char* message) */

    int size_requested = (int) task_get_stack_item(task_current(), 0);
    void* msg_buf_addr = task_get_stack_item(task_current(), 1);
    
    // negative and zero values not allowed 
    if (size_requested <= 0) return (void*) -1;
    else if (size_requested < 1025) {
        char buf[1024];
        copy_string_from_task(task_current(), msg_buf_addr, buf, size_requested);
        print(buf);
        return 0;
    }

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

void* sys_get_key(struct interrupt_frame* frame) {
    /* int sys_get_key(void) */
    char c = keyboard_pop();
    return (void*) (unsigned long) c;
}

void* sys_putchar(struct interrupt_frame* frame) {
    /* int sys_print(char ch) */
    char c = (char)(int) task_get_stack_item(task_current(), 0);
    terminal_write(&c);
    return 0;
}