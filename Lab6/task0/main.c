#include <stdlib.h>
#include <limits.h>
#include "LineParser.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include<sys/wait.h>

#define STDOUT 1
#define STDIN 0
#define EXIT_FAILURE 1


cmdLine* user_input;
int debug_mode;
char* sendMsg = "hello\n";
char c;


int main (int argc , char* argv[], char* envp[]){
    int p[2], pid, nbytes = 6;
    if(pipe(p) < 0)
        exit(EXIT_FAILURE);
    if (!(pid = fork())) {
        close(p[0]);
        write(p[1], sendMsg, nbytes);
        close(p[1]);
    }
    else{
        close(p[1]);
        wait(NULL);
        while (read(p[0], &c, 1) > 0)
            write(STDOUT, &c, 1);
    }

    return 0;
}



