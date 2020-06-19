#All Targets

all: myELF

myELF: task0a.o
		gcc -g -m32 -Wall -o myELF task0a.o

task0a.o: task0a.c
		gcc -m32 -g -Wall -c -o task0a.o task0a.c

.PHONY: clean

clean:
		rm -f *.o myELF
