[bits 32]

; Helper functions etc

section .asm

global get_current_cs

get_current_cs:

    mov ax, cs
    ret