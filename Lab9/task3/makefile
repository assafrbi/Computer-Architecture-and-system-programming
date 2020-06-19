progName1 = skeleton
srcName1 = skeleton

all: skeleton

skeleton: skeleton.s
	nasm -f elf32 skeleton.s -o skeleton.o
	ld -melf_i386 skeleton.o -o skeleton

.PHONY: clean
clean:
	rm -f *.o skeleton
