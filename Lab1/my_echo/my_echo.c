#include <stdio.h>
int main(int argc, char **argv) {
    FILE * input = stdin;
    char c = fgetc(input);
    while(c != EOF && c != '\n'){
        printf("%c" , c);
        c = fgetc(input);
    }
}
