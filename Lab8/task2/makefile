#All Targets

all: myELF

myELF: task2.o
		gcc -g -m32 -Wall -o myELF task2.o

task2.o: task2.c
		gcc -m32 -g -Wall -c -o task2.o task2.c

.PHONY: clean

clean:
		rm -f *.o myELF
