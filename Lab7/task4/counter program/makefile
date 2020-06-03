#All Targets

all: counter

counter: main.o
		gcc -g -m32 -fno-pie -Wall -o counter main.o

main.o: main.c
		gcc -g -m32 -fno-pie -Wall -c -o main.o main.c

.PHONY: clean

clean:
		rm -f *.o counter
