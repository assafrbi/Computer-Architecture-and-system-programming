all: task0

task0: LineParser.o main.o
	gcc -g -m32 -Wall -o task0 LineParser.o main.o

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 
 
main.o: main.c
	gcc -m32 -g -Wall -c -o main.o main.c

.PHONY: clean

clean: 
	rm -f *.o task0