#ifndef KEYBOARD_CLASSIC_DRIVER_H
#define KEYBOARD_CLASSIC_DRIVER_H

struct keyboard* classic_init(void);

// first -> keyboard
// second -> mouse

#define PS2_PORT 0x64
#define PS2_ENABLE_FIRST_PORT 0xAE
#define PS2_ENABLE_SECOND_PORT 0xA8

#define CLASSIC_KEYBOARD_KEY_RELEASED 0x80
#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

#endif