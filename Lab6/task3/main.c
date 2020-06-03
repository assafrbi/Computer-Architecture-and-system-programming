#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include<sys/wait.h> 
#define STDOUT 1
#define STDIN 0
#define EXIT_FAILURE 1
#define MAX_ARGUMENTS 256



typedef struct link_list{
    char* name;
    char* value;
    struct link_list *next;
}link_list;


void execute(char *input_arguments[]);
void freeVariables(link_list *variables);
void add_new_set(char* name, char* value);
static char *strClone(const char *source);
int check_if_name_exist(char* name, char* value);
void print_sets();
char* get_value(const char *name);
void get_arguments(char * input);
int get_args_size(char* input);



int debug_mode , changedMode = 0 , args_count;
int pipefd[2] , fd;
link_list* varibales = NULL;
char varNotFound[22] = "variable is not found\n";
char * arguments[MAX_ARGUMENTS];
char *lsArgs[3];
char *tailArgs[4];

int main (int argc , char* argv[], char* envp[]){
    debug_mode = 0;
    if(argv[1] != NULL && strcmp(argv[1],"-d") == 0)
        debug_mode = 1;
    char pathname [PATH_MAX];
    getcwd(pathname, PATH_MAX);
    printf("~%s: ", pathname);
    char input[2048];
    args_count = 0;
    fgets(input, 2048, stdin);
    input[strlen(input) - 1] = '\0';
    get_arguments(input);

    while (1){
        for(int i = 0 ; i < args_count ; i++){
            if(strncmp(arguments[i] , "$" , 1) == 0)
                arguments[i] = get_value(arguments[i]);
        }
        if(strcmp(arguments[0],"quit")==0){
            break;
        }
        if(strcmp(arguments[0],"cd")==0){
            if(strcmp(arguments[1],"~") == 0){
                char* homePath;
                homePath = getenv("HOME");
                if(homePath)
                    arguments[1] = homePath;
            }
            int ans = chdir(arguments[1]);
            getcwd(pathname, PATH_MAX);
            if (ans == -1){
                fprintf(stderr, "%s\n", "the path doesn't exist");
                _exit(1);
            }
        }
        else if(strcmp(arguments[0],"set")==0){
            changedMode = check_if_name_exist(arguments[1], arguments[2]);
            if(!changedMode){
                add_new_set(arguments[1], arguments[2]);
            }
        }
        else if(strcmp(arguments[0],"vars")==0){
            print_sets();
        }
        else execute (arguments);
        printf("~%s: ", pathname);
        args_count = 0;
        fgets(input, 2048, stdin);
        input[strlen(input) - 1] = '\0';
        get_arguments(input);
    }
    freeVariables(varibales);
    return 0;
}

void freeVariables(link_list *varibales){
    if(varibales!=NULL){
        if(varibales->next!=NULL)
            freeVariables(varibales->next);
        free(varibales->name);
        free(varibales->value);
        free(varibales);
    }
}

void execute(char *input_arguments[]){
    int pid , status;
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if(debug_mode == 1)
        fprintf(stderr, "(parent_process>forking…)\n");
    if(!(pid = fork())){
        if(debug_mode == 1)
            fprintf(stderr, "(parent_process>created process with id: %ld)\n", (long)getpid());
        close (STDOUT);
        if(debug_mode == 1)
            fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");
        fd = dup(pipefd[1]);
        close(pipefd[1]);
        for(int i = 0 ; i < 2 ; i++){
            lsArgs[i] = arguments[i];
        }
        lsArgs[2] = NULL;
        if(debug_mode == 1)
            fprintf(stderr, "(child1>going to execute cmd: ls -l)\n");
        execvp(lsArgs[0], lsArgs);
        perror("the execute failed\n");
        _exit(1);
    }
    else{
        if(debug_mode == 1)
            fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n" );
        close(pipefd[1]);
        if(debug_mode == 1)
            fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n" );
        waitpid(pid , &status , 0);
        if(debug_mode == 1)
            fprintf(stderr, "(parent_process>forking…\n)");
        if(!(pid = fork())){
            if(debug_mode == 1)
                fprintf(stderr, "(parent_process>created process with id: %ld)\n", (long)getpid());
            if(debug_mode == 1){
                fprintf(stderr, "the pid is: %ld\n", (long)getpid());
                fprintf(stderr, "the command is: %s\n", arguments[0]);
            }
            close (STDIN);
            if(debug_mode == 1)
                fprintf(stderr, "(child1>redirecting stdin to the read end of the pipe…)\n");
            fd = dup(pipefd[0]);
            close(pipefd[0]);
            for(int i = 3 ; i < 6 ; i++){
                tailArgs[i - 3] = arguments[i];
            }
            tailArgs[3] = NULL;
            if(debug_mode == 1)
                fprintf(stderr, "(child1>going to execute cmd: tail -n 2)\n");
            execvp(tailArgs[0], tailArgs);
            perror("the execute failed\n");
            _exit(1);
        }
        else{
            if(debug_mode ==1)
                fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n" );
            close(pipefd[0]);
            if(debug_mode == 1)
                fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n" );
            waitpid(pid , &status , 0);
        }
    }
    if(debug_mode == 1)
        fprintf(stderr, "(parent_process>exiting…)\n" );
}


void add_new_set(char* _name, char* value){
    link_list* newSet = (link_list*)malloc(sizeof(link_list));
    char* name = strClone(_name);
    char* val = strClone(value);
    newSet->name = name;
    newSet->value = val;
    newSet->next = varibales;
    varibales = newSet;
}


int check_if_name_exist(char* name, char* value){
    link_list* curr = varibales;
    while (curr != NULL)
    {
        if(strcmp(curr->name, name)==0){
            char* val = strClone(value);
            free(curr->value);
            curr->value =  val;
            changedMode = 1;
            break;
        }
        curr = curr->next; 
    }
    return changedMode;
}


void print_sets(){
    link_list* curr = varibales;
    while (curr != NULL)
    {
        printf("%s , %s\n" , curr->name , curr->value);
        curr = curr->next;
    }
}

static char *strClone(const char *source) // copy from LineParser
{
    char* clone = (char*)malloc(strlen(source) + 1);
    strcpy(clone, source);
    return clone;
}


char* get_value(const char *name){
    link_list* curr = varibales;
    while (curr != NULL)
    {
        if(strcmp(curr->name, name)==0)
            return curr->value;
        curr = curr->next; 
    }
    perror("varibale doesn't exist\n");
    exit(EXIT_FAILURE);
}

void get_arguments(char * input){
    char* token = strtok(input , " ");
    int i = 0;
    while (token != NULL)
    {
        arguments[i] = token;
        i++;
        token = strtok(NULL , " ");
        args_count++;
    }
}


            
