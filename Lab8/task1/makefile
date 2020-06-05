#All Targets

all: myELF

myELF: task1.o
		gcc -g -m32 -Wall -o myELF task1.o

task1.o: task1.c
		gcc -m32 -g -Wall -c -o task1.o task1.c

.PHONY: clean

clean:
		rm -f *.o myELF
