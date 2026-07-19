#include "isr80h/process.h"
#include "task/task.h"
#include "task/process.h"
#include "status.h"
#include "config.h"
#include "string/string.h"

void* sys_process_load_start(struct interrupt_frame* frame) {

    void* filename_ptr = task_get_stack_item(task_current(), 0);
    char filename[_FS_MAX_PATH_LEN];

    int res = copy_string_from_task(task_current(), filename_ptr, filename, sizeof(filename));
    if (res < 0) goto out;

    // emulate a PATH
    // I think it would be better to implement a 'search' functionality
    // in filesystem along with environmental variables so we can
    // iterate over the path directories and decide to return an error.
    // Speaking of which, I do not think this specific function's return
    // will propagate through to the userland program as we restore
    // their registers including eax, albeit the current implementation
    // will never return in first place.
    char path[_FS_MAX_PATH_LEN];
    strcpy(path, "0:/");
    strcpy(path+3, filename);

    struct process* process = 0;
    res = process_load_switch(path, &process);

    if (res < 0) goto out;

    task_switch(process->task);
    task_return(&process->task->registers);

    out:
    return (void*) res;

}