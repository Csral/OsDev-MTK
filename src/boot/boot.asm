org 0x7c00
bits 16

_start:
    jmp short setup
    nop

times 33 db 0

setup:
    jmp 0x00:start

start:
    cli ; Clear interrupt

    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    sti ; Enable Interrupt

    call clear_window

    ; Load extended bootloader

    mov ah, 0x02
    mov al, 0x01
    xor ch, ch
    mov cl, 0x02
    xor dh, dh
    mov bx, EMain

    int 0x13
    jc ELoadError

    mov si, suc_msg
    call print

    jmp EMain

clear_window:

    push bp
    mov bp, sp

    pusha

    mov ah, 0x07
    xor al, al
    mov bh, 0x07 ; white on black

    xor cx, cx ; low right row&col
    mov dh, 24 ; row up left
    mov dl, 79 ; col up left

    int 0x10

    mov ah, 0x02
    xor bh, bh
    xor dx, dx

    int 0x10

    popa

    mov sp, bp
    pop bp

    ret

print:

    ; load si with string
    mov ah, 0x0E

    .loop:
        lodsb ; si reg and adds one
        test al, al
        jz .end

        int 0x10 ; print
        jmp .loop

    .end:
        ret

ELoadError:

    mov si, err_msg
    call print

    cli
    hlt

err_msg: db "Failed to load extended bootloader!", 0
suc_msg: db "Jumping to Extended bootloader", 0

times 510 - ($ - $$) db 0
dw 0xAA55

EMain: