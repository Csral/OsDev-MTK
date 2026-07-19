; Contain functions to call kernel.
[bits 32]

section .asm

global print:function
global getkey:function
global basic_os_kernel_putchar:function
global basic_os_kernel_malloc:function
global basic_os_kernel_free:function

; void print(const char* msg)
print:
    push ebp
    mov ebp, esp

    push dword [ebp+8]
    mov eax, 1
    int 0x80 ; sys_print
    add esp, 4

    pop ebp
    ret

; char getkey(void);
getkey:
    
    push ebp
    mov ebp, esp

    mov eax, 3
    int 0x80
    
    pop ebp
    ret

; void basic_os_kernel_putchar(char ch);
basic_os_kernel_putchar:
    push ebp
    mov ebp, esp

    mov eax, 4              ; sys_put_char
    push dword [ebp+8]      ; ch
    int 0x80
    add esp, 4

    pop ebp
    ret

; void* basic_os_kernel_malloc(size_t size);
basic_os_kernel_malloc:
    push ebp
    mov ebp, esp

    mov eax, 5              ; sys_malloc
    push dword [ebp+8]      ; size
    int 0x80
    add esp, 4

    pop ebp
    ret

; void basic_os_kernel_free(void* ptr);
basic_os_kernel_free:
    push ebp
    mov ebp, esp

    mov eax, 6
    push dword [ebp+8]
    int 0x80
    add esp, 4

    pop ebp
    ret
