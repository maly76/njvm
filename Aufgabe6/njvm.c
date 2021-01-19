// gcc -I./src/bigint/build/include -L./src/bigint/build/lib njvm.c -lbigint -o njvm
// VM Version 6
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "src/bigint/src/support.h"
#include "src/bigint/src/bigint.h"


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
#define VERSION 6

typedef struct {
    bool isDefined; //Versteh den Punkt nicht zu 100%
    bool isObjRef;
    union {
        ObjRef objRef; // isObjRef = TRUE
        int number; // isObjRef = FALSE
    } u;
} StackSlot;

// <--------------------Functions(Deklaration)----------------------->
StackSlot pop();
void push(ObjRef bigObject, int i, bool isObject);
void print_stack(void);
int *cutInstructions(unsigned int instr);
void calculate(char o);
int execute(int i);
int compare(int instr);
void start();
void saveInMemory(int instruction);
void print_step(void);
void debugmode(void);
int printnumberOrObject(StackSlot slot, bool isChar);
void fatalError(char *msg);		
ObjRef newPrimObject(int dataSize);	
void convertAndPushObject(int x);


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


// <------------------------Functions(Definition)--------------------------->

void fatalError(char *msg)
{
    printf("Fatal error: %s\n", msg);
    exit(1);
}

ObjRef newPrimObject(int dataSize) {
  ObjRef objRef;

  objRef = malloc(sizeof(unsigned int) +
                  dataSize * sizeof(unsigned char));
  if (objRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  objRef->size = dataSize;
  return objRef;
}

int *cutInstructions(unsigned int instr){
    int opcode = OPCODE(instr);
    int immediate = SIGN_EXTEND(instr & 0x00FFFFFF);
    int *ret = malloc(2);
    ret[0] = opcode;
    ret[1] = immediate;
    return ret;
}

void calculate(char o){
    int result;
    bip.op2 = pop().u.objRef;
    bip.op1 = pop().u.objRef;
    switch (o)
    {
        case '+':   bigAdd();  break;
        case '-':   bigSub();  break;
        case '*':   bigMul();  break;
        case '/':   bigDiv();  break;
        case '%':   bigDiv();  break;
        default: break;
    }
    
    if (o == '%')
        push(bip.rem, 0, true);
    else 
        push(bip.res, 0, true);

    //bip.op1 = bip.res;
    //push(bip.op1, 0, true);
    //result = bigToInt();
    //convertAndPushObject(result);
}

void convertAndPushObject(int x)
{
    bigFromInt(x);
    push(bip.res, 0, true);         // we don't need the x value because it is an object
}

int execute(int i){
    int *instruc = cutInstructions(program_memory[pc]);
    pc++;
    int opcode = instruc[0];
    int immediate = instruc[1];
    int tempint;
    char tempchar;
    switch (opcode)
    {
        case 1: convertAndPushObject(immediate); break;//pushc: convert the int to big and push it on the stack
        case 2: calculate('+');break; //add
        case 3: calculate('-');break; //sub
        case 4: calculate('*');break; //mul
        case 5: calculate('/');break; //div
        case 6: calculate('%');break; //mod
        case 7: /*scanf("%d", &tempint);*/ bigRead(stdin); push(bip.res,0,true); /*convertAndPushObject(tempint);*/ break;// rdint
        case 8: printnumberOrObject(pop(), false);  break;// wrint
        case 9: /*scanf(" %c", &tempchar);*/ bigRead(stdin); push(bip.res,0,true); /*convertAndPushObject(tempchar);*/ break;// rdchr
        case 10: printnumberOrObject(pop(), true); break;// wrchr
        case 11: push(SDA[immediate], 0, true); break;    // pushg
        case 12: SDA[immediate] = pop().u.objRef; break;  // popg
        case 13: push(NULL, fp, false); fp = sp; sp += immediate;break;     // asf        //Warum ist hier false - also ist kein Objekt
        case 14: sp = fp; fp = pop().u.number; break;     // rsf
        case 15: push(stack[fp+immediate].u.objRef, 0, true); break;     // pushl
        case 16: stack[fp+immediate] = pop(); break;     // popl

        /******************COMPARISON*****************/
        case 17: convertAndPushObject(compare(eq)); break; // eq
        case 18: convertAndPushObject(compare(ne)); break; // ne
        case 19: convertAndPushObject(compare(lt)); break; // lt
        case 20: convertAndPushObject(compare(le)); break; // le
        case 21: convertAndPushObject(compare(gt)); break; // gt
        case 22: convertAndPushObject(compare(ge)); break; // ge
        /********************JAMPING********************/
        case 23: pc = immediate; break; // jmp
        case 24: bip.op1 = pop().u.objRef; if(bigToInt() == FALSE) pc = immediate; break; // brf
        case 25: bip.op1 = pop().u.objRef; if(bigToInt() == TRUE) pc = immediate; break; // brt
        /******************FUNCTION CALL*************************/
        // Berechnung eines Offsets:   stack[-2-(Anzahl der Argumente)+(i-tes Argument)]
        case 26: push(NULL, pc/*+1*/, false); pc = immediate; break;       // call
        case 27: pc = pop().u.number;  break;                    // ret
        case 28: for (int i = 0; i < immediate; i++) pop();  break; // drop n, es werden n Elemente gelöscht..!!
        case 29: push(RVR, 0, true); break;           // pushr
        case 30: RVR = pop().u.objRef;    break;           // popr
        //case 31: if(stack[sp-1].isObjRef == true) convertAndPushObject(*(int *) stack[sp-1].u.objRef->data);
        //         else push(NULL, stack[sp-1].u.number, false); /*int dup = pop(); push(dup); push(dup);*/    //dup

        default: break;
    }

    if (cutInstructions(program_memory[pc])[0] == 0)        // halt
    {
        printf("Ninja Virtual Machine stopped\n");
        exit(0);
    }
    return 1;
}

int printnumberOrObject(StackSlot slot, bool  isChar)
{
    bip.op1 = slot.u.objRef;
    if (slot.isObjRef == true && isChar == false)
    {
        bigPrint(stdout);
    }
    else if (isChar == false)
        printf("%d", slot.u.number);
    else
        printf("%c", bigToInt());
}

int compare(int instr)
{
    bip.op2 = pop().u.objRef;
    bip.op1 = pop().u.objRef;
    int result = bigCmp();
    switch (instr)
    {
        case 17: if(result == 0) return TRUE;  else return FALSE; break; // eq
        case 18: if(result != 0) return TRUE; else return FALSE; break; // ne
        case 19: if(result < 0) return TRUE; else return FALSE; break; // lt
        case 20: if(result <= 0) return TRUE; else return FALSE; break; // le
        case 21: if(result > 0) return TRUE; else return FALSE; break; // gt
        case 22: if(result >= 0) return TRUE; else return FALSE; break; // ge
        default:
            break;
    }
    return FALSE;
}

void start(){
    while (1)                   // while true = unendliche Schleife
    {
        if (breakpoint == pc)
        {
            print_step();
            breakpoint = -1;                // stop at the breakpoint and reset it
            break;
        }
        execute(program_memory[pc]);
    }
}

void print_step(void){
        int *instruc = cutInstructions(program_memory[pc]);
        int opcode = instruc[0];
        int immediate = instruc[1];

        if (opcode == ADD || opcode == MUL || opcode == DIV || opcode == SUB || opcode == ne || opcode == RDINT ||
            opcode == lt || opcode == gt || opcode == ge || opcode == WRCHR || opcode == WRINT ||
            opcode == HALT || opcode == RSF || opcode == RDCHR || opcode == RDINT)
            printf("%d:  %4s \n", pc, instructions[opcode]);
        else
            printf("%d:  %4s %4d\n", pc, instructions[opcode], immediate);
}

void saveInMemory(int instruction){
    program_memory[counter] = instruction;
    counter++;
}

// Stack implementaion
void push(ObjRef bigObject, int x, bool isObject) {

    StackSlot slot;
    slot.isDefined = true;
    if (isObject == false)
    {
        slot.isObjRef = false;
        slot.u.number = x;
    }else {
        slot.isObjRef = true;
        slot.u.objRef = bigObject/*bip.res*/;
    }

    stack[sp] = slot;
    sp++;
}

StackSlot pop(void) {
    sp--;
    StackSlot tmp = stack[sp];
    return tmp;
}

void print_stack(void) {
    for (int i=sp; i>=0; i--) {
        if (i == sp && i == fp)
            printf("sp, fp   --->   %3d:   (xxxxxx) xxxxxx\n", i);
        else if (i==sp)
            printf("sp   --->   %3d:   (xxxxxx) xxxxxx\n", i);
        else if (i == fp && stack[i].isDefined != true)
            printf("fp   --->   %3d:   (xxxxxx) xxxxxx\n", i);
        else if (i == fp && stack[i].isDefined == true)
        {
            if (stack[i].isObjRef == true)
                printf("fp   --->   %3d: (ObRef) %p\n", i, (void *)stack[i].u.objRef);
            else
                printf("fp   --->   %3d: (Number) %4d\n", i, stack[i].u.number);
        }
        else if (stack[i].isDefined != true)
            printf("%15d:   (xxxxxx) xxxxxx\n", i);
        else
        {
            if (stack[i].isObjRef == true)
                printf("%15d: (ObRef) %p\n", i, (void *)stack[i].u.objRef);
            else
                printf("%15d: (Number) %4d\n", i, stack[i].u.number);
        }
    }
}

void openFile(char filename[])
{
    FILE * filep = NULL;
    if ((filep = fopen(filename, "r")) == NULL)
    {
        perror("ERROR - cannot open file code\n");
        exit(1);
    }

    fread(info, sizeof(unsigned int), 4, filep); //   global variable.size * 4

     if(info[0] != 0x46424a4e){//info0 - identifies
         perror("Falscher Identifier");
         exit(100);
     }

    if(info[1] != VERSION){ //info1= Version
        perror("VERSION ist falsch!");
        exit(100);
    }

    program_memory = malloc(info[2] * 4); //info2 * Int größe  info2= number Instruction
    stack = malloc(info[2] * sizeof(StackSlot));
    SDA = malloc(info[3] * sizeof(ObjRef));

    if(program_memory == NULL){
        perror("Speicherplatz falsch vergeben");
        exit(100);
    }

    if(SDA == NULL){
        perror("Globaler Speicherplatz falsch vergeben");
        exit(100);
    }

    int read_len = 0;
    unsigned int c;
    fseek(filep, 16, SEEK_SET);        // überspringe die ersten 16 Bytes, weil sie keine Instruktionen enthalten
    while ((read_len = fread(&c, sizeof(unsigned int), 1, filep)) != 0) {
        saveInMemory(c);
    }
    int fclose(FILE * filep);
    if (fclose(filep) != 0){
        perror("ERROR (fclose)");
    }
}

void debugmode()
{
    // inspect, list, breakpoint, step, run, quit
    while (1)
    {
        char * input = malloc(50);
        printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
        scanf("%s", input);                 // read the specified action
        if (strcmp(input, "inspect") == 0)
          {
          while (1)
          {
            printf("DEBUG [inspect]: stack, data, object or <ret> for return?\n");
            scanf("%s", input);
            if (strcmp(input, "data") == 0)
            {
                printf("                --- end of data ---\n");
                print_step();
                break;
            }
            else if (strcmp(input, "stack") == 0)
            {
                print_stack();
                printf("                --- bottom of stack ---\n");
                print_step();
                break;
            }
            else if (strcmp(input, "object") == 0)
            {
                printf ("object reference?");
                long ul;
                scanf("%s", input);
                ul = strtoul (input, NULL, 0);

                if (ul == 0)    // check if the entered reference is valid
                {
                printf("Segmentation fault\n");
                break;
                }

                for (int i = 0; i < sp; i++)    // check if the entered reference exists on the stack
                {
                    if ((long)stack[i].u.objRef == ul)
                    {
                        ObjRef or = (ObjRef)ul;
                        bip.op1 = or;
                        printf("Value = %d\n", bigToInt());
                        break;
                    }
                }
                break;
            }
            else if (strcmp(input, "ret") == 0)
                break;
            else
                printf("unknown column: please try it again\n");
          }
    }
        else if (strcmp(input, "list") == 0)
        {
            for (int i = 0; i < counter; i++)
            {
                int *instruction = cutInstructions(program_memory[i]);
                if (instruction[1] != 0)
                    printf("%d:   %s   %d  \n", i, instructions[instruction[0]], instruction[1]);
                else
                    printf("%d:   %s  \n", i, instructions[instruction[0]]);
            }
            printf("        --- end of code ---\n");
            print_step();
        }
        else if (strcmp(input, "breakpoint") == 0)
        {
            if (breakpoint > -1) printf("DEBUG [breakpoint]: set at %d\n", breakpoint);

            printf("DEBUG [breakpoint]: cleared\n");
            printf("DEBUG [breakpoint]: address to set, -1 to clear, <ret> for no change?\n");
            scanf("%s", input);
            if (strcmp(input, "ret") != 0)
            {
                int address = atoi(input);
                if(address == -1){
                    breakpoint = -1;
                    printf("DEBUG [breakpoint]: now cleared\n");
                }

                else if(address > -1){
                    breakpoint = address;
                    printf("DEBUG [breakpoint]: now set at %d\n", breakpoint);
                }
                print_step();
            }
        }
        else if (strcmp(input, "step") == 0)
               {
                   execute(program_memory[pc]);
                   print_step();
               }
               else if (strcmp(input, "run") == 0)
                   start();
               else if (strcmp(input, "quit") == 0)
                   exit(0);
               else
                   printf("unknown column: please try it again\n");
    }
}


int main (int argc, char *argv[]) {

    if (argc < 2)
           exit(1);

    if(strcmp(argv[1], "--help") == 0)
    {
        printf("usage: ./njvm [option] [option] ...\n"
               "--debug          debug mode\n"
               "<filename>       select file code to run\n"
               "--version        show version and exit\n"
               "--help           show this help and exit\n");
        exit(0);
    }
    else if(strcmp(argv[1], "--version") == 0)
    {
        printf("Ninja Virtual Machine version %d \n", VERSION);
        exit(0);
    }
    else if (strcmp(argv[1], "--debug") == 0)
    {
        openFile(argv[2]);
        printf("DEBUG: file '%s' loaded (code size = %d, data size = %d)\n", argv[2], info[2], info[3]);
        printf("Ninja Virtual Machine started\n");
        print_step();
        debugmode();
        printf("Ninja Virtual Machine stopped\n");
    }
    else if (strstr(argv[argc-1], ".bin") != NULL){//strcmp(argv[argc-1], "") != 0){
        printf("Ninja Virtual Machine started\n");
        openFile(argv[argc-1]);
        start();
    }
    else
    {
        printf("ERORR - please enter a valied file name!\n");
        exit(0);   
    }
    
    return 0;
}