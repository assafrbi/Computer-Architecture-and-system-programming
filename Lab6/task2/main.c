#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "LineParser.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include<sys/wait.h>
#define STDOUT 1
#define STDIN 0
#define EXIT_FAILURE 1

//functions declaration
void execute(cmdLine *pCmdLine);
void add_new_set(cmdLine* user_input);
static char *strClone(const char *source);

typedef struct link_list{
    char* name;
    char* value;
    struct link_list *next;
}link_list;

cmdLine* user_input;
int debug_mode , changedMode = 0;
link_list* varibales = NULL;
char varNotFound[25] = "variable %s is not found\n";
char input[2048];


int check_if_name_exist(cmdLine* user_input);
void print_sets();
void freeVariables(link_list *variables);
char* get_value(const char *name);


int main (int argc , char* argv[], char* envp[]){
    debug_mode = 0;
    if(argv[1] != NULL && strcmp(argv[1],"-d") == 0)
        debug_mode = 1;
    char pathname [PATH_MAX];
    getcwd(pathname, PATH_MAX);
    printf("~%s: ", pathname);
    fgets(input, 2048, stdin);
    user_input = parseCmdLines(input);
    input[strlen(input)-1] = '\0';
    while (1){
        for(int i = 0 ; i < user_input->argCount ; i++)
        {
            if(strncmp(user_input->arguments[i] , "$" , 1) == 0){
                char tmp [2048];
                for(int j = 0; j<i; i++){
                    strcat(tmp, user_input->arguments[j]);
                    strcat(tmp, " ");
                }
                strcat(tmp, get_value(user_input->arguments[i] + 1));
                strcat(tmp, " ");
                for(int j = i+1; j<user_input->argCount; i++){
                    strcat(tmp, " ");
                    strcat(tmp, user_input->arguments[j]);
                }
                freeCmdLines(user_input);
                tmp[strlen(tmp)-1]='\0';
                user_input = parseCmdLines(tmp);
                for(int i =0; i<strlen(tmp); i++)
                    tmp[i] = '\0';
            }
        }
        if(strcmp(user_input->arguments[0],"quit")==0){
            freeCmdLines(user_input);
            break;
        }
        if(strcmp(user_input->arguments[0],"cd")==0){
            if(strcmp(user_input->arguments[1],"~") == 0){
                char* homePath;
                homePath = getenv("HOME");
                if(homePath)
                    replaceCmdArg(user_input , 1 , homePath);
            }
            int ans = chdir(user_input->arguments[1]);
            getcwd(pathname, PATH_MAX);
            if (ans == -1){
                freeCmdLines(user_input);
                fprintf(stderr, "%s\n", "the path doesn't exist");
                _exit(1);
            }
        }
        else if(strcmp(user_input->arguments[0],"set")==0){
            changedMode = check_if_name_exist(user_input);
            if(!changedMode){
                add_new_set(user_input);
            }
        }
        else if(strcmp(user_input->arguments[0],"vars")==0){
            print_sets();
        }
        else execute (user_input);
        freeCmdLines(user_input);
        printf("~%s: ", pathname);
        fgets(input, 2048, stdin);
        input[strlen(input)-1] = '\0';
        user_input = parseCmdLines(input);
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

void execute(cmdLine *pCmdLine){
    int pid;
    int status;
    if(!(pid = fork())){
        if(debug_mode == 1){
            fprintf(stderr, "the pid is: %ld\n", (long)getpid());
            fprintf(stderr, "the command is: %s\n", pCmdLine->arguments[0]);
        }
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("the execute failed\n");
        _exit(1);
    }
    if(pCmdLine->blocking)
        waitpid(pid, &status, 0);
}


void add_new_set(cmdLine* user_input){
    link_list* newSet = (link_list*)malloc(sizeof(link_list));
    char* name = strClone(user_input->arguments[1]);
    char* val = strClone(input+5+strlen(user_input->arguments[1]));
    newSet->name = name;
    newSet->value = val;
    newSet->next = varibales;
    varibales = newSet;
}


int check_if_name_exist(cmdLine* user_input){
    link_list* curr = varibales;
    while (curr != NULL)
    {
        if(strcmp(curr->name, user_input->arguments[1])==0){
            char* val = strClone(input+5+strlen(user_input->arguments[1]));
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
    fprintf(stderr , varNotFound , name);
    exit(EXIT_FAILURE);
}