#ifndef KEYBOARD_CLASSIC_DRIVER_H
#define KEYBOARD_CLASSIC_DRIVER_H

struct keyboard* classic_init(void);

// first -> keyboard
// second -> mouse

#define PS2_PORT 0x64
#define PS2_ENABLE_FIRST_PORT 0xAE
#define PS2_ENABLE_SECOND_PORT 0xA8

#endif