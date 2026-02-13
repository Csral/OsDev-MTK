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

    mov ah, 02h
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov bx, buffer

    int 0x13

    jc error
    mov si, buffer
    call print

    cli
    hlt

error:

    mov si, err_msg
    call print

    cli
    hlt

print:

    mov ah, 0eh
    lodsb
    
    test al, al
    jz end
    int 0x10
    jmp print

end:
    ret

suc_msg: db "Loaded sector", 0
err_msg: db "Failed to load sector.", 0

times 510 - ($ - $$) db 0
dw 0xAA55

buffer: