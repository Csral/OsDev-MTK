[bits 32]

section .asm
; void restore_general_purpose_registers(struct registers* reg);
global restore_general_purpose_registers
global task_return
global user_registers

restore_general_purpose_registers:

    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    pop ebp
    ret

; void task_return(struct registers* regs);
task_return:

    mov ebp, esp

    ; PUSH data segment
    ; PUSH stack addr
    ; PUSH flags
    ; PUSH code segment
    ; PUSH IP

    mov ebx, [ebp+4]
    push dword [ebx+44] ; Data segment - SS
    push dword [ebx+40] ; Stack pointer - esp
    
    ; Push flags
    pushf
    pop eax

    or eax, 0x200 ; enable interrupts after IRET
    push eax

    push dword [ebx+32] ; Code segment - CS
    push dword [ebx+28] ; eip

    ; Setup segments
    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebp+4]
    call restore_general_purpose_registers
    add esp, 4

    iretd ; Switch to userland.

user_registers:

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret
