#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdint.h>
#include "config.h"
#include "task/task.h"

//* Flat Binary
struct process {
    uint32_t id;
    char filename[_FS_MAX_PATH_LEN];
    struct task* task;
    void* allocations[BasicOS_MAX_ALLOCATIONS_ALLOWED_PER_PROCESS];
    void* ptr; //* A pointer to the process memory.

    void* stack; //* PTR to Stack
    uint32_t size; //* size of ptr.

    /* Keyboard */
    struct keyboard_buffer {

        char buffer[BasicOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;

    } keyboard;
};

int process_switch(struct process* process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load_switch(const char* filename, struct process** process);
int process_load(const char* filename, struct process** process);
struct process* get_current_process(void);
struct process* get_process(int pid);

#endif