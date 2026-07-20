[bits 32]

global _start
extern _cstart
extern exit

section .asm

_start:
    call _cstart
    call exit
    ret