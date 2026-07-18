section .asm

global idt_int_zero_handler
global invalid_opcode_fault_handler
global unhandled_interrupts
global no_interrupt_routine
global isr80h_wrapper
global interrupt_pointer_table

; 32-bit Kernel Handler functions
extern isr80h_handler
extern interrupt_handler

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

    %if i = 0x80
        dd isr80h_handler
    %else
        interrupt_array_entry i
    %endif
    
%assign i i+1
%endrep
