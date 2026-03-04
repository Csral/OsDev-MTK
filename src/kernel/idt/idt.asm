section .asm
global idt_load

; Interrupt handler exports
global idt_int_zero_handler
global general_protection_fault
global unhandled_interrupts

; 32-bit Kernel Handler functions
extern int_zero
extern int_gp_fault
extern unhandled_interrupts_handler_basic

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

idt_int_zero_handler:

    push ebp
    mov ebp, esp
    pushad

    call int_zero

    popad
    mov esp, ebp
    pop ebp

    iret

general_protection_fault:

    push ebp
    mov ebp, esp
    pushad

    push [ebp+4] ; err_code
    push [ebp+8] ; EIP of faulting instruction
    call int_gp_fault

    add esp, 8
    
    cli
    hlt

    popad
    mov esp, ebp
    pop ebp
    add esp, 4
    
    iret

unhandled_interrupts:

    push ebp
    mov ebp, esp
    pushad

    call unhandled_interrupts_handler_basic

    cli
    hlt

    ; restore proper stack before making this generic

    popad
    mov esp, ebp
    pop ebp
