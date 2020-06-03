all: myshell

myshell: LineParser.o task1.o
	gcc -g -m32 -Wall -o myshell LineParser.o task1.o

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 
 
task1.o: task1.c
	gcc -m32 -g -Wall -c -o task1.o task1.c

.PHONY: clean

clean: 
	rm -f *.o myshell