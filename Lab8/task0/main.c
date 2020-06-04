
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <elf.h>
#include <stdbool.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  char display_mode;
} state;


struct fun_desc {
    char *name;
    void (*fun)(state* s);
};

void Toggle_Debug_Mode (state* s);
void Examine_ELF_File (state* s);
void Quit(state* s);
void close_currentFD();

struct fun_desc menu[] = { { "Toggle Debug Mode", Toggle_Debug_Mode }, { "Examine ELF File", Examine_ELF_File},{ "Quit", Quit}, { NULL, NULL}};
int menu_size = sizeof(menu)/sizeof(menu[0]) , debug_mode = 0;
int  option , currnt_fd = -1;
state * s;
FILE* fp;
int option;
char op [4];
struct stat fd_stat; /* this is needed to  the size of the file */
void *map_start; /* will point to the start of the memory mapped file */
Elf32_Ehdr *header; /* this will point to the header structure */


int main (int argc , char* argv[]){
    s = (state *) malloc (sizeof(state));
    s->unit_size = 1;
    s->debug_mode = 0;
    s->display_mode = 0;
    while(true){
        if(s->debug_mode){
            fprintf(stderr,"unit size is: %d\nfile name is: %s\nmem count is: %d\n\n",s->unit_size,  s->file_name, s-> mem_count); 
        }
        printf("%s\n","please choose an action: ");
        for(int j = 0 ; j<menu_size-1; j++){
            printf("%d",j);
            printf(") %s\n",menu[j].name);
        }
        
        printf("%s","option: ");
        fgets(op, 4 ,stdin);
        sscanf(op, "%d", &option);
        if(option>=0 && option < menu_size-1)
            printf("%s\n","within bounds");
        else{
            printf("%s\n","not within bounds");
            exit(0);
        }
        menu[option].fun(s);
        printf("\n");
    }
    return 0;
}

void Toggle_Debug_Mode (state* s){
    if(s->debug_mode == 0){
        s->debug_mode = 1;
        fprintf(stderr,"Debug flag now on");
    }
    else{
        s->debug_mode = 0;
        fprintf(stderr,"Debug flag now off");
    }
}

void Examine_ELF_File (state* s){
    char fileName[10000];
    printf("Please enter ELF file name: ");
    fgets(fileName , 99999 , stdin);
    fileName[strlen(fileName)-1]='\0';
    close_currentFD();
    if((currnt_fd = open(fileName , O_RDONLY)) == -1){
        perror("Can't open the file");
        return;
    }
    if(fstat(currnt_fd, &fd_stat) != 0) {
        perror("Error stat failed");
        close_currentFD();
        return;
    }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ, MAP_SHARED, currnt_fd, 0)) == MAP_FAILED) {
        perror("Error mmap failed");
        close_currentFD();
        return;
    }
    header = (Elf32_Ehdr *) map_start;
    printf("The magic numbers are:\t\t\t%c %c %c\n",header->e_ident[EI_MAG1],header->e_ident[EI_MAG2],header->e_ident[EI_MAG3]);
    if(!(header->e_ident[EI_MAG0] == ELFMAG0 && header->e_ident[EI_MAG1] == ELFMAG1 && header->e_ident[EI_MAG2] == ELFMAG2 && header->e_ident[EI_MAG3] == ELFMAG3)){
        perror("Error not an ELF file");
        munmap(map_start, fd_stat.st_size);
        close_currentFD();
        return;
    }
    if(header->e_ident[EI_DATA] == ELFDATA2LSB) 
        printf("The encoding scheme:\t\t\t2's complement, little endian\n");
    else if(header->e_ident[EI_DATA] == ELFDATA2MSB) 
        printf("2's complement, big endian\n");
    else printf("Invalid data encoding\n");
    printf("The entry point is:\t\t\t%p\n", (void*)header->e_entry);
    printf("The offset of section header table: \t%d\n", header->e_shoff);
    printf("The number of section header entries: \t%d\n",header->e_shnum);
    printf("The size of section header:\t\t%d\n",header->e_shentsize);
    printf("The offset of program header table:\t%d\n", header->e_phoff);
    printf("The number of program header entries:\t%d\n", header->e_phnum);
    printf("The size of each program header entry:  %d\n", header->e_phentsize);
}

void Quit(state* s) {
    munmap(map_start, fd_stat.st_size);
    close_currentFD();
	exit(0);
}

void close_currentFD(){
  if(currnt_fd != -1){
    close(currnt_fd);
    currnt_fd = -1;
  }
}
