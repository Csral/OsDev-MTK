#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdint.h>
#include <stddef.h>
#include "config.h"
#include "task/task.h"

//* Uncomment this line to panic system
// #define PANIC_SYSTEM_ON_PROCESS_FORMAT_UNHANDLED

#define PROCESS_FORMAT_ELF 0x0
#define PROCESS_FORMAT_FBIN 0x1

typedef unsigned char process_format_t;

struct command_argument {
    char argument[512];
    struct command_argument* next;
};

struct process_arguments {
    int argc;
    char** argv;
};

struct process_allocation {
    void* ptr;
    size_t size;
};

struct process {
    uint32_t id;
    char filename[_FS_MAX_PATH_LEN];
    struct task* task;
    struct process_allocation allocations[BasicOS_MAX_ALLOCATIONS_ALLOWED_PER_PROCESS];
    process_format_t filetype;

    //* A pointer to the process memory.
    union {
        void* ptr;
        struct elf_file* elf_file;
    };
    

    void* stack; //* PTR to Stack
    uint32_t size; //* size of ptr.

    /* Keyboard */
    struct keyboard_buffer {

        char buffer[BasicOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;

    } keyboard;

    struct process_arguments arguments;

};

int process_switch(struct process* process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load_switch(const char* filename, struct process** process);
int process_load(const char* filename, struct process** process);
struct process* get_current_process(void);
struct process* get_process(int pid);
void* process_malloc(struct process* process, size_t size);
void process_free(struct process* process, void* ptr);
void process_get_arguments(struct process* process, int* argc, char*** argv);
int process_inject_arguments(struct process* process, struct command_argument* root_argument);
int process_terminate(struct process* process);

#endif