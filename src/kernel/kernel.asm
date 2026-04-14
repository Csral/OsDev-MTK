[bits 32]

global _start
global problem
global __panic
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax

    mov ebp, 0x00200000
    mov esp, ebp

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; remap the master PIC 
    ; start initialization
    mov al, 0x11
    out 0x20, al
    out 0xA0, al

    ; set vector offsets
    mov al, 0x20
    out 0x21, al

    mov al, 0x28
    out 0xA1, al

    ; tell master PIC about slave at IRQ2
    mov al, 0x04
    out 0x21, al

    ; tell slave PIC its cascade identity
    mov al, 0x02
    out 0xA1, al

    ; set 8086 mode
    mov al, 0x01
    out 0x21, al
    out 0xA1, al

    mov al, 0x00
    out 0x21, al
    out 0xA1, al

    ; End remapping the master PIC

    call kernel_main

    sti
    cli
    .loop:
        hlt
        jmp .loop

__panic:

    cli
    .panic_loop:
        hlt
        jmp .panic_loop

problem:

    ; Check interrupts/traps here.

    ; mov eax, 0
    ; div eax

    ; jmp DATA_SEG:0xFFFF
    ; mov ax, 0x1233
    ; mov ds, ax

    ; int 50h
    ; hlt
    ret

times 512 - ($ - $$) db 0 ; for alignment
