section .asm
global idt_load

; Interrupt handler exports

global enable_interrupts
global disable_interrupts

global idt_int_zero_handler
global idt_20_h
global int_21_h
global general_protection_fault
global unhandled_interrupts
global no_interrupt_routine

; 32-bit Kernel Handler functions
extern int_zero
extern timer_handler
extern int_21_handler
extern int_gp_fault
extern unhandled_interrupts_handler_basic
extern no_interrupt_routine_handler

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

    iret

idt_20_h:
    cli

    push ebp
    mov ebp, esp
    pushad

    call timer_handler

    popad
    mov esp, ebp
    pop ebp
    sti
    iret

int_21_h:

    cli
    push ebp
    mov ebp, esp
    pushad

    call int_21_handler
    
    popad
    mov esp, ebp
    pop ebp
    sti

    iret

no_interrupt_routine:

    cli
    push ebp
    mov ebp, esp
    pushad

    call no_interrupt_routine_handler

    ; restore proper stack before making this generic
    popad
    mov esp, ebp
    pop ebp

    sti
    iret
