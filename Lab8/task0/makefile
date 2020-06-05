#All Targets

all: myELF

myELF: task0.o
		gcc -g -m32 -Wall -o myELF task0.o

task0.o: task0.c
		gcc -m32 -g -Wall -c -o task0.o task0.c

.PHONY: clean

clean:
		rm -f *.o myELF
