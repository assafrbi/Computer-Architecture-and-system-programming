progName1 = task0b
srcName1 = task0b

all: task1

task1: skeleton.s
	nasm -f elf32 skeleton.s -o skeleton.o
	ld -melf_i386 skeleton.o -o task1

.PHONY: clean
clean:
	rm -f *.o task1
