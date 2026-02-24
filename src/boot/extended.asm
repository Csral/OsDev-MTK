org 0x7E00
bits 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

EMain:

    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    sti

    mov si, ld_msg
    call print

    ; Do Stuff
    jmp 0x00:load_protected

load_protected:

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp CODE_SEG:pModeMain 

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

ld_msg: db "Started extended bootloader.", 0

; GDT

gdt_start:

gdt_null:

    dd 0x0
    dd 0x0

; offset 0x8
gdt_code: ; cs shud point here

    dw 0xffff ; segment limit first 0 to 15 bits
    dw 0x0000 ; base 16 to 23 bits
    db 0x0000 ; base 16 to 23 bits

    db 0x9a ; access byte
    db 11001111b ; high 4 bit and low 4 bit flags

    db 0 ; base 24 to 31 bits

; offset 0x10
gdt_data:   ; DS, SS, FS, ES, GS

    dw 0xffff ; segment limit first 0 to 15 bits
    dw 0x0000 ; base 16 to 23 bits
    db 0x0000 ; base 16 to 23 bits

    db 0x92 ; access byte
    db 11001111b ; high 4 bit and low 4 bit flags

    db 0 ; base 24 to 31 bits

gdt_end:

gdt_descriptor:

    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
pModeMain:

    mov eax, 2
    mov ecx, 100
    mov edi, 0x0100000

    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:

    mov ebx, eax ; backup the LBA
    ; send the highest 8 bit of lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; Select master drive
    mov dx, 0x1F6
    out dx, al

    ; Finished sending highest 8 bits of LBA

    ; Send total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending total sectors to read

    mov eax, ebx
    ; Send more bits of LBA
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of LBA

    ; send more LBA bits
    mov dx, 0x1F4
    mov eax, ebx ; Restore LBA
    shr eax, 8
    out dx, al 
    ; Finished sending more LBA bits

    ; Send upper 16 bits of the LBA

    mov dx, 0x1F5
    mov eax, ebx
    shr eax, 16
    out dx, al

    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory

.next_sector:

    push ecx
    
.try_again:

    ; not the best implementation: Refer https://wiki.osdev.org/ATA_PIO_Mode

    mov dx, 0x1F7
    in al, dx

    test al, 8
    jz .try_again

    ; read 256 words at once
    mov ecx, 256
    mov dx, 0x1F0
    rep insw

    pop ecx
    loop .next_sector

    ; End of reading sectors
    ret

times 512 - ($ - $$) db 0