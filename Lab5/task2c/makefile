all: myshell looper

myshell: LineParser.o main.o
	gcc -g -m32 -Wall -o myshell LineParser.o main.o

LineParser.o: LineParser.c LineParser.h
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 
 
main.o: main.c
	gcc -m32 -g -Wall -c -o main.o main.c

looper: looper.o
	gcc -g -m32 -Wall -o looper looper.o

looper.o: looper.c
		gcc -m32 -g -Wall -c -o looper.o looper.c
.PHONY: clean

clean: 
	rm -f *.o myshell looper