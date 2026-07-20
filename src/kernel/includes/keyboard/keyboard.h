#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_CAPS_LOCK_ON 0x00000001
#define KEYBOARD_CAPS_LOCK_OFF 0x0000000

typedef int(*KEYBOARD_INIT_FUNCTION)();

struct keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[35];
    struct keyboard* next;
    int caps_lock_state;
};

// forward declaration
struct process;

void keyboard_init(void);
int keyboard_insert(struct keyboard* keyboard);
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop();

void keyboard_set_caps_lock(struct keyboard* keyboard, int state);
int keyboard_get_caps_lock(struct keyboard* keyboard);

#endif