[bits 32]

section .asm

global _start
_start:
    jmp label

label:

    call get_key
    push eax
    mov eax, 4 ; sys_putchar
    int 0x80

    add esp, 4

    jmp label

get_key:

    mov eax, 3 ; sys_get_key
    int 0x80

    cmp eax, 0x00
    je get_key
    ret

section .data
msg: db "Hello World", 0
