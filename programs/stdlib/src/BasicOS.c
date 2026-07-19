#include "BasicOS.h"
#include "string.h"
#include "stdlib.h"
#include "status.h"

// User programs do not need to concern themselves with this routine
// Implemented in kernel.as
extern char basic_os_kernel_getkey(void);

char getkey(void) {
    
    char val = 0x00U;
    do {
        val = basic_os_kernel_getkey();
    } while (val == 0x00);
    return val;

}

struct command_argument* basic_os_parse_command(const char* command, int max) {

    struct command_argument* root_argument = 0;
    char scommand[1025];

    if (max >= (int) sizeof(scommand)) {
        return 0;
    }

    strncpy(scommand, command, sizeof(scommand));

    char* token = strtok(scommand, " ");

    if (!token) {
        goto out;
    }

    root_argument = (struct command_argument*) malloc(sizeof(struct command_argument));
    if (!root_argument) goto out;

    strncpy(root_argument->argument, token, sizeof(root_argument->argument));

    root_argument->next = 0;

    struct command_argument* current = root_argument;
    token = strtok(NULL, " ");

    while (token != 0x00) {

        struct command_argument* new_command =  (struct command_argument*) malloc(sizeof(struct command_argument));
        if (!new_command) break;

        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;

        token = strtok(NULL, " ");

    }

    out:
    return root_argument;

}

// User programs do not need to concern themselves with this routine
// Implemented in kernel.as
extern int basic_os_kernel_system(struct command_argument* arguments);

int basic_os_system_run(const char* command) {

    // maximum of 1024 letters
    char buf[1024];
    strncpy(buf, command, sizeof(buf));

    struct command_argument* root_command_argument = basic_os_parse_command(command, sizeof(buf));

    if (!root_command_argument) {
        return -EINVARG;
    }

    basic_os_kernel_system(root_command_argument);
    return 0;

}