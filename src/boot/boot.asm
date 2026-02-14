org 0x7c00
bits 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short setup
    nop

times 33 db 0

setup:
    jmp 0x00:start

start:
    cli ; Clear interrupt

    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    sti ; Enable Interrupt

    jmp load_protected

load_protected:

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp CODE_SEG:pModeMain 

; GDT

gdt_start:

gdt_null:

    dd 0x0
    dd 0x0

; offset 0x8
gdt_code: ; cs shud point here

    dw 0xffff ; segment limit first 0 to 15 bits
    dw 0x0000 ; base 16 to 23 bits
    db 0x0000 ; base 16 to 23 bits

    db 0x9a ; access byte
    db 11001111b ; high 4 bit and low 4 bit flags

    db 0 ; base 24 to 31 bits

; offset 0x10
gdt_data:   ; DS, SS, FS, ES, GS

    dw 0xffff ; segment limit first 0 to 15 bits
    dw 0x0000 ; base 16 to 23 bits
    db 0x0000 ; base 16 to 23 bits

    db 0x92 ; access byte
    db 11001111b ; high 4 bit and low 4 bit flags

    db 0 ; base 24 to 31 bits

gdt_end:

gdt_descriptor:

    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
pModeMain:

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax

    mov ebp, 0x00200000
    mov esp, ebp

    cli
    hlt

times 510 - ($ - $$) db 0
dw 0xAA55