all:

	nasm -f bin bootloader/boot.asm -o bootloader.bin
	dd if=./message.txt >> ./bootloader.bin
	dd if=/dev/zero bs=512 count=1 >> ./bootloader.bin