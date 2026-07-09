# Issue
My sys calls were failing but the reason was obv once I got to debugging:

```c
Working:
        (gdb) print *frame $1= {edi = 0, esi = 0, ebp = 2097084, handler_esp = 6291436, ebx = 0, edx = 0, ecx = 0 , eax=2, eip = 4194323, CS = 27 , eflags = 530, esp = 4190200, ss = 35 }

        (gdb) print *handler $2 = { void*(struct interrupt_frame*) } 0x100fc0 <sys_print_ext>
        size_requested = 1025. valid.

        Failing:
        (gdb) print *frame $1 = {edi = 0, esi = 0, ebp = 2097084, handler_esp = 6291436, ebx = 0, edx= 0, ecx = 0, eax 2, err_code = 4194323, eip = 27, cs 530, eflags = 4190200, esp 35, ss = 0} 
```

So, I know what's going on in here. Since err_code is optional and in int 0x80 its not there, when we "access" the stack we screw up big time!
But as I already mentioned, we are also generalizing the stack frame and its handlers:

```c
#include "io.h"

#define _ACK_IRQ_EOI outb(0x20, 0x20);

// The handler decides what values it will use.
struct interrupt_frame {

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;

    // Now since we are having generic handler, it is better to know what this is
    // useful for. So, this esp is set by 'pushad' instruction. As such, it points
    // to the HANDLER's stack immediately after priv change.
    // Simply put: the processor pushes onto stack in the order of SS of the userland
    // task, followed by ESP, EFLAGS, CS, EIP, Err code (if any applicable).
    // Or: the first value accessed by this frame is Err code, EIP, CS, EFLAGS, ESP
    // and SS.
    uint32_t handler_esp; // technically esp but not the one we are concerned with
    
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // since we generally pass the address of stack here, the layout must
    // match the exact behavior by CPU which was mentioned above.

    uint32_t err_code; // Since we are abstracting away interrupt wrappers and having a generic handler

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;

} __attribute__((__packed__));

```


Although the main concern lies after the line `%macro interrupt 1`, as a reference, this was the asm file when this issue existed:
```.asm
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

%macro interrupt 1
    global int%1
    int%1:
        ; IA-32 mode stack after priv. change
        ; Error Code (optional)             ----------> stack top.
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

        ; So we won't remove error code here, in other words the handler
        ; should pop this value out. Is this possible? Idk.
        ; But we do have a way to make it possible

        ; My current idea is simple:
        ; The handler returns a numeric value that will be added to esp
        ; along with 8 to remove arguments.

        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
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

%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep

```

Fixes I could think of:
* A simple fix would be to push a fake error code but I don't really consider that as a fix.
* Another is to dynamically decide and identify error_code using frame->handler_esp for handlers that expect a potential error code but it'll make code complex for no reason.
* Final fix is to eliminate "err_code" from my interrupt frame. The asm file and its generic handlers will still do the same job but the handler's declaration will decide what the frame's layout is. This has its pros and cons but the biggest con is that the generic handler will not be generic anymore. Currently its generic because the higher level 'C' handlers do not care about the underlying handler. They just invoke the function registered with that ID. Refer this piece of code:

```c
void interrupt_handler(int interrupt, struct interrupt_frame* frame) {

    kernel_page();
    if (interrupt_callbacks[interrupt] != 0) {
        task_current_save_state(frame);
        interrupt_callbacks[interrupt](frame);
    }

    task_page();
    // if (interrupt == 13) int_gp_fault(frame);
    
    // Specifically sending ACK if needed to preserve CPU cycles and maintain correctness
    if (interrupt >= 0x20 && interrupt < 0x30) {
        // PIC needs ACK
        _ACK_IRQ_EOI
        // Master and slave scenario not handled well. We can come back to it later if needed.
    }
    
}

int idt_register_interrupt_callback(int interrupt_id, INTERRUPT_CALLBACK_FUNCTION callback) {

    if (interrupt_id < 0 || interrupt_id >= IDT_MAX_ENTRIES) {
        return -EINVARG;
    }

    interrupt_callbacks[interrupt_id] = callback;
    return NE;

}
```

# Solution
None of them sound right. What's the actual solution to this issue?
Its kinda simple. We make the higher level handlers generic and push a dummy 0x00 error code at lower level by splitting the macros.
This is possible because we already know which interrupts push error code.

This also fixes another issue I was addressing when using only one macro. How do I know to pop out the error code?
If you're unaware, processor leaves the responsibility of removing error code from stack onto us.
Hm... What if we don't remove it? Well, then we corrupt the state of processor. Why? Because processor assumes the value of error code is the value of EIP and returns execution at that address which is uh, really bad! 

Under the context of only using one macro; the solution I came with is:
* So we won't remove error code here, in other words the handler should pop this value out. Is this possible? Idk. But we do have a way to make it possible My current idea is simple: The handler returns a numeric value that will be added to esp along with 8 to remove arguments.

However, now both macros remove it regardless.