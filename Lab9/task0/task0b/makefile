progName1 = task0b
srcName1 = task0b

all: task0b

task0b: skeleton.s
	nasm -f elf32 skeleton.s -o skeleton.o
	ld -melf_i386 skeleton.o -o task0b

.PHONY: clean
clean:
	rm -f *.o task0b
