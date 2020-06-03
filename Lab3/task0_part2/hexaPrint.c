#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void PrintHex(unsigned char* buffer, int length);
int main(int argc, char **argv)
{
    FILE *file;
    file = fopen(argv[1], "rb");
    fseek(file,0L,SEEK_END);
    long int sizeOfFile =ftell(file);
    unsigned char buffer[sizeOfFile];
    rewind(file);
    fread(buffer,sizeof(buffer),1,file);
    PrintHex(buffer, sizeOfFile);

    return 0;

}

void PrintHex(unsigned char* buffer, int length)
{
    for(int i = 0; i<length; i++)
        printf("%x ", buffer[i]);
}