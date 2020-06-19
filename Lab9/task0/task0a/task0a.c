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


int Currentfd = -1;
Elf32_Ehdr* header;
Elf32_Phdr* program;
struct stat fd_stat;
void *map_start;



int main (int argc , char* argv[]){
    char file [100];
    printf("please enter file name: ");
    fgets(file, 99, stdin);
    file[strlen(file)-1]='\0';
    if(Currentfd != -1) // it means we already open some file
        close(Currentfd);
    Currentfd = open(file, O_RDONLY);
    if( fstat(Currentfd, &fd_stat) != 0 ) {
      fprintf(stderr, "\nfile doesn't exist\n");
      close(Currentfd);
      exit(1);
    }
    map_start =mmap(NULL, fd_stat.st_size, PROT_READ, MAP_SHARED, Currentfd, 0);
    header = (Elf32_Ehdr *) map_start;
    if(header-> e_ident[EI_MAG1]!=0x45 || header-> e_ident[EI_MAG2]!=0x4c || header-> e_ident[EI_MAG3]!=0x46){
        fprintf(stderr, "The magic number isn't consistent with an ELF file");
        munmap(map_start, fd_stat.st_size);
        close(Currentfd);
        exit(1);
    }
    else{
        program = (Elf32_Phdr *)(map_start + header->e_phoff);// points to the program header
        int program_num = header->e_phnum;
        printf("Type\t\toffset\t VirtAddr   PhysAddr   FileSiz    MemSiz    flg\t      Align\n");
        for (int i = 0; i<program_num; i++){
            printf("%010d\t0x%06x 0x%08x 0x%08x 0x%06x   0x%06x  0x%06x  0x%06x\n",program[i].p_type, program[i].p_offset, program[i].p_vaddr, program[i].p_paddr, program[i].p_filesz, program[i].p_memsz, program[i].p_flags, program[i].p_align);
        }
    }
    return 0;
}

