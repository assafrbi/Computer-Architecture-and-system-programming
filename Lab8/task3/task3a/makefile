#All Targets

all: myELF

myELF: task3a.o
		gcc -g -m32 -Wall -o myELF task3a.o

task3a.o: task3a.c
		gcc -m32 -g -Wall -c -o task3a.o task3a.c

.PHONY: clean

clean:
		rm -f *.o myELF
