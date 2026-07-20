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

void* sys_invoke_sys_command(struct interrupt_frame* frame) {

    struct task* task = task_current();
    struct command_argument* arguments = task_virtual_address_to_physical(task, task_get_stack_item(task, 0));
    if (!arguments || strlen(arguments[0].argument) == 0) return (void*) -EINVARG;

    // program to run
    struct command_argument* root_command_arg = &arguments[0];
    const char* prog_name = root_command_arg->argument;

    char path[_FS_MAX_PATH_LEN];
    strcpy(path, "0:/");
    strncpy(path+3, prog_name, sizeof(path));

    struct process* process = 0;
    int res = process_load_switch(path, &process);

    if (res < 0) return (void*) res;

    res = process_inject_arguments(process, root_command_arg);

    if (res < 0) return (void*) res;

    task_switch(process->task);
    task_return(&process->task->registers);

    return 0;
}

void* sys_get_process_arguments(struct interrupt_frame* frame) {
    
    struct task* task = task_current();
    struct process* process = task->process;
    struct process_arguments* arguments = task_virtual_address_to_physical(task, task_get_stack_item(task, 0));
    process_get_arguments(process, &arguments->argc, &arguments->argv);

    return 0;

}

void* sys_exit(struct interrupt_frame* frame) {

    /*
    *
    *   Improvements:
    *   Collect an exit code argument and switch to parent process
    *   and provide the error code into eax register of parent.
    *
    */

    struct process* process = task_current()->process;
    process_terminate(process);
    task_next();

    return 0;
    
}