#include "lab4_util.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define O_RDRW 2
#define READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define O_WRONLY 1
#define O_CREAT 64
#define SYS_CLOSE 6
#define SYS_EXIT 1



int i, debugMode = 0, basic = 1, iFileMode = 0, oFileMode = 0 , saveImode , saveOmode;
char c;
char* retVal;


extern int system_call();
void exit_program(char* msg, int status);


void print_debug_mode (char* first_arg , int inputFile);

int main (int argc , char* argv[], char* envp[])
{
    int inputFile = STDIN , outputFile = STDOUT;
    for(i = 1 ; i < argc ; i ++)
    {
        if(strcmp(argv[i] , "-D") == 0)
            debugMode = 1;
    }
    for(i = 1 ; i < argc ; i++)
    {
        if (strncmp(argv[i] , "-i" , 2) == 0)
        {
            inputFile = system_call(SYS_OPEN, argv[i] + 2, O_CREAT | O_RDRW, 0777);
            if(inputFile < 1)
                exit_program("Can not open input file", inputFile);
            iFileMode = 1;
            saveImode = i;
            if(debugMode)
                print_debug_mode("5" , inputFile);
            
        }   
        else if(strncmp(argv[i] , "-o", 2) == 0)
        {
            outputFile = system_call(SYS_OPEN, argv[i] + 2, O_CREAT | O_WRONLY, 0777);
            if(outputFile < 1)
                exit_program("Can not open output file", outputFile);
            oFileMode = 1;
            saveOmode = i;
            if(debugMode)
                print_debug_mode("5" , outputFile);
        }        
    }
    if(debugMode)
    {
        for(i = 1 ; i < argc ; i++)
        {
            system_call(SYS_WRITE,STDERR,argv[i],strlen(argv[i]));
            system_call(SYS_WRITE,STDERR,"\n",1);
        }
        system_call(SYS_WRITE,STDERR,"input file: ",12);
        iFileMode ? system_call(SYS_WRITE, STDERR, argv[saveImode] + 2, strlen(argv[saveImode] + 2)) : 
        system_call(SYS_WRITE, STDERR, "stdin", 5);
        system_call(SYS_WRITE,STDERR,"\n",1);
        system_call(SYS_WRITE,STDERR,"output file: ",13);
        oFileMode ? system_call(SYS_WRITE, STDERR, argv[saveOmode] + 2, strlen(argv[saveOmode] + 2)) : 
        system_call(SYS_WRITE, STDERR, "stdout", 6);
        system_call(SYS_WRITE,STDERR,"\n",1);
    }
    while (system_call(READ, inputFile, &c, 1) > 0)
    {
        char toPrint = c;
        if(basic)
        {
            if (c >= 'a' && c <= 'z')
                toPrint = c - 32;
        }
        int return_val = system_call(SYS_WRITE, outputFile, &toPrint, 1);
        if(return_val < 1)
            exit_program("Can not write to output file", return_val);
        if(debugMode)
            print_debug_mode("4" , return_val);
    }
    return 0;
}

void print_debug_mode (char *first_arg , int inputFile){
    retVal = itoa(inputFile);
    system_call(SYS_WRITE, STDERR, "system_call's first arg: ", 25);
    system_call(SYS_WRITE, STDERR, first_arg, strlen(first_arg));
    system_call(SYS_WRITE, STDERR, "\n", 1);
    system_call(SYS_WRITE, STDERR, "system_call's return value: ", 28);
    system_call(SYS_WRITE, STDERR, retVal, 1);
    system_call(SYS_WRITE, STDERR, "\n", 1);
}


void exit_program(char* msg, int status){
    system_call(SYS_WRITE, STDERR, msg, strlen(msg));
    system_call(SYS_EXIT, 0x55);
}
