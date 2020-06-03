#include "lab4_util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define SYS_LSEEK 19
#define SEEK_SET 0
#define NAME_SIZE 30
#define O_RDRW 2
#define O_WRONLY 1
#define O_CREAT 64
#define SYS_CLOSE 6

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
  char* name = argv[1];
  int file = system_call(SYS_OPEN, "greeting",O_CREAT | O_WRONLY, 0777);
  if(file < 1)
      system_call(SYS_WRITE,STDOUT,"not file",8);
  int nameToChange = system_call(SYS_LSEEK, file, 657, SEEK_SET);
  if(file < 1)
      system_call(SYS_WRITE,STDOUT,"not lseek",8);
  int bytesSend = system_call(SYS_WRITE, file, name, strlen(name));
  if(bytesSend < 1)
      system_call(SYS_WRITE,STDOUT,"not write",10);
  system_call(SYS_CLOSE, file);
  
  return 0;
}
