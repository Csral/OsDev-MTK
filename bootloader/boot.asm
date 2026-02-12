org 0
bits 16

_start:
    jmp short setup
    nop

times 33 db 0

setup:
    jmp 0x7c0:start

start:
    cli ; Clear interrupt

    mov ax, 0x7c0
    mov ds, ax
    mov es, ax

    xor ax, ax
    mov ss, ax
    mov sp, 0x7c00

    sti ; Enable Interrupt

    mov ah, 0eh
    mov si, message
    call print

    cli
    hlt

print:

    lodsb
    
    test al, al
    jz end
    int 0x10
    jmp print

end:
    ret

message: db "Hello World from Csral", 0

times 510 - ($ - $$) db 0
dw 0xAA55