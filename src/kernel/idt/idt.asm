section .asm
global idt_load

; Interrupt handler exports
global idt_int_zero_handler

; 32-bit Kernel Handler functions
extern int_zero

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

idt_int_zero_handler:

    pushad

    cld
    call int_zero

    popad
    iret
