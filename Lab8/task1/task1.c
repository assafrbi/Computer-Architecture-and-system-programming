#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
} state;


struct fun_desc {
    char *name;
    void (*fun)(state* s);
};

void Toggle_Debug_Mode (state* s);
void Examine_ELF_File (state* s);
void Quit(state* s);
void Print_Section_Names(state* s);
char* typeString (uint32_t type);

int option;
char op [4];
state * s;
int Currentfd = -1;
struct stat fd_stat;
Elf32_Ehdr* header;
Elf32_Shdr* section;

void *map_start;



int main (int argc , char* argv[]){
    s = (state *) malloc (sizeof(state));
    s->unit_size = 1;
    s->debug_mode = 0;
    struct fun_desc menu[] = { { "Toggle Debug Mode", Toggle_Debug_Mode }, { "Examine ELF File", Examine_ELF_File}, { "Print Section Names", Print_Section_Names} ,{ "Quit", Quit}, { NULL, NULL}};
    int menu_size = sizeof(menu)/sizeof(menu[0]);
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
        if(option<0 || option >= menu_size-1)
            printf("\n%s\n\n","not within bounds");
        else{
        printf("%s\n","");
        void (*f) (state* s) = menu[option].fun; //save the function of the requested function 
        f(s); //execute the requested function
        printf("%s\n","");
        }
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
    char file [100];
    printf("please enter file name: ");
    fgets(file, 99, stdin);
    file[strlen(file)-1]='\0';
    strncpy(s->file_name, file, 100);
    if(s->debug_mode)
        fprintf(stderr, "Debug: file name set to %s\n", s->file_name);
    if(Currentfd != -1) // it means we already open some file
        close(Currentfd);
    Currentfd = open(file, O_RDONLY);
    if( fstat(Currentfd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }
    map_start =mmap(NULL, fd_stat.st_size, PROT_READ, MAP_SHARED, Currentfd, 0);
    header = (Elf32_Ehdr *) map_start;
    printf("\nMagic:\t\t\t\t%x %x %x\n", header-> 	e_ident[EI_MAG1], header-> 	e_ident[EI_MAG2], header-> 	e_ident[EI_MAG3]);
    if(header-> e_ident[EI_MAG1]!=0x45 || header-> e_ident[EI_MAG2]!=0x4c || header-> e_ident[EI_MAG3]!=0x46){
        fprintf(stderr, "The magic number isn't consistent with an ELF file");
        munmap(map_start, fd_stat.st_size);
        close(Currentfd);
        Currentfd = -1;
    }
    else{
    printf("Data:\t\t\t\t0x%x\n", header-> e_ident[EI_DATA]);    
    printf("Entry point address:\t\t%x\n", header-> e_entry);
    printf("Start of section headers:\t%d (bytes into file)\n", header-> e_shoff);
    printf("Number of section headers:\t%d\n", header-> e_shnum);   
    printf("Size of section headers:\t%d (bytes)\n", header-> e_shentsize);  
    printf("Start of program headers:\t%d (bytes into file)\n", header-> e_phoff);
    printf("Number of program headers:\t%d\n", header-> e_phnum);   
    printf("Size of program headers:\t%d (bytes)\n", header-> e_phentsize);   
    }
}

void Quit(state* s){
    if(s->debug_mode)
        fprintf(stderr, "quitting\n");
    munmap(map_start, fd_stat.st_size);
    free (s);
    exit(0);
}

void Print_Section_Names(state* s){
    if(Currentfd == -1)
    {
        fprintf(stderr, "please eximine a file first\n");
        return;
    }
    section = (Elf32_Shdr *)(map_start + header->e_shoff);
    int sections_num = header->e_shnum;
    char* string_table = (char*)(map_start + (section[header->e_shstrndx]).sh_offset); //points to the first name in section headers string table
    for(int i = 0 ; i < sections_num ; i++){
        printf("[%d] %s\t\t %x\t %x\t %x\t %s\t\n" , i , &string_table[section[i].sh_name] , section[i].sh_addr , section[i].sh_offset , section[i].sh_size , typeString(section->sh_type));
    }

}

char* typeString (uint32_t type){
    switch (type){
    case SHT_NULL:
        return "NULL";
    case SHT_PROGBITS:
        return "PROGBITS";
    case SHT_SYMTAB:
        return "SYMTAB";
    case SHT_STRTAB:
        return "STRTAB";
    case SHT_RELA:
        return "RELA";
    default:
        return "";
    }
}

