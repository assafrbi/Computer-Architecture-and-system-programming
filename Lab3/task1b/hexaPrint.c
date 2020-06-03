#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <fcntl.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char* sig;
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};


void PrintHex(char* buffer, int length);
void printVirus(virus* virus, FILE* output);
virus* readVirus(FILE*);
void virusDestroy (virus *this);


int main(int argc, char **argv)
{   
    char y [5];
    int x;
    link* head = (link*)malloc(sizeof(link));
    link* node = (link*)malloc(sizeof(link));
    char sigName[128];
    FILE *file = NULL;
    while (1)
    {
        printf("%s\n%s\n%s\n", "1) Load signatures", "2) Print signatures", "3) Quit");
        printf("%s" , "Option: ");
        fgets(y, 4, stdin);
        sscanf(y,"%d",&x);
        if(x == 1)
        {
            printf("Please enter a signature file name\n");
            fgets(sigName, 127, stdin);
            size_t len = strlen(sigName);
            if (len > 0 && sigName[len - 1] == '\n')
                sigName[len - 1] = '\0'; 
            file = fopen(sigName, "rb");
            virus* corona =readVirus(file);
            head->vir = corona;
            corona = readVirus(file);
            node->vir = corona;
            head->nextVirus = node;
            while (node->vir != NULL)
            {
                corona = readVirus(file);
                link* tmp = (link*)malloc(sizeof(link));
                tmp->vir = corona;
                node->nextVirus = tmp;
                node = tmp;
            }
        }
        else if(x == 2 && file)
        {
            printVirus(head->vir, stdout);
            node = head->nextVirus;
            while (node->vir != NULL)
            {
                printVirus(node->vir, stdout);
                node = node->nextVirus;
            }
        }
        else if(x == 3)
        {
            exit(0);
        }
    }
    
    return 0;

}

void PrintHex(char* buffer, int length)
{
    for(int i = 0; i<length; i++)
    {
        unsigned char c = buffer[i];
        printf("%02x ", c);
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