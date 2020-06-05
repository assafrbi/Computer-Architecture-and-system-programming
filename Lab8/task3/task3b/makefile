#All Targets

all: myELF

myELF: task3b.o
		gcc -g -m32 -Wall -o myELF task3b.o

task3b.o: task3b.c
		gcc -m32 -g -Wall -c -o task3b.o task3b.c

.PHONY: clean

clean:
		rm -f *.o myELF
