section .asm


global idt_int_zero_handler
global invalid_opcode_fault_handler

global unhandled_interrupts
global no_interrupt_routine

global isr80h_wrapper

global interrupt_pointer_table

; 32-bit Kernel Handler functions
extern int_zero
extern idt_invalid_opcode_fault_handler
extern unhandled_interrupts_handler_basic
extern no_interrupt_routine_handler
extern isr80h_handler

extern interrupt_handler

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

%macro interrupt_no_err 1
    global int%1
    int%1:
        ; IA-32 mode stack after priv. change
        ; IP             ----------> stack top.
        ; CS
        ; FLAGS
        ; SP
        ; SS
        
        ; Notice we have no error code? So we send a dummy 0x00 to make it generic. Read about this in issues.
        push dword 0x00

        ; append registers on them 
        pushad

        ; push interrupt frame.
        push esp
        push dword %1

        call interrupt_handler
        add esp, 8

        popad
        
        ; pop the fake err code
        add esp, 4
        iret
%endmacro

%macro interrupt_err 1
    global int%1
    int%1:
        ; IA-32 mode stack after priv. change
        ; Error Code (pushed onto stack)             ----------> stack top.
        ; IP
        ; CS
        ; FLAGS
        ; SP
        ; SS                        

        ; append registers on them 
        pushad

        ; push interrupt frame.
        push esp
        push dword %1

        call interrupt_handler
        add esp, 8

        popad

        ; pop the err code
        add esp, 4
        iret
%endmacro

%assign i 0
%rep 256
    ; Interrupts with error.
    %if i = 8 || i = 10 || i = 11 || i = 12 || i = 13 || i = 14 || i = 17 || i = 21
        interrupt_err i
    %else
        interrupt_no_err i
    %endif
%assign i i+1
%endrep

isr80h_wrapper:

    cli
    
    ; IA-32 mode stack after priv. change
    ; IP
    ; CS
    ; FLAGS
    ; SP
    ; SS

    ; Push fake error code
    push dword 0x00

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
    ; Remove the fake err_code
    add esp, 4

    mov eax, [tmp_res]
    iretd

section .data
; Temp register to store return value from isr80h_handler
tmp_res: dd 0

%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 256
    interrupt_array_entry i
%assign i i+1
%endrep
