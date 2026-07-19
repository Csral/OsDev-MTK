[bits 32]

global _start
extern _cstart

section .asm

_start:
    call _cstart
    ret