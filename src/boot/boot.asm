org 0x7c00
bits 16

jmp short setup
nop

; FAT 16 Header
OEMIdentifier               db  'BasicCOS'
bytes_per_sector            dw  0x0200
sectors_per_cluster         db  0x80
reserved_sectors            dw  200
num_fat_copies              db  0x02
root_directory_entries      dw  0x40
num_sectors                 dw  0x00
media_type                  db  0xF8
sectors_per_fat             dw  0x100
sectors_per_track           dw  0x20
number_of_heads             dw  0x40
num_hidden_sectors          dd  0x00
num_sectors_large           dd  0x773594

; Extended BPB (Dos 4.0)
drive_number                db  0x80
WinNTBit                    db  0x00
signature                   db  0x29
volume_id                   dd  0xD105
volume_id_string            db  'BasicOSBooT'
system_id_string            db  'FAT16   '

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

    jmp 0x00:EMain

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