#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

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
void Set_File_Name(state* s);
void Set_Unit_Size(state* s);
void Quit(state* s);
void Load_Into_Memory(state* s);
void Toggle_Display_Mode(state* s);
void Memory_Display(state* s);
void Save_Into_File(state* s);
void Memory_Modify(state* s);
void read_units_to_memory(FILE* input, unsigned char* buffer, int count);
void print_units(FILE* output, unsigned char* buffer, int count);
char* unit_to_format(int unit);
void write_units(FILE* output, char* buffer, int count);

int option;
char op [4];
int debug_mode = 0;
char error_fileName[21] = "file name is not set\n";
state * s;
FILE* fp;

int main (int argc , char* argv[]){
    s = (state *) malloc (sizeof(state));
    s->unit_size = 1;
    s->debug_mode = 0;
    s->display_mode = 0;
    struct fun_desc menu[] = { { "Toggle Debug Mode", Toggle_Debug_Mode }, { "Set File Name", Set_File_Name},{ "Set Unit Size", Set_Unit_Size}, {"Load Into Memory" , Load_Into_Memory},{"Toggle Display Mode", Toggle_Display_Mode}, {"Memory Display" , Memory_Display}, {"Save Into File" , Save_Into_File}, {"Memory Modify" , Memory_Modify},{ "Quit", Quit}, { NULL, NULL}};
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
    if(s->debug_mode == 0){
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
    printf("please enter file name: ");
    fgets(file, 99, stdin);
    file[strlen(file)-1]='\0';
    strncpy(s->file_name, file, 100);
    if(s->debug_mode)
        fprintf(stderr, "Debug: file name set to %s\n", s->file_name);
}

void Set_Unit_Size(state* s){
    char num[4];
    int number;
    printf("please enter unit size: ");
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
    if(s->debug_mode)
        fprintf(stderr, "quitting\n");
    exit(0);
}

void Load_Into_Memory(state* s){
    char location[10000];
    char length[10000];
    int length_int, location_int;
    if(strcmp(s->file_name,"")==0){
        fprintf(stderr , "Please first set a file\n");
        return;
    }
    fp = fopen(s->file_name , "r+");
    if(!fp){
        fprintf(stderr , "Can't open file %s\n" , s->file_name);
        return;
    }
    printf("Please enter location (in hexadecimal) and length (in decimal):\n");
    fgets(location , 99999, stdin);
    sscanf(location, "%x\n", &location_int);
    fgets(length , 99999, stdin);
    sscanf(length, "%d\n", &length_int);
    s->mem_count = length_int*s->unit_size;
    if(s->debug_mode){
        fprintf(stderr, "file name is: %s\n", s->file_name);
        fprintf(stderr, "location is: %d\n", location_int);
        fprintf(stderr, "length is: %d\n", length_int);
    }
    fseek(fp , location_int , SEEK_SET);
    read_units_to_memory(fp , s->mem_buf , length_int);
    fclose(fp);
}

void Toggle_Display_Mode(state* s){
    if(s->display_mode == 0){
        s->display_mode = 1;
        fprintf(stderr,"Display flag now on, hexadecimal representation\n");
    }
    else{
        s->display_mode = 0;
        fprintf(stderr,"Display flag now off, decimal representation\n");
    }
}


void Memory_Display(state* s){
    char addr[10000];
    char units[10000];
    int addr_int, unit_int;
    printf("Please enter number of address (in Hexadecimal) and units (in decimal):\n");
    fgets(addr , 99999, stdin);
    sscanf(addr, "%x\n", &addr_int);
    fgets(units , 99999, stdin);
    sscanf(units, "%d\n", &unit_int);
    if(!addr_int)
        print_units(stdout , s->mem_buf , unit_int);
    else
        print_units(stdout , (unsigned char*)(addr_int) , unit_int);    
}


void Save_Into_File(state* s){
    fp = fopen(s->file_name, "r+");
    char _source_address[10000];
    char _target_location[10000];
    char _length[10000];
    int source_address;
    int target_location;
    int length;
    printf("Please enter source-adress (in hexadecimal), target-location (in hexadecimal) and length (in decimal):\n");
    fgets(_source_address , 99999, stdin);
    sscanf(_source_address, "%x\n", &source_address);
    fgets(_target_location , 99999, stdin);
    sscanf(_target_location, "%x\n", &target_location);
    fgets(_length , 99999, stdin);
    sscanf(_length, "%d\n", &length);
    if (fseek(fp, target_location, SEEK_SET) == -1){
        fprintf(stderr, "the offset bigger than the file's size");
        fclose(fp);
        return;
    }
    if(source_address == 0){
        write_units(fp, (char*)s->mem_buf, length);
    }
    else{
        write_units(fp, (char*) source_address, length);
    }
    fclose(fp);
}

void Memory_Modify(state* s){
    char _location[10000];
    char _val[10000];
    int location;
    int val;
    printf("Please enter location and val (in hexadecimal):\n");
    fgets(_location , 99999, stdin);
    sscanf(_location, "%x\n", &location);
    fgets(_val , 99999, stdin);
    sscanf(_val, "%x\n", &val);
    if(location+s->unit_size > s->mem_count){
        fprintf (stderr, "location is invalid");
        return;
    }
    if(s->debug_mode)
        fprintf (stderr, "the location is: %x and the val is: %x\n", location, val);
    memcpy(s->mem_buf+location, &val, s->unit_size);
}

void read_units_to_memory(FILE* input, unsigned char* buffer, int count) {
    fread(buffer, s->unit_size, count, input);
}


void print_units(FILE* output, unsigned char* buffer, int count) {
    unsigned char* end = buffer + s->unit_size*count;
    int counter = 0;
    while (buffer < end && counter<=s->mem_count) {
        counter++;
        int var = *((int*)(buffer));
        fprintf(output, unit_to_format(s->unit_size), var);
        buffer += s->unit_size;
    }
}

char* unit_to_format(int unit) {
    static char* formats_hex[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    static char* formats_dec[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    if(s->display_mode == 1)
        return formats_hex[s->unit_size-1];
    return formats_dec[s->unit_size-1];
}  

void write_units(FILE* output, char* buffer, int count) {
    fwrite(buffer, s->unit_size, count, output);
}
