section .asm


global idt_int_zero_handler
global invalid_opcode_fault_handler
global general_protection_fault

global idt_20_h
global int_21_h
global unhandled_interrupts
global no_interrupt_routine

; 32-bit Kernel Handler functions
extern int_zero
extern idt_invalid_opcode_fault_handler
extern timer_handler
extern int_21_handler
extern int_gp_fault
extern unhandled_interrupts_handler_basic
extern no_interrupt_routine_handler

idt_int_zero_handler:

    push ebp
    mov ebp, esp
    pushad

    call int_zero

    popad
    mov esp, ebp
    pop ebp

    iret


invalid_opcode_fault_handler:

    push ebp
    mov ebp, esp
    pushad

    call idt_invalid_opcode_fault_handler
    ; As of now, we do not return from this fault handler - we panic the kernel.

    popad
    mov esp, ebp
    pop ebp

    iret

general_protection_fault:

    push ebp
    mov ebp, esp
    pushad

    push [ebp] ; err_code
    push [ebp+4] ; EIP of faulting instruction
    call int_gp_fault
    add esp, 8
    
    popad
    mov esp, ebp
    pop ebp
    add esp, 4          ; Pop the ERR_CODE out of stack
    
    iret

unhandled_interrupts:

    push ebp
    mov ebp, esp
    pushad

    call unhandled_interrupts_handler_basic
    ; Do not expect to return from this interrupt.
    cli
    .stop_trying_to_rtn:
        hlt
        jmp .stop_trying_to_rtn

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
