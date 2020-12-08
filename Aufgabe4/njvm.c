// gcc -g -Wall -std=c99 -pedantic -o njvm njvm.c
// VM Version 4
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
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
#define VERSION 4

// <--------------------Functions(Deklaration)----------------------->
int pop();
void push(int i);
void print_stack(void);
int *cutInstructions(unsigned int instr);
void calculate(char o);
int execute(int i);
int compare(int instr);
void start();
void saveInMemory(int instruction);
void print_step(void);
void debugmode(void);


// ------------------------Variables--------------------->
const char *instructions[] = {"halt", "pushc", "add", "sub", "mul", "div", "mod", "rdint", "wrint", "rdchr",
 "wrchr", "pushg", "popg", "asf", "rsf", "pushl", "popl", "eq", "ne", "lt", "le", "gt", "ge", "jmp", "brf", 
 "brt", "call", "ret", "drop", "pushr", "popr", "dup"};
int sp=0; // the stackpointer
int counter = 0;  // the counter for saving the instructions
int pc = 0; // the program counter
int fp = 0;   // frame pointer
int *SDA;
int *stack;
int RVR;
unsigned int *program_memory;
unsigned int info [4];
int breakpoint = -1;


// <------------------------Functions(Definition)--------------------------->

int *cutInstructions(unsigned int instr){
    int opcode = OPCODE(instr);
    int immediate = SIGN_EXTEND(instr & 0x00FFFFFF);
    //printf("0x%08x -> Opcode [%d] Immediate [%d]\n", instr, opcode, immediate);
    int *ret = malloc(2);
    ret[0] = opcode;
    ret[1] = immediate;
    return ret;
}

void calculate(char o){
    int y = pop();
    int x = pop();
    int result;
    switch (o)
    {
        case '+':result = x + y;break;
        case '-':result = x - y;break;
        case '*':result = x * y;break;
        case '/':result = x / y;break;
        case '%':result = x % y;break;
        default: break;
    }
    push(result);
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
        case 1: push(immediate);break;//pushc
        case 2: calculate('+');break; //add
        case 3: calculate('-');break; //sub
        case 4: calculate('*');break; //mul
        case 5: calculate('/');break; //div
        case 6: calculate('%');break; //mod
        case 7: printf("please enter a number..\n"); scanf("%d", &tempint); push(tempint);break;// rdint
        case 8: printf("%d", pop());break;// wrint
        case 9: printf("please enter a charakter..\n"); scanf(" %c", &tempchar); push(tempchar); break;// rdchr
        case 10: printf("%c", pop());;break;// wrchr
        case 11: push(SDA[immediate]);break;    // pushg
        case 12: SDA[immediate] = pop();break;  // popg
        case 13: push(fp); fp = sp; sp += immediate;break;     // asf
        case 14: sp = fp; fp = pop();break;     // rsf
        case 15: push(stack[fp+immediate]);break;     // pushl
        case 16: stack[fp+immediate] = pop();break;     // popl
            
        /******************COMPARISON*****************/
        case 17: push(compare(eq)); break; // eq
        case 18: push(compare(ne)); break; // ne
        case 19: push(compare(lt)); break; // lt
        case 20: push(compare(le)); break; // le
        case 21: push(compare(gt)); break; // gt
        case 22: push(compare(ge)); break; // ge
        /********************JAMPING********************/
        case 23: pc = immediate; break; // jmp
        case 24: if(pop() == FALSE) pc = immediate; break; // brf
        case 25: if(pop() == TRUE) pc = immediate; break; // brt
        /******************FUNCTION CALL*************************/
        // Berechnung eines Offsets:   stack[-2-(Anzahl der Argumente)+(i-tes Argument)]
        case 26: push(pc/*+1*/); pc = immediate; break;       // call
        case 27: pc = pop();  break;                    // ret
        case 28: for (int i = 0; i < immediate; i++) pop();  break; // drop n, es werden n Elemente gelöscht..!!
        case 29: push(RVR);      break;           // pushr
        case 30: RVR = pop();    break;           // popr
        case 31: push(stack[sp-1]); /*int dup = pop(); push(dup); push(dup);*/    //dup
            
        default: break;        
    }

    if (cutInstructions(program_memory[pc])[0] == 0)        // halt
    {
        //return 0;
        printf("Ninja Virtual Mashine stopped\n");
        exit(0);
    }
    return 1;
}

int compare(int instr)
{
    int b = pop();
    int a = pop();
    switch (instr)
    {
        case 17: if(a == b) return TRUE; else return FALSE; break; // eq
        case 18: if(a != b) return TRUE; else return FALSE; break; // ne
        case 19: if(a < b) return TRUE; else return FALSE; break; // lt
        case 20: if(a <= b) return TRUE; else return FALSE; break; // le
        case 21: if(a > b) return TRUE; else return FALSE; break; // gt
        case 22: if(a >= b) return TRUE; else return FALSE; break; // ge
        default:
            break;
    }
    return FALSE;
}

void start(){
    while (1)                   // while true = unendliche Schleife
    {
        //print_stack();
        //print_step();
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
void push(int x) {
    //printf("-[%4s]-> pushing [%d] onto stack @sp [%d]\n", __func__, x, sp);
    //printf("Das wird gepusht %d \n",x);
    stack[sp]=x;
    //printf("-[%4s]-> inc stack pointer [%d -> ", __func__, sp);
    sp++;
    //printf("%d]\n", sp);
}
int pop(void) {
    //printf("-[%4s]-> dec stack pointer [%d -> ", __func__, sp);
    sp--;
    //printf("%d]\n", sp);
    int tmp = stack[sp];
    //printf("-[%4s]-> popping [%d] from stack @sp [%d]\n", __func__, tmp, sp);
    return tmp;
}
void print_stack(void) {
    for (int i=sp; i>=0; i--) {
        if (i == sp && i == fp)
            printf("sp, fp   --->   %3d: xxxx\n", i);
        else if (i==sp)
            printf("sp   --->   %3d: xxxx\n", i);
        else if (i == fp && stack[i] == 0)
            printf("fp   --->   %3d: xxxx\n", i);
        else if (i == fp && stack[i] != 0)
            printf("fp   --->   %3d: %4d\n", i, stack[i]);
        else if (stack[i] == 0)
            printf("%15d: xxxx\n", i); 
        else
            printf("%15d: %4d\n", i, stack[i]);
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
    stack = malloc(info[2] * 4);

    if(program_memory == NULL){
        perror("Speicherplatz falsch vergeben");
        exit(100);
    }

    SDA = malloc(info[3] * 4);                  

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
        char * input = malloc(12);
        printf("DEBUG: inspect, list, breakpoint, step, run, quit..?\n");
        scanf("%s", input);                 // read the specified action
        if (strcmp(input, "inspect") == 0)
        {
            while (1)
            {
                printf("DEBUG [inspect]: stack, data, <ret> for return..?\n");
                scanf("%s", input);
                if (strcmp(input, "data") == 0)
                {
                    printf("        --- end of data ---        \n");
                    print_step();
                    break;
                }
                else if (strcmp(input, "stack") == 0)
                {
                    print_stack();
                    printf("        --- bottom of stack ---        \n");
                    printf("--------------------------------------------------------\n");
                    print_step();
                    break;
                }
                else if (strcmp(input, "ret") == 0)
                    break;
                else
                    printf("unknown column: please try it again...\n");
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
            printf("--------------end of code-------------");
            printf("--------------------------------------------------------\n");
            print_step();
        }
        else if (strcmp(input, "breakpoint") == 0)
        {
            if (breakpoint > -1)
                    printf("DEBUG [breakpoint]: set at %d\n", breakpoint);
            printf("DEBUG [breakpoint]:  address to set, -1 to clear, <ret> for no change..?  \n");
            scanf("%s", input);
            if (strcmp(input, "ret") == 0)
                continue;

            int address = atoi(input);   
            if(address == -1)
                breakpoint = -1;
            else if(address > -1)
                breakpoint = address;
            printf("DEBUG [breakpoint]: now set at %d\n", breakpoint);
            printf("--------------------------------------------------------\n");
            print_step();
        }
        else if (strcmp(input, "step") == 0)
        {
            execute(program_memory[pc]);
            printf("--------------------------------------------------------\n");
            print_step();
        }
        else if (strcmp(input, "run") == 0)
            start();
        else if (strcmp(input, "quit") == 0)
            exit(0);
        else
            printf("unknown column: please try it again...\n");
    }
}

int main (int argc, char *argv[]) {
    
    /*if (argc < 2)
        exit(1);*/
    printf("Ninja Virtual Mashine started\n");
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
        printf("Ninja Virtual Machine version 2\n");
        exit(0);
    }
    else if (strcmp(argv[1], "--debug") == 0)
    {
        openFile(argv[2]);
        printf("DEBUG: file '%s' loaded (code size = %d, data size = %d)\n", argv[2], info[2], info[3]);
        debugmode();
    }
    else
    {
        openFile(argv[1]);
        start();
    }
    printf("Ninja Virtual Mashine stopped\n");
    return 0;
}