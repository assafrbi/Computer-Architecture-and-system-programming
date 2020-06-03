#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LineParser.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0
#define MAX_LINE 2048


typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                      /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	              /* next process in chain */
} process;


char path[PATH_MAX] , input[MAX_INPUT] , errorExeMsg[13] = "execv failed\n" , errorCwdMsg[13] = "getcwd error\n";
char chdirError[12] = "Chdir error\n" , terminated[10] = "Terminated" , running[7] = "Running" , suspended[9] = "Suspended";
char* status_char;
cmdLine* line;
int debug_mode = 0 , ret_chdir , status , status_ret , ret_waitpid1 = 0 , ret_waitpid2 = 0 , terminatedMode = 0 , procsMode = 0;
int sig;
process* processList = NULL;
pid_t pid;


int execute(cmdLine *pCmdLine);
void debug_info (int pid , char* exe_command);
void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
void freeTerminatedProcess(process* process_list);
void updateProcessList(process **process_list);
void updateProcessStatus(process* process_list, int pid, int status);
void freeProcessList(process* process_list);


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
            freeProcessList(processList);
            exit(0);
        }
        else if(strncmp(input , "cd" , 2) == 0){
            ret_chdir = chdir(input + 3);
            if(ret_chdir != 0){
                perror(chdirError);
                exit(1);
            }
        }
        else if(strncmp(input , "procs" , 5) == 0 ){
            printProcessList(&processList);
        }
        else if(strncmp(input , "kill" , 4) == 0){
            sscanf(input + 5 , "%d" , &sig);
            kill(sig , SIGINT);
        }
        else if(strncmp(input , "suspend" , 7) == 0){
            sscanf(input + 8 , "%d" , &sig);
            kill(sig , SIGTSTP);
        }
        else if(strncmp(input , "wake" , 4) == 0){
            sscanf(input + 5 , "%d" , &sig);
            kill(sig , SIGCONT);
        }
        else{
            line = parseCmdLines(input);
            execute(line);
        }
    }
    return 0;
}


int execute(cmdLine *pCmdLine){
    if(!(pid = fork())){
        if(debug_mode)
            debug_info(pid , pCmdLine->arguments[0]);            
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror(errorExeMsg);    
        exit(EXIT_FAILURE); // error
    }
    else if(pCmdLine->blocking)
        waitpid(pid , &status , 0);
    addProcess(&processList , pCmdLine , pid);
    return 0;
}


void debug_info (int pid , char* exe_command){
    fprintf(stderr , "PID: %d\n" , pid);
    fprintf(stderr , "Executing command: %s\n" , exe_command);
}


void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process* newProcess = (process*)malloc(sizeof(process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->status = ret_waitpid1;
    newProcess->next = *process_list;
    *process_list = newProcess; 
}


void printProcessList(process** process_list){
    updateProcessList(process_list);
    process* tmp = (*process_list);
    while (tmp != NULL)
    {
        status = tmp->status;
        if(status == -1){
            status_char = terminated;
            terminatedMode = 1;
        }
        else if(status == 1)
            status_char = running;
        else if(status == 0)
            status_char = suspended;
        printf("PID         Command         STATUS\n");
        printf("%d          %s          %s\n\n" , tmp->pid , tmp->cmd->arguments[0], status_char);
        process* tmpToDelete = tmp;
        tmp = tmp->next;
        if(terminatedMode)
        {
            freeTerminatedProcess(tmpToDelete);
            terminatedMode = 0;
        }
    }
}


void freeTerminatedProcess(process* process_list){
    process* tmp = processList;
    if(processList->pid == process_list->pid)
        processList = processList->next;
    else{
        while (tmp->next != NULL && tmp->next->pid != process_list->pid)
            tmp = tmp->next;
        tmp->next = process_list->next;
    }
    freeCmdLines(process_list->cmd);
    free(process_list);
}


void updateProcessList(process **process_list){
    process *curr = *process_list;
    while (curr != NULL)
    {
        ret_waitpid2 = waitpid(curr->pid , &status , WNOHANG | WUNTRACED);
        if(ret_waitpid2 == -1){
            if(errno == ECHILD) 
                updateProcessStatus(curr , curr->pid , TERMINATED);
            else{
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        }
         if(ret_waitpid2 == 0)
            updateProcessStatus(curr , curr->pid , RUNNING);
        else if(WIFEXITED(status)) //process exited
            updateProcessStatus(curr , curr->pid , TERMINATED);
        else if(WIFSIGNALED(status)) //process was killed by a signal
            updateProcessStatus(curr , curr->pid , TERMINATED);
        else if(WIFSTOPPED(status)) //process was stopped by a signal
            updateProcessStatus(curr , curr->pid , SUSPENDED);
        else if(WIFCONTINUED(status)) //process continue
            updateProcessStatus(curr , curr->pid , RUNNING);
        curr = curr->next;
    }
}


void updateProcessStatus(process* process_list, int pid, int status){
    process_list->status = status;
}


void freeProcessList(process* process_list){
    process* tmp;
    while (process_list != NULL)
    {
        tmp = process_list;
        process_list = process_list->next;
        freeCmdLines(tmp->cmd);
        free(tmp);
    }
}
