FILES = ./build/cblank.o
INCLUDES = -I ./ -I ../stdlib/includes
FLAGS = -g -ffreestanding -nostdlib -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
STDLIB_FILES = ../stdlib/stdlib.elf

all: $(FILES)
	i686-elf-gcc -g -T ./linker.ld -o ./cblank.elf -ffreestanding -O0 -nostdlib -fpic -g $(FILES) $(STDLIB_FILES)

./build/cblank.o: ./blank.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./blank.c -o ./build/cblank.o

clean:
	rm -rf $(FILES)
	rm -rf ./blank.elf