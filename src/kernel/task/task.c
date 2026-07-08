#include "task/task.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "status.h"
#include "task/process.h"

struct task* current_task = 0;

/* Task Linked List */
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process) {
    memset(task, 0, sizeof(struct task));

    task->page_directory = _gen_paging_4gb(PAGING_MASKS_IS_PRESENT | PAGING_MASKS_ACCESS_ALL);
    if (!task->page_directory) return -EIO;

    task->registers.eip = BKE_TASK_PROGRAM_VIRTUAL_ADDR;
    task->registers.ss = BKE_TASK_USER_DATA_SEGMENT;
    task->registers.cs = BKE_TASK_USER_CODE_SEGMENT;
    task->registers.esp = BKE_TASK_PROGRAM_VIRTUAL_STACK_ADDR_START;

    task->process = process;

    return 0;

}

struct task* task_new(struct process* process) {
    
    int res = NE;
    
    struct task* task = kzalloc(sizeof(struct task));
    if (!task) {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
    if (res != NE) goto out;

    if (task_head == 0) {
        task_head = task;
        task_tail = task;
        current_task = task;
        goto out;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

    out:
    if (IS_ERR(res)) {
        task_free(task);
        return ERROR(res);
    }

    return task;

}

struct task* task_current() {
    return current_task;
}

struct task* task_get_next() {
    return (current_task->next) ? current_task->next : task_head;
}

static void task_list_remove(struct task* task) {

    if (task->prev)
        task->prev->next = task->next;

    if (task == task_head)
        task_head = task->next;

    if (task == task_tail)
        task_tail = task->prev;

    if (task == current_task)
        current_task = task_get_next();

    return;

}

int task_free(struct task* task) {
    paging_free_4gb(task->page_directory);
    task_list_remove(task);
    kfree(task);
    return 0;
}

int task_switch(struct task* task) {

    current_task = task;
    paging_switch(task->page_directory);
    return 0;

}

int task_page() {
    user_registers();
    task_switch(current_task);
    return 0;
}

void task_run_first_task(void) {

    // First task run.
    if (!current_task) {
        kernel_panic("Task run failure: No current task to execute!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);

}