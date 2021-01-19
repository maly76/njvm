// gcc -g -Wall -std=c99 -pedantic -o njvm njvm.c
// VM Version 4
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
//Macro
#define MAXITEMS 100

#define TRUE 1
#define FALSE 0

#define HALT 0
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10
#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define eq 17
#define ne 18
#define lt 19
#define le 20
#define gt 21
#define ge 22
#define jmp 23
#define brf 24
#define brt 25
#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31

#define IMMEDIATE(x)((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i)((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#define OPCODE(x)((x) >> 24)
#define VERSION 5

typedef struct {
unsigned int size; // # byte of payload
unsigned char data[1]; // payload data, size as needed!
} *ObjRef;

typedef struct {
    bool isDefined;
    bool isObjRef;
    union {
        ObjRef objRef; // isObjRef = TRUE
        int number; // isObjRef = FALSE
    } u;
} StackSlot;


// ------------------------Variables--------------------->
const char *instructions[] = {"halt", "pushc", "add", "sub", "mul", "div", "mod", "rdint", "wrint", "rdchr",
 "wrchr", "pushg", "popg", "asf", "rsf", "pushl", "popl", "eq", "ne", "lt", "le", "gt", "ge", "jmp", "brf", 
 "brt", "call", "ret", "drop", "pushr", "popr", "dup"};
int sp=0; // the stackpointer
int counter = 0;  // the counter for saving the instructions
int pc = 0; // the program counter
int fp = 0;   // frame pointer
ObjRef *SDA;
StackSlot *stack;
ObjRef RVR;
unsigned int *program_memory;
unsigned int info [4];
int breakpoint = -1;

int main (int argc, char *argv[]) {
    ObjRef obref;
    char * input = malloc(50);
        if ((obref = malloc(sizeof(unsigned int) + sizeof(int))) == NULL)
            perror("malloc");
        
        obref->size = sizeof(int);
        *(int *)obref->data = 25;
    printf("%p\n", (void *)obref);

    char buffer [256];
    long ul;
    printf ("Enter an reference: ");
    //fgets (input, 256, stdin);
    scanf("%s", input);
    ul = strtoul (input, NULL, 0);
    
    printf ("Value entered: %x.",ul);
    return 0;
    
    /*char * input = malloc(20);
    printf("%u\n", (void*)&a);
    printf("object reference?\n");
    scanf("%p", (void **)b);
    printf("%d", *b);*/
    return 0;
}