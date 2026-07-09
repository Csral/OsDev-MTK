#include "task/process.h"
#include "memory/memory.h"
#include "status.h"
#include "task/task.h"
#include "memory/heap/kheap.h"
#include "fs/file.h"
#include "string/string.h"
#include "kernel.h"
#include "memory/paging/paging.h"

struct process* current_process = 0;
static struct process* processes[BasicOS_MAX_PROCESSES];

static void process_init(struct process* process) {
    memset(process, 0, sizeof(struct process));
}

struct process* get_current_process(void) {
    return current_process;
}

struct process* get_process(int pid) {
    return (pid < 0 || pid >= BasicOS_MAX_PROCESSES) ? NULL : processes[pid];
}

int process_switch(struct process* process) {

    // Later we can save a process context here. Say VGA memory of each process.
    // So each process gets the screen wiped and its current content saved somewhere.
    // This is a simple 1600 bytes overhead and a simple functionality.

    current_process = process;
    return 0;

}

static int process_load_binary(const char* filename, struct process* process) {

    int res = 0;
    int fd = fopen(filename, "r");
    if (!fd) {
        res = -EIO;
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res != NE) goto out;

    void* program_data_ptr = kzalloc(stat.file_size);
    if (!program_data_ptr) {
        res = -ENOMEM;
        goto out;
    }

    if (fread(program_data_ptr, stat.file_size, 1, fd) != 1) {
        res = -EIO;
        goto out;
    }

    process->ptr = program_data_ptr;
    process->size = stat.file_size;

    out:
    fclose(fd);
    return res;

}

static int process_load_data(const char* filename, struct process* process) {
    
    //* Handle different formats differently. For now only supports flat binary, no ELF etc.
    
    int res = 0;
    res = process_load_binary(filename, process);
    return res;
}

int process_map_binary(struct process* process) {
    int res = 0;

    paging_map_to(process->task->page_directory,
        (void*) BKE_TASK_PROGRAM_VIRTUAL_ADDR,
        process->ptr,
        (void*) paging_align_address( ((unsigned long) process->ptr) + process->size),
        PAGING_MASKS_ACCESS_ALL | PAGING_MASKS_IS_PRESENT | PAGING_MASKS_IS_WRITABLE
    );

    return res;

}

int process_map_memory(struct process* process) {
    
    //* Handle different format differently. Well, like process_load_data.

    int res = 0;
    res = process_map_binary(process);

    if (res < 0) {
        goto out;
    }

    paging_map_to(process->task->page_directory,
        (void*) BKE_TASK_PROGRAM_VIRTUAL_STACK_ADDR_END,
        process->stack,
        (void*) paging_align_address( ((unsigned long) process->stack) + BKE_TASK_USER_STACK_SIZE),
        PAGING_MASKS_IS_PRESENT | PAGING_MASKS_ACCESS_ALL | PAGING_MASKS_IS_WRITABLE
    );

    out:
    return res;
}

long get_free_process_slot() {
    for (unsigned long i = 0; i < BasicOS_MAX_PROCESSES; i++) {
        if (processes[i] == 0) return i;
    }

    return -EPROC_NOMEM;
}

int process_load(const char* filename, struct process** process) {
    int res = 0;

    int process_slot = get_free_process_slot();
    if (process_slot < 0) {
        res = -EPROC_NOMEM;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);
    out:
    return res;
}

int process_load_switch(const char* filename, struct process** process) {

    int res = process_load(filename, process);

    if (res == 0) {
        process_switch(*process);
    }

    return res;

}

int process_load_for_slot(const char* filename, struct process** process, int process_slot) {

    int res = 0;
    struct task* task = 0;
    struct process* _process;

    void* program_stack_ptr = 0;

    if (get_process(process_slot) != 0) {
        res = -EPID_ISTKN;
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process) {
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
        goto out;

    program_stack_ptr = kzalloc(BKE_TASK_USER_STACK_SIZE);
    if (!program_stack_ptr) {
        res = -ENOMEM;
        goto out;
    }
    _process->stack = program_stack_ptr;

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->id = process_slot;

    //* Make the task now.
    task = task_new(_process);
    if (ERROR_I(task) == 0) {
        res = ERROR_I(task);
        goto out;
    }

    _process->task = task;
    res = process_map_memory(_process);
    if (res < 0) goto out;

    *process = _process;
    processes[process_slot] = _process;

    out:

    if (res < 0) {
        if (_process) {
            if (_process->task)
                task_free(_process->task);

            if (_process->stack)
                kfree(_process->stack);

            if (_process->ptr)
                kfree(_process->ptr);

            kfree(_process);
        }
    }

    return res;

}