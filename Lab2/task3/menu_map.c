#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int i = 0;
int x;
char* carray;
char censor(char c);
char encrypt(char c);
char decrypt(char c);
char dprt(char c);
char cprt(char c);
char my_get(char c);
char quit(char c);

struct fun_desc {
  char *name;
  char (*fun)(char);
};

struct fun_desc menu[] = { { "censor", censor }, { "encrypt", encrypt }, { "decrypt", decrypt }, { "Print dec", dprt }, { "Print string", cprt }, { "Get string   ", my_get }, { "Quit", quit }, { NULL, NULL } };


char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char encrypt(char c){
  if(c >= ' ' && c <= '~')
      return (c+3);
  else
      return c;
}

char decrypt(char c){
  if(c >= ' ' && c <= '~')
      return (c-3);
  else
      return c;
}

char dprt(char c){
  printf("%d%c" , c , '\n' );
  return c;
}

char cprt(char c){
  if(c >= ' ' && c <= '~')
      printf("%c\n" , c);
  else
  {
      printf("%c" , '.');
  }
  return c;
}

char my_get(char c){
  FILE *input = stdin;
  char res = (char)fgetc(input);
  if(res == '\n')
      res = (char)fgetc(input);
  return res;
}

char quit(char c){
  if(c =='q')
  {
    free(carray);
    exit(0);
  }
  return c;
}
 
void map(char *array, int array_length, char (*f) (char)){
  for(int i = 0 ; i < array_length ; i++)
  {
      array[i] = f(array[i]);
  }
}


int main(int argc, char **argv){
    char carray[5];
    carray[0] = '\0';
    while (1)
    { 
      printf("%s" , "Please choose a function:\n");
      while (menu[i].name != NULL)
      {
        printf("%d) %s\n" , i , menu[i].name);
        i++;
      }
      printf("%s" , "Option: ");
      scanf("%d", &x);
      if(x >= 0 && x <= i-1)
      {
        printf("%s\n" , "Within bounds");
        map(*&carray, 4, menu[x].fun);
      }
      else
        printf("%s\n" , "Not within bounds");
      i = 0;
      printf("%s\n\n", "DONE");
    }
}
