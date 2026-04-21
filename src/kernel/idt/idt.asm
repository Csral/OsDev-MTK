section .asm
global idt_load

; Interrupt handler exports

global enable_interrupts
global disable_interrupts

disable_interrupts:
    
    cli
    ret

enable_interrupts:

    sti
    ret

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret
    