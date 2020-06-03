all: encoder

encoder: start.o util.o task1b.o
	ld -m elf_i386 start.o task1b.o util.o -o encoder

start.o: lab4_start.s
	nasm -f elf32 lab4_start.s -o start.o

util.o: lab4_util.c lab4_util.h
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_util.c -o util.o

task1b.o: task1b.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector task1b.c -o task1b.o

.PHONY: clean

clean: 
	rm -f *.o encoder
