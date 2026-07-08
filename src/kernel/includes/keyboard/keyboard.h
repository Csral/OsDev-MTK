#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef int(*KEYBOARD_INIT_FUNCTION)();

struct keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[35];
    struct keyboard* next;
};

// forward declaration
struct process;

void keyboard_init(void);
int keyboard_insert(struct keyboard* keyboard);
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop();

#endif