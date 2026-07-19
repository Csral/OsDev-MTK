FILES = ./build/shell.o
INCLUDES = -I ./includes -I ../stdlib/includes
FLAGS = -g -ffreestanding -nostdlib -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
STDLIB_FILES = ../stdlib/stdlib.elf

all: $(FILES)
	i686-elf-gcc -g -T ./linker.ld -o ./shell.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(STDLIB_FILES)

./build/shell.o: ./src/shell.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/shell.c -o ./build/shell.o

clean:
	rm -rf $(FILES)
	rm -rf ./shell.elf