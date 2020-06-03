#include "util.h"

#define STDIN 0
#define STDOUT 1
#define SYS_EXIT 1
#define STDERR 2
#define O_RDRW 2
#define READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_LSEEK 19
#define O_WRONLY 1
#define O_CREAT 64
#define SYS_CLOSE 6
#define BUF_SIZE 8192
#define SYS_getdents 141
#define O_APPEND 1024
#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12


typedef struct ent
{
    int inode;
    int offset;
    short len;
    char buf[1];
}ent;


extern int system_call();
void print_type (char d_type , ent* d);
void print_debug_mode (int first_arg , int inputFile);
void print_files_names(int nread, char buf[], int save_pfix, char* argv[], char d_type, ent* d);
void exit_program(char* msg, int status);


int i, debugMode = 0, basic = 1, iFileMode = 0, oFileMode = 0 , returnValue, save_pfix = 0, save_afix = 0 , a_fix_mode = 0;
int inputFile = STDIN , outputFile = STDOUT, p_fix_mode = 0,fd = 0, nread, saveImode, saveOmode;
char c, buf[BUF_SIZE], d_type, dirExitMsg[22] = "Can not open directory" , getdentesExitMsg[27] = "Can not read with getdents\n";
char* retVal;
char outputExitMsg[24] = "Can not open output file", inputExitMsg[23] = "Can not open input file";
ent *d;
char* first_char_arg;


int main (int argc , char* argv[], char* envp[])
{
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
                exit_program(inputExitMsg, inputFile);
            saveImode = i;
            if(debugMode)
                print_debug_mode(SYS_OPEN , inputFile);
        }   
        else if(strncmp(argv[i] , "-o", 2) == 0)
        {
            outputFile = system_call(SYS_OPEN, argv[i] + 2, O_CREAT | O_RDRW, 0777);
            if(outputFile < 1)
                exit_program(outputExitMsg, outputFile);
            saveOmode = i;
            if(debugMode)
                print_debug_mode(SYS_OPEN , outputFile);
        }
        else if(strncmp(argv[i] , "-p", 2) == 0){
            p_fix_mode = 1;
            save_pfix = i;
            fd = system_call(SYS_OPEN,".", 0, 0777);
            if(fd < 1)
                exit_program(dirExitMsg, outputFile);
            if (debugMode)
                print_debug_mode(SYS_OPEN , fd);
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
    if(p_fix_mode || a_fix_mode)
    {
        nread = system_call(SYS_getdents, fd, buf, BUF_SIZE);
        print_files_names(nread, buf, save_pfix, argv, d_type, d);
        if(nread < 1)
            exit_program(getdentesExitMsg , nread);
        if(debugMode)
            print_debug_mode(SYS_getdents , nread);
        returnValue = system_call(SYS_CLOSE, fd);
        if(debugMode)
            print_debug_mode(SYS_CLOSE , returnValue);
    }
    return 0;
}


void print_type (char d_type , ent* d)
{
    if(d_type == DT_REG)
        system_call(SYS_WRITE, STDOUT, "regular\n\n", 9);
    if(d_type == DT_DIR)
        system_call(SYS_WRITE, STDOUT, "directory\n\n", 11);
    if(d_type == DT_FIFO)
        system_call(SYS_WRITE, STDOUT, "FIFO\n\n", 6);
    if(d_type == DT_SOCK)
        system_call(SYS_WRITE, STDOUT, "socket\n\n", 8);
    if(d_type == DT_LNK)
        system_call(SYS_WRITE, STDOUT, "symlink\n\n", 9);
    if(d_type == DT_BLK)
        system_call(SYS_WRITE, STDOUT, "block dev\n\n", 11);
    if(d_type == DT_CHR)
        system_call(SYS_WRITE, STDOUT, "char dev\n\n", 10);
}


void print_debug_mode (int first_arg , int inputFile){
    retVal = itoa(inputFile);
    first_char_arg = itoa(first_arg);
    system_call(SYS_WRITE, STDERR, "system_call's first arg: ", 25);
    system_call(SYS_WRITE, STDERR, first_char_arg, strlen(first_char_arg));
    system_call(SYS_WRITE, STDERR, "\n", 1);
    system_call(SYS_WRITE, STDERR, "system_call's return value: ", 28);
    system_call(SYS_WRITE, STDERR, retVal, 1);
    system_call(SYS_WRITE, STDERR, "\n", 1);
}


void print_files_names(int nread, char buf[], int save_pfix, char* argv[], char d_type, ent* d){
    system_call(SYS_WRITE, STDOUT, "Flame 2 strikes!\n\n", 18);
    for(i = 0 ; i < nread;)
    {
        d = (ent*)(buf + i);
        if(d->inode != 0 && strncmp(argv[save_pfix] + 2 , d->buf , strlen(argv[save_pfix] + 2)) == 0)
        {
            system_call(SYS_WRITE, STDOUT, "file name is: ", 14);
            system_call(SYS_WRITE, STDOUT, d->buf, strlen(d->buf));
            system_call(SYS_WRITE, STDOUT,"\n", 1);
            system_call(SYS_WRITE, STDOUT, "file's d_type is: ", 19);
            d_type = *(buf + i + d->len - 1);
            print_type (d_type ,  d);
        }
        i += d->len;
    }
}


void exit_program(char* msg, int status){
    system_call(SYS_WRITE, STDERR, msg, strlen(msg));
    system_call(SYS_EXIT, 0x55);
}
