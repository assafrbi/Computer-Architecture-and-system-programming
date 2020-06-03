#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){ 

	printf("Starting the program\n");

	while(1) {
		sleep(2);
	}

	return 0;
}

void handle(int my_signal){
	printf("Loop handle's with signal %s\n" , strsignal(my_signal));
}