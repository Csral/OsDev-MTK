FILES = ./build/kernel.asm.o ./build/kernel.o ./build/gdt/gdt.asm.o ./build/gdt/gdt.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/idt/interrupts.asm.o ./build/idt/interrupts.o ./build/memory/memory.o ./build/task/tss.asm.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.asm.o ./build/memory/paging/paging.o ./build/disk/disk.o ./build/string/string.o ./build/fs/pparser.o ./build/disk/streamer.o ./build/fs/file.o ./build/fs/fat/fat16.o
INCLUDES = -I ./src/kernel/includes
FLAGS = -g -ffreestanding -nostdlib -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/extended.bin ./bin/kernel.bin

	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/extended.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin

	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin

	sudo mount -t vfat ./bin/os.bin /mnt/OsDevMnt
	# copy a file over to the bin
	sudo cp ./message.txt /mnt/OsDevMnt
	sudo umount /mnt/OsDevMnt

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./bin/extended.bin: ./src/boot/extended.asm
	nasm -f bin ./src/boot/extended.asm -o ./bin/extended.bin

./build/kernel.asm.o: ./src/kernel/kernel.asm
	nasm -f elf -g ./src/kernel/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/kernel.c -o ./build/kernel.o

./build/gdt/gdt.asm.o: ./src/kernel/gdt/gdt.asm
	nasm -f elf -g ./src/kernel/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

./build/gdt/gdt.o: ./src/kernel/gdt/gdt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/gdt/gdt.c -o ./build/gdt/gdt.o

./build/idt/idt.asm.o: ./src/kernel/idt/idt.asm
	nasm -f elf -g ./src/kernel/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./src/kernel/idt/idt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/idt/idt.c -o ./build/idt/idt.o

./build/idt/interrupts.asm.o: ./src/kernel/idt/idt.asm
	nasm -f elf -g ./src/kernel/idt/interrupts.asm -o ./build/idt/interrupts.asm.o

./build/idt/interrupts.o: ./src/kernel/idt/interrupts.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/idt/interrupts.c -o ./build/idt/interrupts.o

./build/memory/memory.o: ./src/kernel/memory/memory.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/memory/memory.c -o ./build/memory/memory.o

./build/task/tss.asm.o: ./src/kernel/task/tss.asm
	nasm -f elf -g ./src/kernel/task/tss.asm -o ./build/task/tss.asm.o

./build/io/io.asm.o: ./src/kernel/io/io.asm
	nasm -f elf -g ./src/kernel/io/io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./src/kernel/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./src/kernel/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/memory/heap/kheap.c -o ./build/memory/heap/kheap.o

./build/memory/paging/paging.asm.o: ./src/kernel/memory/paging/paging.asm
	nasm -f elf -g ./src/kernel/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/memory/paging/paging.o: ./src/kernel/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/disk/disk.o: ./src/kernel/disk/disk.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/disk/disk.c -o ./build/disk/disk.o

./build/disk/streamer.o: ./src/kernel/disk/streamer.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/disk/streamer.c -o ./build/disk/streamer.o

./build/fs/pparser.o: ./src/kernel/fs/pparser.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/fs/pparser.c -o ./build/fs/pparser.o

./build/fs/file.o: ./src/kernel/fs/file.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/fs/file.c -o ./build/fs/file.o

./build/fs/fat/fat16.o: ./src/kernel/fs/fat/fat16.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -I./src/fs/fat -std=gnu99 -c ./src/kernel/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/string/string.o: ./src/kernel/string/string.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel/string/string.c -o ./build/string/string.o

clean:
	rm -rf ./bin/*.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o