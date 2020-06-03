#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int digit_cnt (char * str);

int main (int argc , char* argv[]){
    printf ("The number of digits in the string is: %d\n" , digit_cnt (argv[1]));
    return 0;
}

int digit_cnt (char * str){
    int counter = 0 ;
    int i = 0;
    while(str[i] != '\0'){
        if (str[i] >= '0' && str[i] <= '9')
            counter++;
        i++;
    }
    return counter;
}
