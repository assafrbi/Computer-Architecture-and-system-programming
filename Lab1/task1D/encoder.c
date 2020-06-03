#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    FILE *input = stdin;
    FILE *output = stdout;
    int debugMode = 0 , eMp = 0 , eMm = 0 , index , eIndex , basic = 1;
    char c , toPrint;
    for(int i = 1 ; i < argc ; i ++)
    {
        if(strcmp(argv[i] , "-D") == 0)
        {
            debugMode = 1;
        }
        else if(strncmp(argv[i] , "+e" , 2) == 0)
        {
            eMp = 1;
            basic = 0;
            eIndex = i;
        }
        else if(strncmp(argv[i] , "-e" , 2) == 0)
        {
            eMm = 1;
            basic = 0;
            eIndex = i;
        }
        else if(strncmp(argv[i] , "-o" , 2) == 0)
        {
            output = fopen(argv[i] + 2 , "w");
        }
        else if(strncmp(argv[i] , "-i" , 2) == 0)
        {
            input = fopen(argv[i] + 2 , "r");
        }
    }
    if(debugMode)
    {
        for(int i = 1 ; i < argc ; i++)
        {
            fprintf(stderr, "%s%s", argv[i] , "\n");
        }
    }
    c = (char)fgetc(input);
    index = 2;
    while(c != EOF)
    {
        toPrint = c;
        if(basic)
        {
            if (c >= 'A' && c <= 'Z')
                toPrint = c + 32;
        }
        if(eMp)
        {
            int x = argv[eIndex][index];
            int toAdd = (int)x-64;
            toPrint = c+toAdd;
            if(strlen(argv[eIndex]) - 1 == index)
                index = 2;
            else
                index++;
        }
        if(eMm)
        {
            int x = argv[eIndex][index];
            int toAdd = (int)x-64;
            toPrint = c-toAdd;
            if(strlen(argv[eIndex]) - 1 == index)
                index = 2;
            else
                index++;
        }
        if(debugMode && c != '\n')
        {
            fprintf(stderr , "%d\t%d\n" , c , toPrint);
        }
        fprintf(output , "%c" , toPrint);
        c = (char)fgetc(input);
        if(debugMode &&  c == '\n')
            fprintf(stderr , "%c" , '\n');
        if(c == '\n' && (eMp == 1 || eMm == 1))
        {
            fprintf(output , "%c" , c);
            index = 2;
            c = (char)fgetc(input);
        }
    }
    fclose(input);
    return(0);  
}