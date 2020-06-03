all: task0

task0: start.o util.o main.o
	ld -m elf_i386 start.o main.o util.o -o task0

start.o: lab4_start.s
	nasm -f elf32 lab4_start.s -o start.o

util.o: lab4_util.c lab4_util.h
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_util.c -o util.o

main.o: lab4_main.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector lab4_main.c -o main.o

.PHONY: clean

clean: 
	rm -f *.o task0