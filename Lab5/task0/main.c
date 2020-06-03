#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LineParser.h"
#include <unistd.h>
#include <signal.h>


#define MAX_LINE 2048

char path[PATH_MAX] , input[MAX_INPUT] , errorExeMsg[13] = "execv failed\n" , errorCwdMsg[13] = "getcwd error\n";
cmdLine* line;
int pid;

int execute(cmdLine *pCmdLine);

int main(int argc, char** argv){

    while(1){
        getcwd(path , PATH_MAX);
        if(getcwd(path , PATH_MAX) == NULL){
            perror(errorCwdMsg);
            exit(1);
        }
        else
            printf("Current working dir: %s\n", path);
        fgets(input , MAX_INPUT  , stdin);
        input[strlen(input) - 1] = '\0';
        if(strcmp(input , "quit") == 0 )
            exit(0);
        line = parseCmdLines(input);
        execute(line);
    }
    
    return 0;
}

int execute(cmdLine *pCmdLine){
    if(!(pid = fork())){
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror(errorExeMsg);
        exit(1); // error
    }
    return 0;
}

