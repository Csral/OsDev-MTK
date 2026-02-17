[bits 32]

global _start
global problem
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

    call kernel_main

    cli
    hlt

problem:

    ; mov eax, 0
    ; div eax

    int 0
    ; ret

times 512 - ($ - $$) db 0 ; for alignment
