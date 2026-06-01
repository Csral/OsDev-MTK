#ifndef _KERNEL_TASK_H_
#define _KERNEL_TASK_H_

#include "config.h"
#include "memory/paging/paging.h"

//* If we ever implement 64 bit mode, just undef this and redef _ARCH_x64
//* Alternatively, handle a configuration header. Use KConfig etc.
#define _ARCH_x86
//* Usually all this goes into something clean like a types.h but for simple implementation, this works.

#ifdef _ARCH_x86
typedef uint32_t __rs; //* register size
#endif

struct registers {
    __rs edi;
    __rs esi;
    __rs ebp;
    __rs ebx;
    __rs edx;
    __rs ecx;
    __rs eax;
    __rs eip;
    __rs cs;
    __rs eflags;
    __rs esp;
    __rs ss;
};

struct process;
struct task {

    struct paging_4gb_memory_map* page_directory; // Page directory for this task.
    struct registers registers;
    struct process* process; // Process of the task.
    struct task* prev; // Prev task in linked list.
    struct task* next; // Next task in linked list.

};

struct task* task_new(struct process* process);
struct task* task_current();
int task_free(struct task* task);
int task_switch(struct task* task);
int task_page();
struct task* task_get_next();

void task_run_first_task(void);

extern void task_return(struct registers* regs);
extern void restore_general_purpose_registers(struct registers* regs);
extern void user_registers(void);

#endif