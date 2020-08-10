.386
.model flat,stdcall
include \masm32\include\masm32rt.inc
include \masm32\include\msvcrt.inc
include \masm32\include\kernel32.inc
includelib \masm32\lib\masm32.lib
includelib \masm32\lib\msvcrt.lib
includelib \masm32\lib\kernel32.lib
.data
prnt_scan_real DB "%f",0
OutputFormat db "%i",0
a DD ?
b DD ?
var_1 DB "Vvedite_N:",0
var_2 DB "Summa_ryada=",0
.code
Sum proc k:DWORD, d:DWORD
MOV edx,d
CMP edx,0
JLE L0
MOV eax,k
MOV ebx,d
ADD eax,ebx
MOV k,eax
MOV eax,d
SUB eax,1
MOV d,eax
invoke Sum, k,d
JMP L1
L0:
MOV eax,k
MOV a,eax
L1:
ret
Sum ENDP
start:
FINIT
printf("Vvedite_N:")
invoke crt_scanf, ADDR OutputFormat, ADDR b
MOV eax,0
MOV a,eax
invoke Sum, a,b
printf("Summa_ryada=")
printf("%i",a)
invoke crt__getch
invoke ExitProcess,0
end start
END
