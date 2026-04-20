[bits 32]

global _start
global problem
global __panic
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    
    ; mov ax, DATA_SEG
    ; mov ds, ax
    ; mov es, ax
    ; mov fs, ax
    ; mov gs, ax
    ; mov ss, ax

    lgdt [kernel_gdt_descriptor]

    jmp CODE_SEG:.reload_cs
    .reload_cs:
        mov ax, DATA_SEG
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

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
    cli
    call kernel_main

    jmp $                 ; just to be sure.
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


; Kernel's GDT
kernel_gdt:

kernel_gdt_null:

    dd 0x0
    dd 0x0

; offset 0x8
kernel_gdt_code: ; cs shud point here

    dw 0xFFFF       ; segment limit first 0 to 15 bits
    
    dw 0x0000       ; base 0 to 15 bits
    db 0x00         ; base 16 to 23 bits
    
    db 0x9A         ; Access byte
    db 0xCF         ; Flags + 16 to 19 bits of limit

    db 0x0          ; base 24 to 31 bits

; offset 0x10
kernel_gdt_data:   ; DS, SS, FS, ES, GS

    dw 0xFFFF       ; segment limit first 0 to 15 bits
    
    dw 0x0000       ; base 0 to 15 bits
    db 0x00         ; base 16 to 23 bits
    
    db 0x92         ; Access byte
    db 0xCF         ; Flags + 16 to 19 bits of limit

    db 0x0          ; base 24 to 31 bits

kernel_gdt_end:

kernel_gdt_descriptor:

    dw kernel_gdt_end - kernel_gdt - 1
    dd kernel_gdt

times 512 - ($ - $$) db 0 ; for alignment
