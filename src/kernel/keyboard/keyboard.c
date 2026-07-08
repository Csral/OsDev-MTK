#include "keyboard/keyboard.h"
#include "status.h"
#include "task/task.h"
#include "task/process.h"
#include "kernel.h"

#include "keyboard/classic.h"

static struct keyboard* keyboard_list_head = 0;
static struct keyboard* keyboard_list_tail = 0;

void keyboard_init(void) {

    keyboard_insert(classic_init());

}

int keyboard_insert(struct keyboard* keyboard) {
    
    int res = 0;
    if (!keyboard->init) {
        res = -EINVARG;
        print("Keyboard doesn't have init function.");
        goto out;
    }

    if (keyboard_list_tail) {
        keyboard_list_tail->next = keyboard;
        keyboard_list_tail = keyboard;
    } else {
        keyboard_list_head = keyboard;
        keyboard_list_tail = keyboard;
    }
    
    res = keyboard->init();

    out:
    return res;

}

__attribute__((__always_inline__)) inline static int keyboard_get_tail_index(struct process* process) {
    return process->keyboard.tail % sizeof(process->keyboard.buffer);
}

void keyboard_backspace(struct process* process) {
    process->keyboard.tail--;
    int real_idx = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_idx] = 0x00;
}

void keyboard_push(char c) {

    struct process* process = get_current_process();
    if (!process) return;

    int real_idx = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_idx] = c;
    process->keyboard.tail++;

}

char keyboard_pop() {
    
    if (!task_current()) return 0;

    struct process* process = task_current()->process;
    int real_idx = process->keyboard.head % sizeof(process->keyboard.buffer);
    char c = process->keyboard.buffer[real_idx];

    if (c == 0x00) return 0;

    process->keyboard.buffer[real_idx] = 0x00;
    process->keyboard.head++;
    return c;

}