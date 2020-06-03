#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char* sig;
} virus;

struct fun_desc {
  char *name;
  void (*fun)();
};

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

virus* readVirus(FILE* file);
void PrintHex(char* buffer, int length);
void printVirus(virus* virus, FILE* output);
void list_print(link *virus_list, FILE*);
link* list_append(link* virus_list, virus* data);
void list_free(link *virus_list);
void load_help();
void printHelp();
void detectHelp();
void quit();
void detect_virus(char *buffer, unsigned int size, link *virus_list);
void help_killVirus();
void kill_virus(char *fileName, int signitureOffset, int signitureSize);


char argv1[128];
link* head = NULL;
FILE *file = NULL;
int loaded = 0;
struct fun_desc menu[] = { { "Load signatures", load_help}, { "Print signatures", printHelp},{ "Detect viruses", detectHelp},{"Fix file", help_killVirus},{"Quit", quit}, { NULL, NULL } };

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        for(int i = 0 ; i < strlen(argv[1]) ; i++)
        {
            argv1[i] = argv[1][i];
        }
        argv1[strlen(argv[1])] = '\0';
    }      
    char option [5];
    int x;
    int len = sizeof(menu)/sizeof(struct fun_desc);
    int i = 1;
    while (1)
    {
        printf("%c" , '\n');
        while (i < len)
        {
            printf("%d) %s\n" , i , menu[i-1].name);
            i++;
        }
        i = 1;
        printf("%s" , "Option: ");
        fgets(option, 4, stdin);
        sscanf(option,"%d",&x);
        x = x - 1;
        menu[x].fun();
    }
    return 0;
}

virus* readVirus(FILE* file){

    virus* toReturn = malloc(sizeof(virus));
    unsigned char buffer1[18];
    size_t n = fread(buffer1,1,sizeof(buffer1),file);
    size_t y = sizeof(buffer1);
    if(n < y)
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
        return NULL;
    return toReturn;
}


void PrintHex(char* buffer, int length)
{
    for(int i = 0; i<length; i++)
    {
        unsigned char c = buffer[i];
        printf("%02hX ", c);
    }
    printf("%c%c", '\n', '\n');
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "%s%s\n","Virus name: " ,virus->virusName);
    fprintf(output, "%s%d\n","Virus size: ", virus->SigSize);
    printf("%s\n", "signature:");
    PrintHex(virus->sig, virus->SigSize);
}


void list_print(link *virus_list, FILE* output){
    fprintf(output, "%s%s\n","Virus name: " ,virus_list->vir->virusName);
    fprintf(output, "%s%d\n","Virus size: ", virus_list->vir->SigSize);
    printf("%s\n", "signature:");
    PrintHex(virus_list->vir->sig, virus_list->vir->SigSize);
}


link* list_append(link* virus_list, virus* data){
    link* newLink = (link*)malloc(sizeof(link));
    newLink->vir = data;
    if(virus_list == NULL)
    {
        virus_list = newLink;
        return virus_list;
    }
    virus_list->nextVirus = newLink;
    return newLink;
}


void list_free(link *virus_list){
    free(virus_list->vir->sig);
    free(virus_list->vir);
    free(virus_list);
}


void load_help(){
    char sigName[128];
    printf("Please enter a signature file name\n");
    fgets(sigName, 128, stdin);
    size_t len = strlen(sigName);
    if (len > 0 && sigName[len - 1] == '\n')
        sigName[len - 1] = '\0'; 
    file = fopen(sigName, "rb");
    virus* corona = readVirus(file);
    link* tmp = NULL;
    while (corona != NULL)
    {
        tmp = list_append(tmp, corona);
        if(head == NULL)
            head = tmp;
        corona =readVirus(file);
    }
    tmp->nextVirus = NULL;
    loaded = 1;
    fclose(file); 
}


void printHelp(){
    list_print(head, stdout);
    link *node = head->nextVirus;
    while (node != NULL)
    {
        list_print(node, stdout);
        node = node->nextVirus;
    }
}


void detectHelp(){
    char* buffer = calloc(10000, sizeof(char));
    file = fopen(argv1, "rb");
    fseek(file,0L,SEEK_END);
    long int sizeOfFile =ftell(file);
    rewind(file);
    int size = fread(buffer,sizeof(buffer[0]),sizeOfFile,file);
    detect_virus(buffer, size, head);
    free(buffer);
    fclose(file);
}


void quit(){
    link* tmp = head;
    while (tmp != NULL)
    {
        tmp = head->nextVirus;
        list_free(head);
        head = tmp;
    }
    exit(0);
}


void detect_virus(char *buffer, unsigned int size, link *virus_list){
    int res = 0;
    int i = 0;
    while (virus_list != NULL)
    {
        while (i < size)
        {
            res = memcmp(&buffer[i], &virus_list->vir->sig[0], virus_list->vir->SigSize);
            if(res == 0){
                printf("%s%d\n", "The starting byte location in the suspected file is: ",i);
                printf("%s%s\n", "Virus name: ", virus_list->vir->virusName);
                printf("%s%d\n\n", "Virus size: ", virus_list->vir->SigSize);
                i = size;
            }
            i = i + 1;
            res = 0;
        }
        virus_list = virus_list->nextVirus;
        i = 0;
    }

}


void help_killVirus()
{
    char inputStart[100];
    char inputSize[100];
    int startByte = 0, sigSize = 0;
    printf("Please enter the starting byte location in the suspected file:\n");
    fgets(inputStart, 99, stdin);
    sscanf(inputStart, "%d", &startByte);
    printf("Please enter the size of the virus signature:\n");
    fgets(inputSize, 99, stdin);
    sscanf(inputSize, "%d", &sigSize);
    kill_virus(argv1, startByte, sigSize);
}


void kill_virus(char *fileName, int signitureOffset, int signitureSize){
    FILE* infected = fopen(fileName, "r+");
    char tobeWritten[signitureSize];
    for(int i = 0 ; i < signitureSize; i++)
    {
        tobeWritten[i] = '\x90';
    }
    fseek(infected, signitureOffset, SEEK_SET);
    fwrite(tobeWritten, 1, signitureSize, infected);
    fclose(infected);
}
