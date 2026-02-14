all:
	nasm -f bin bootloader/boot.asm -o ./bin/bootloader.bin

clean:
	rm -rf ./bin/*.bin
	rm -rf ./build/*