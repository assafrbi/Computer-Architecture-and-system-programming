#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;


struct fun_desc {
    char *name;
    void (*fun)(state* s);
};

void Toggle_Debug_Mode (state* s);
void Set_File_Name(state* s);
void Set_Unit_Size(state* s);
void Quit(state* s);

int option;
char op [4];
int debug_mode = 0;
state * s;

int main (int argc , char* argv[]){
    s = (state *) malloc (sizeof(state));
    struct fun_desc menu[] = { { "Toggle Debug Mode", Toggle_Debug_Mode }, { "Set File Name", Set_File_Name},{ "Set Unit Size", Set_Unit_Size}, { "Quit", Quit}, { NULL, NULL}};
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
        if(option>=0 && option < menu_size-1)
            printf("%s\n","within bounds");
        else{
            printf("%s\n","not within bounds");
            exit(0);
        }
        void (*f) (state* s) = menu[option].fun; //save the function of the requested function 
        f(s); //execute the requested function
        printf("%s\n","");
    }
    return 0;
}

void Toggle_Debug_Mode (state* s){
    if(!s->debug_mode){
        s->debug_mode = 1;
        fprintf(stderr,"Debug flag now on");
    }
    else{
        s->debug_mode = 0;
        fprintf(stderr,"Debug flag now off");
    }
}

void Set_File_Name(state* s){
    char file [100];
    fgets(file, 100, stdin);
    file[strlen(file)-1]='\0';
    strncpy(s->file_name, file, 100);
    if(s->debug_mode)
        fprintf(stderr, "Debug: file name set to %s\n", s->file_name);
}

void Set_Unit_Size(state* s){
    char num[4];
    int number;
    fgets(num, 4, stdin);
    sscanf(num, "%d", &number);
    if(number == 1 || number == 2 || number == 4){
        s->unit_size = number;
        if(s->debug_mode)
            fprintf(stderr, "Debug: set size to %d\n", s->unit_size);
    }
    else
        fprintf(stderr, "Error: invaild unit size\n");
}

void Quit(state* s){
    free(s);
    if(s->debug_mode)
        fprintf(stderr, "Quitting");
    exit(0);
}
