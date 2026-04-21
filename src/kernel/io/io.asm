section .asm

global insb
global insw
global outb
global outw

insb:

    push ebp
    mov ebp, esp
    push edx

    xor eax, eax
    mov edx, [ebp+8]

    in al, dx

    pop edx
    mov esp, ebp
    pop ebp

    ret

insw:

    push ebp
    mov ebp, esp
    push edx

    xor eax, eax
    mov edx, [ebp+8]

    in ax, dx

    pop edx
    mov esp, ebp
    pop ebp

    ret

outb:

    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp+12]   ; value
    mov edx, [ebp+8]    ; port

    out dx, al

    popad
    mov esp, ebp
    pop ebp

    ret

outw:

    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp+12]   ; value
    mov edx, [ebp+8]    ; port

    out dx, ax

    popad
    mov esp, ebp
    pop ebp

    ret
