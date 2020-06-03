#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
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
      return dprt(c);
  else
  {
      printf("%c" , '.');
      return c;
  }
}

char my_get(char c){
  FILE *input = stdin;
  char res = (char)fgetc(input);
  return res;
}

char quit(char c){
  if(c =='q')
    exit(0);
  return c;
}

 
void map(char *array, int array_length, char (*f) (char)){
  for(int i = 0 ; i < array_length ; i++)
  {
      array[i] = f(array[i]);
  }
}
 
int main(int argc, char **argv){
    char arr1[] = {'H','E','G','q'};
    map(*&arr1, 4, quit);
    printf("\n%s\n", arr1);
}