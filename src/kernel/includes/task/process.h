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
};

int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load(const char* filename, struct process** process);

#endif