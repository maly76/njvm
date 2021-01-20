// gcc -I./src/bigint/build/include -L./src/bigint/build/lib njvm.c MemoryManagement.c -lbigint -o njvm
// VM Version 8
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MemoryManagement.h"
#include "src/bigint/src/bigint.h"


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
#define NEW 32
#define GETF 33
#define PUTF 34
#define NEWA 35
#define GETFA 36
#define PUTFA 37
#define GETSZ 38
#define PUSHN 39
#define REFEQ 40
#define REFNE 41

#define IMMEDIATE(x)((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i)((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#define OPCODE(x)((x) >> 24)


#define VERSION 8

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
ObjRef newCmpObject(int elementsNumber);
void initializeStackslots(int counter);
bool checkIndex(int index, ObjRef cmpObj);
bool statistic = false;
bool purge = false;

// ------------------------Variables--------------------->
const char *instructions[] = {"halt", "pushc", "add", "sub", "mul", "div", "mod", "rdint", "wrint", "rdchr",
 "wrchr", "pushg", "popg", "asf", "rsf", "pushl", "popl", "eq", "ne", "lt", "le", "gt", "ge", "jmp", "brf",
 "brt", "call", "ret", "drop", "pushr", "popr", "dup", "new", "getf", "putf", "newa", "getfa", "putfa", "getsz",
  "pushn", "refeq", "refne"};
int sp=0; // the stackpointer
int counter = 0;  // the counter for saving the instructions
int pc = 0; // the program counter
int fp = 0;   // frame pointer
ObjRef *SDA = NULL;
StackSlot *stack = NULL;
ObjRef RVR = NULL;
unsigned int *program_memory;
unsigned int info [4];
int breakpoint = -1;
int sizeOfStack = 64;       // defaultvalue is 64
int sizeOfHeap = 8192;      // defaultvalue is 8192
bool debugMode = false;
int counter;
size_t currentSizeOfStack = 0;



// <------------------------Functions(Definition)--------------------------->

bool checkIndex(int index, ObjRef cmpObject)
{
    if (IS_PRIMITIVE(cmpObject) == TRUE)
        fatalError("trying to index a not array");
    
    if (index < 0 || index > GET_ELEMENT_COUNT(cmpObject))
        fatalError("Array index out of bounds");
    else 
        return true;
}

void initializeStackslots(int count){
    for (int i = 0; i < sizeof(stack) / sizeof(StackSlot); i++)
    {
        StackSlot slot;
        slot.u.objRef = NULL;
        slot.isObjRef = true;
        stack[i] = slot;
    }
}

void fatalError(char *msg)
{
    printf("Error: %s\n", msg);
    exit(1);
}

ObjRef newPrimObject(int datasize) {
  //printf("size is %d\n", datasize);
  ObjRef objRef;

  objRef = reserveMemoryForAnObject(sizeof(*objRef)+(datasize * sizeof(unsigned char)));

  if (objRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  objRef->size = datasize;
  return objRef;
}

ObjRef newCmpObject(int elementsNumber)
{
    ObjRef cmpObject = reserveMemoryForAnObject(sizeof(*cmpObject) + elementsNumber * 8);
    if (cmpObject == NULL)
        fatalError("newCmpObject() got no memory");
    for (int i = 0; i < elementsNumber; i++){
        GET_REFS_PTR(cmpObject)[i] = reserveMemoryForAnObject(8);
        GET_REFS_PTR(cmpObject)[i] = NULL;
    }
    cmpObject->size = elementsNumber;
    cmpObject->size |= MSB; 

    return cmpObject;
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
    else{
        push(bip.res, 0, true);
    }
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
    free(instruc);
    ObjRef value;
    ObjRef object;
    switch (opcode)
    {
        case 1: convertAndPushObject(immediate); break;//pushc: convert the int to big and push it on the stack
        case 2: calculate('+');break; //add
        case 3: calculate('-');break; //sub
        case 4: calculate('*');break; //mul
        case 5: calculate('/');break; //div
        case 6: calculate('%');break; //mod
        case 7: bigRead(stdin); push(bip.res,0,true); break;// rdint
        case 8: printnumberOrObject(pop(), false);  break;// wrint
        case 9: bigRead(stdin); push(bip.res,0,true); break;// rdchr
        case 10: printnumberOrObject(pop(), true); break;// wrchr
        case 11: push(SDA[immediate], 0, true); break;    // pushg
        case 12: SDA[immediate] = pop().u.objRef; break;  // popg
        case 13: push(NULL, fp, false); fp = sp; sp += immediate;break;     // asf        //Warum ist hier false - also ist kein Objekt
        case 14: sp = fp; fp = pop().u.number; break;     // rsf
        case 15: push(stack[fp+immediate].u.objRef, 0, true); break;     // pushl
        case 16: stack[fp+immediate] = pop(); break;     // popl

        /**********************COMPARISON**********************/
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
        case 30: RVR = pop().u.objRef;   break;           // popr
        case 31: if(stack[sp-1].isObjRef == true) push(stack[sp-1].u.objRef, 0, true); 
                   else push(NULL, stack[sp-1].u.number, false); break;  //dup
        case 32: push(newCmpObject(immediate), 0, true);    break; // new <n>
        case 33: value = GET_REFS_PTR(pop().u.objRef)[immediate]; push(value, 0, true); break; // getf
        case 34: value = pop().u.objRef; GET_REFS_PTR(pop().u.objRef)[immediate] = value; break; // putf
        case 35: bip.op1 = pop().u.objRef; push(newCmpObject(bigToInt()), 0, true); break; // newa
        case 36: bip.op1 = pop().u.objRef; value = pop().u.objRef; 
        if (checkIndex(bigToInt(), value) == true) push(GET_REFS_PTR(value)[bigToInt()] , 0, true); break; // getfa
        case 37: value = pop().u.objRef; bip.op1 = pop().u.objRef; object = pop().u.objRef; 
        if(checkIndex(bigToInt(), object) == true) GET_REFS_PTR(object)[bigToInt()] = value; break; // putfa
        case 38: value = pop().u.objRef; if (IS_PRIMITIVE(value) == TRUE) convertAndPushObject(-1);
                                         else{ convertAndPushObject(GET_ELEMENT_COUNT(value));} break; // getsz
        case 39: push(NULL, 0, true); break; // pushn
        case 40: convertAndPushObject(compare(REFEQ));  break; // refeq
        case 41: convertAndPushObject(compare(REFNE));  break; // refne
        default: break;
    }

    instruc = cutInstructions(program_memory[pc]);
    if (instruc[0] == 0)        // halt
    {
        printf("Ninja Virtual Machine stopped\n");
        exit(0);
    }
    free(instruc);
    return 1;
}

int printnumberOrObject(StackSlot slot, bool isChar)
{
    bip.op1 = slot.u.objRef;
    if (slot.isObjRef == true && isChar == false)
        bigPrint(stdout);
    else if (isChar == false)
        printf("%d", slot.u.number);
    else if(isChar == true);
        printf("%c", bigToInt());
}

int compare(int instr)
{
    bip.op2 = pop().u.objRef;
    bip.op1 = pop().u.objRef;
    switch (instr)
    {
        case 17: if(bigCmp() == 0) return TRUE;  else return FALSE; break; // eq
        case 18: if(bigCmp() != 0) return TRUE; else return FALSE; break; // ne
        case 19: if(bigCmp() < 0) return TRUE; else return FALSE; break; // lt
        case 20: if(bigCmp() <= 0) return TRUE; else return FALSE; break; // le
        case 21: if(bigCmp() > 0) return TRUE; else return FALSE; break; // gt
        case 22: if(bigCmp() >= 0) return TRUE; else return FALSE; break; // ge
        case 40: if (bip.op1 == bip.op2) return TRUE; else return FALSE; break;  // refe
        case 41: if (bip.op1 != bip.op2) return TRUE; else return FALSE; break;  // refne
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
        slot.u.objRef = bigObject;
    }

    if (currentSizeOfStack + sizeof(slot) > sizeOfStack * 1024){
        fatalError("stack overflow");
    }

    stack[sp] = slot;
    currentSizeOfStack += sizeof(slot);
    sp++;
}

StackSlot pop(void) {
    sp--;
    StackSlot tmp = stack[sp];
    currentSizeOfStack -= sizeof(tmp);
    return tmp;
}

void print_stack(void) {
    for (int i=sp; i>=0; i--) {
        if (i == sp && i == fp)
            printf("sp, fp   --->   %3d:   (xxxxxx) xxxxxx\n", i);
        else if (i==sp)
            printf("sp   --->   %3d:   (xxxxxx) xxxxxx\n", i);
        else if (i == fp){
            if (stack[i].isObjRef == true)
                printf("fp   --->   %3d: (ObRef) %p\n", i, (void *)stack[i].u.objRef);
            else
                printf("fp   --->   %3d: (Number) %4d\n", i, stack[i].u.number);
        }
        else{
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
    if ((filep = fopen(filename, "r")) == NULL){
        fatalError("cannot open file code");
    }

    fread(info, sizeof(unsigned int), 4, filep); //   global variable.size * 4

     if(info[0] != 0x46424a4e){//info0 - identifies
        fatalError("Falscher Identifier");
     }

    if(info[1] != VERSION){ //info1= Version
        fatalError("VERSION ist falsch!");
    }

    program_memory = reserveMemoryForPM(info[2]);
    stack = reserveMemoryForStack(sizeOfStack);
    initializeStackslots(info[2]);
    SDA = reserveMemoryForSDA(info[3]);
    reserveMemoryinHeap(sizeOfHeap);

    if(program_memory == NULL){
        fatalError("Speicherplatz falsch vergeben");
    }

    if(SDA == NULL){
        fatalError("Globaler Speicherplatz falsch vergeben");
    }

    int read_len = 0;
    unsigned int c;
    fseek(filep, 16, SEEK_SET);        // überspringe die ersten 16 Bytes, weil sie keine Instruktionen enthalten
    while ((read_len = fread(&c, sizeof(unsigned int), 1, filep)) != 0) {
        saveInMemory(c);
    }
    int fclose(FILE * filep);
    if (fclose(filep) != 0){
        fatalError("ERROR (fclose)");
    }
}

void debugmode()
{
    print_step();
    // inspect, list, breakpoint, step, run, quit
    while (1)
    {
        char * input = malloc(20);
        printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
        scanf("%s", input);                 // read the specified action
        if (strcmp(input, "inspect") == 0){
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
                    printf ("object reference?\n");
                    long ul;
                    scanf("%s", input);
                    ul = strtoul (input, NULL, 0);
                    ObjRef or = (ObjRef)ul;

                    if (ul == 0)    // check if the entered reference is valid
                    {
                        printf("Segmentation fault\n");
                        break;
                    }

                    if (IS_PRIMITIVE(or) == FALSE)
                    {
                        printf("<compound object>\n");
                        break;
                    }

                    for (int i = 0; i < sp; i++)    // check if the entered reference exists on the stack
                    {
                        if ((long)stack[i].u.objRef == ul)
                        {
                            bip.op1 = or;
                            printf("<primitive object> \nValue = %d\n", bigToInt());
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
        free(input);
    }
}

int main (int argc, char *argv[]) {
    if (argc < 2)
        fatalError("too few arguments");

    if(strcmp(argv[1], "--help") == 0)
    {
        printf("usage: ./njvm [options] ...\n"
               "--stack <n>      set stack size to n KBytes (default: n = 64)\n"
               "--heap <n>       set heap size to n KBytes (default: n = 8192)\n"
               "--gcstats        show garbage collection statistics\n"
               "--gcpurge         purge old objects after collection\n"
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

    for (int i = 0; i < argc - 1; i++)
    {
        
        if(strcmp(argv[i], "--stack") == 0)
        {
            sizeOfStack = atoi(argv[i+1]);
            if (sizeOfHeap <= 0)
                fatalError("illegal stack size");
        }
        else if(strcmp(argv[i], "--heap") == 0)
        {
            sizeOfHeap = atoi(argv[i+1]);
            if (sizeOfHeap <= 0)
                fatalError("illegal heap size");
        }
        else if (strcmp(argv[i], "--debug") == 0)
        {
            printf("DEBUG: file '%s' loaded (code size = %d, data size = %d)\n", argv[2], info[2], info[3]);
            //printf("Ninja Virtual Machine started\n");
            debugMode = true;
        }
        else if (strcmp(argv[i], "--gcpurge") == 0)  
        {
            purge = true;
        }
        else if (strcmp(argv[i], "--gcstats") == 0)
        {
            statistic = true;
        }
    }

    if (strstr(argv[argc-1], ".bin") != NULL){//strcmp(argv[argc-1], "") != 0){
        printf("Ninja Virtual Machine started\n");
        openFile(argv[argc-1]);
        if (debugMode == true)
            debugmode();
        else 
            start();
    }
    else
        fatalError("unvalied file name!\n"); 
    return 0;
}