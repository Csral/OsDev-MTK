[bits 32]
section .asm
global gdt_load

gdt_load:
    mov eax, [esp+4]
    mov [gdt_descriptor + 2], eax
    mov ax, [esp+8]
    mov [gdt_descriptor], ax
    lgdt [gdt_descriptor]

    xor eax, eax    ; Clear out eax.

    ret

section .data
gdt_descriptor:
    dw 0x00         ; Size
    dd 0x00         ; GDT Start address
