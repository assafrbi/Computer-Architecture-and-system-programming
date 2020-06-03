#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char* sig;
} virus;

void PrintHex(char* buffer, int length);
void printVirus(virus* virus, FILE* output);
virus* readVirus(FILE*);
void virusDestroy (virus *this);

int main(int argc, char **argv)
{
    FILE *file;
    file = fopen(argv[1], "rb");
    virus* corona =readVirus(file);
    while (corona != NULL)
    {
        printVirus(corona, stdout);
        corona = readVirus(file);
    }
    fclose(file);
    return 0;

}

void PrintHex(char* buffer, int length)
{
    for(int i = 0; i<length; i++)
    {
        unsigned char c = buffer[i];
        printf("%x ", c);
    }
    printf("%c", '\n');
}


virus* readVirus(FILE* file){

    virus* toReturn = malloc(sizeof(virus));
    unsigned char buffer1[18];
    size_t n = fread(buffer1,1,sizeof(buffer1),file);
    size_t y = sizeof(buffer1);
    if(n != y)
    {
        free(toReturn);
        return NULL;
    }
    toReturn->SigSize = buffer1[0] + (buffer1[1] << 8);
    for(int i = 2 ; i < 18 ; i++)
    {
        toReturn->virusName[i-2] = buffer1[i];
    }
    toReturn->sig = malloc(toReturn->SigSize*sizeof(char));
    n = fread(toReturn->sig,sizeof(toReturn->sig[0]),toReturn->SigSize,file);
    size_t z = toReturn->SigSize*sizeof(toReturn->sig[0]);
    if(n < z)
    {
        virusDestroy(toReturn);
        return NULL;
    }
    return toReturn;
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "%s%s\n","Virus name: " ,virus->virusName);
    fprintf(output, "%s%d\n","Virus size: ", virus->SigSize);
    printf("%s\n", "signature:");
    PrintHex(virus->sig, virus->SigSize);
    virusDestroy(virus); 
}

void virusDestroy (virus *this){
    free(this->sig);
    free(this);
}