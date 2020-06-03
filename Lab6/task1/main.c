#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LineParser.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>


#define STDIN 0
#define STDOUT 1
#define MAX_LINE 2048

char path[PATH_MAX] , input[MAX_INPUT] , errorExeMsg[13] = "execv failed\n" , errorCwdMsg[13] = "getcwd error\n" , errorExecvp[13] = "execvp error\n";
char chdirError[12] = "Chdir error\n";
char buf;
int pid, debug_mode = 0 , ret_chdir , status , inputFile , outputFile;
int pipefd[2];
cmdLine* line;



int execute(cmdLine *pCmdLine);
void debug_info (int pid , char* exe_command);

int main(int argc, char* argv[]){
    if(argc > 1 && strcmp(argv[1] , "-D") == 0)
        debug_mode = 1;
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
        if(strcmp(input , "quit") == 0 ){
            exit(0);
        }     
        line = parseCmdLines(input);  
        if(strncmp(line->arguments[0] , "cd" , 2) == 0){
            ret_chdir = chdir(line->arguments[1]);
            if(ret_chdir != 0){
                perror(chdirError);
                exit(1);
            }
            freeCmdLines(line);
        }
        else
            execute(line);
    }
    return 0;
}


int execute(cmdLine *pCmdLine){
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if(!(pid = fork())){
        if(debug_mode)
            debug_info(getpid() , pCmdLine->arguments[0]);
        if(pCmdLine->inputRedirect){
            close(STDIN);            
            inputFile = open(pCmdLine->inputRedirect, O_RDONLY , S_IRWXU);
        }
        if(pCmdLine->outputRedirect){
            close (STDOUT);
            outputFile = open(pCmdLine->outputRedirect, O_CREAT | O_RDWR , S_IRWXU) ;
        }
        if(execvp(pCmdLine->arguments[0],pCmdLine->arguments) == -1) {
            perror(errorExecvp);
            _exit(EXIT_FAILURE);
        }
    }
    else if(pCmdLine->blocking)
        waitpid(pid , &status , 0);
    freeCmdLines(pCmdLine);
    return 0;
}

void debug_info (int pid , char* exe_command){
    fprintf(stderr , "Process ID: %d\n" , pid);
    fprintf(stderr , "command line: %s\n" , exe_command);
}
