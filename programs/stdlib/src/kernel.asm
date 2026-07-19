; Contain functions to call kernel.
[bits 32]

section .asm

global print:function
global basic_os_kernel_getkey:function
global basic_os_kernel_putchar:function
global basic_os_kernel_malloc:function
global basic_os_kernel_free:function
global basic_os_kernel_start_new_process:function
global basic_os_kernel_system:function
global basic_os_kernel_get_process_arguments:function

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

; char basic_os_kernel_getkey(void);
basic_os_kernel_getkey:
    
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

; int basic_os_kernel_start_new_process(const char* filename);
basic_os_kernel_start_new_process:

    push ebp
    mov ebp, esp

    mov eax, 7              ; sys_process_load_start
    push dword [ebp+8]      ; filename
    int 0x80
    add esp, 4

    pop ebp
    ret

; void basic_os_kernel_system(struct command_argument* arguments);
basic_os_kernel_system:

    push ebp
    mov ebp, esp

    mov eax, 8                  ; sys_invoke_sys_command
    push dword [ebp+8]          ; arguments
    int 0x80
    add esp, 4

    pop ebp
    ret

; void basic_os_kernel_get_process_arguments(struct process_arguments* arguments)
basic_os_kernel_get_process_arguments:

    push ebp
    mov ebp, esp

    mov eax, 9              ; sys_get_process_arguments
    push dword [ebp+8]      ; arguments
    int 0x80
    add esp, 4

    pop ebp
    ret
