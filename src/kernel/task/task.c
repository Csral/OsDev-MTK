#include "task/task.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "status.h"
#include "task/process.h"
#include "string/string.h"
#include "interrupts.h"
#include "loader/formats/elf_loader.h"

struct task* current_task = 0;

/* Task Linked List */
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process) {
    memset(task, 0, sizeof(struct task));

    task->page_directory = _gen_paging_4gb(PAGING_MASKS_IS_PRESENT | PAGING_MASKS_ACCESS_ALL);
    if (!task->page_directory) return -EIO;

    
    if (process->filetype == PROCESS_FORMAT_ELF) {
        task->registers.eip = elf_header(process->elf_file)->e_entry;
    } else {
        task->registers.eip = BKE_TASK_PROGRAM_VIRTUAL_ADDR;
    }

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

static inline void task_save_state(struct task* task, struct interrupt_frame* frame) {

    task->registers.eip = frame->eip;
    task->registers.cs = frame->cs;
    task->registers.eflags = frame->eflags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;

};

int copy_string_from_task(struct task* task, void* virtual, void* physical, int max) {

    // Kernel page needs to be active

    if (max >= PAGING_PAGE_SIZE) {
        return -EINVARG;
    }

    int res = 0;
    char* tmp = kzalloc(max);

    if (!tmp) {
        res = -ENOMEM;
        goto out;
    }

    uint32_t* task_directory = task->page_directory->d_entry;
    uint32_t old_entry = paging_get(task_directory, tmp);

    paging_map(task->page_directory, tmp, tmp, PAGING_MASKS_IS_WRITABLE | PAGING_MASKS_IS_PRESENT | PAGING_MASKS_ACCESS_ALL);
    paging_switch(task->page_directory);

    strncpy(tmp, virtual, max);
    kernel_page();
    
    res = paging_set(task_directory, tmp, old_entry);

    if (res < 0) {
        res = -EIO;
        goto out_free;
    }

    strncpy(physical, tmp, max);

    out_free:
    kfree(tmp);

    out:
    return res;

};

void task_current_save_state(struct interrupt_frame* frame) {

    // Kernel page needs to be active
    struct task* task = task_current();
    if (!task) {
        kernel_panic("No task to save!\n");
    }

    task_save_state(task, frame);

}

int task_page() {
    user_registers();
    task_switch(current_task);
    return 0;
}

void task_page_task(struct task* task) {

    user_registers();
    paging_switch(task->page_directory);
    
}

void task_run_first_task(void) {

    // First task run.
    if (!current_task) {
        kernel_panic("Task run failure: No current task to execute!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);

}

void* task_get_stack_item(struct task* task, int idx) {

    // Must be in kernel page
    void* result = 0;

    uint32_t* sp_ptr = (uint32_t*) task->registers.esp;
    // switch to this specific task's page.
    task_page_task(task);
    result = (void*) sp_ptr[idx];
    kernel_page();

    return result;

}

void* task_virtual_address_to_physical(struct task* task, void* v_addr) {
    return paging_get_physical_address(task->page_directory->d_entry, v_addr);
}