[bits 32]

section .asm

global _start
_start:
    jmp label

label:

    ; To test if flags are accounted for
    ; we can try to write something into the code section
    ; which is usually expected to be RX
    
    ; Since we expect this specific program to start
    ; at 0x400000 and that is the address of _start
    ; or its subsequent instructions, I think we can
    ; try to write to that address.

    ; mov eax, 0x400000
    ; mov [eax], 0xA0

    ; This should trigger a page fault.

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
