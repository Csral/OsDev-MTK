section .asm


global idt_int_zero_handler
global invalid_opcode_fault_handler
global general_protection_fault

global idt_20_h
global int_21_h
global unhandled_interrupts
global no_interrupt_routine

global isr80h_wrapper

; 32-bit Kernel Handler functions
extern int_zero
extern idt_invalid_opcode_fault_handler
extern timer_handler
extern int_21_handler
extern int_gp_fault
extern unhandled_interrupts_handler_basic
extern no_interrupt_routine_handler
extern isr80h_handler

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

    pushad
    mov eax, esp
    push eax
    call int_gp_fault
    add esp, 4
    popad
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


isr80h_wrapper:

    cli
    
    ; IA-32 mode stack after priv. change
    ; IP
    ; CS
    ; FLAGS
    ; SP
    ; SS

    ; append registers on them 
    pushad

    ; push interrupt frame.
    push esp
    ; syscall ID
    push eax

    call isr80h_handler
    mov dword[tmp_res], eax
    add esp, 8
    
    ; get back the registers of process
    popad

    mov eax, [tmp_res]
    iretd

section .data
; Temp register to store return value from isr80h_handler
tmp_res: dd 0
