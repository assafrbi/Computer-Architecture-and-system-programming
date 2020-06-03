#All Targets

all: mypipe

mypipe: main.o LineParser.o
		gcc -g -m32 -Wall -o mypipe main.o LineParser.o

main.o: main.c
		gcc -m32 -g -Wall -c -o main.o main.c
		
LineParser.o: LineParser.c LineParser.h
		gcc -m32 -g -Wall -c -o LineParser.o LineParser.c


.PHONY: clean

clean:
		rm -f *.o mypipe
