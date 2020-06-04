
progName = myELF
sourceName = main
gccFlags1 = -g -Wall -m32 -c -o
gccFlags2 = -g -Wall -m32 -o

all: $(progName)

$(progName): $(sourceName).o
	gcc $(gccFlags2) $(progName) $(sourceName).o

$(sourceName).o: $(sourceName).c
	gcc $(gccFlags1) $(sourceName).o $(sourceName).c

.PHONY: clean
clean:
	rm -f *.o $(progName)
