// gcc -g -Wall -std=c99 -pedantic -o njvm njvm.c
// VM Version 2
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
//Macro
#define MAXITEMS 100
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

#define IMMEDIATE(x)((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i)((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#define OPCODE(x)((x) >> 24)
#define VERSION 2

// <--------------------Functions(Deklaration)----------------------->
int pop();
void push(int i);
void print_stack(void);
int *cutInstructions(unsigned int instr);
void calculate(char o);
int execute(int i);
void start();
void saveInMemory(int instruction);
void print_step(void);


// ------------------------Variables--------------------->
const char *instructions[] = {"halt", "pushc", "add", "sub", "mul", "div", "mod", "rdint", "wrint", "rdchr",
 "wrchr", "pushg", "popg", "asf", "rsf", "pushl", "popl"};
int sp=0; // the stackpointer
int counter = 0;  // the counter for saving the instructions
int pc = 0; // the program counter
int fp = 0;   // frame pointer
int *SDA;
int *stack;
unsigned int *program_memory;


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
    int x = pop();
    int y = pop();
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
    int opcode = instruc[0];
    int immediate = instruc[1];
    int tempint;
    char tempchar;
    if (opcode == 0)        // halt
        return 0;
    else
    {
        switch (opcode)
        {
            case 1: push(immediate);break;//pushc
            case 2: calculate('+');break; //add
            case 3: calculate('-');break; //sub
            case 4: calculate('*');break; //mul
            case 5: calculate('/');break; //div
            case 6: calculate('%');break; //mod
            case 7: scanf("%d", &tempint); printf("Der Wert %d \n",tempint);push(tempint); break;// rdint
            case 8: printf("%d", pop());break;// wrint
            case 9: scanf("%c", &tempchar); push(tempchar); break;// rdchr
            case 10: printf("%c", pop());;break;// wrchr
            case 11: push(SDA[immediate]);break;    // pushg
            case 12: SDA[immediate] = pop();break;  // popg
            case 13: push(fp); fp = sp; sp += immediate;break;     // asf
            case 14: sp = fp; fp = pop();break;     // rsf
            case 15: push(stack[fp+immediate]);break;     // pushl
            case 16: printf("an der Stelle wird gepusht: %d", fp+immediate); stack[fp+immediate] = pop();break;     // popl
            default: break;
        }
        return 1;
    }
}

void start(){
    while (1)                   // while true = unendliche Schleife
    {
        print_stack();
        print_step();
        if (execute(program_memory[counter]) == 0)  // wenn die Instruktion halt vorkommt
        {
            free(program_memory);           // program memory wird freigegeben
            break;
        }
        pc++;
    }
}

void print_step(void){
    printf("\n ASSEMBLER     |  BYTECODE  \n");
    printf(".--------------+------------------.\n");
    int i = 0;
    while (1)
    {
        int *instruc = cutInstructions(program_memory[i]);
        int opcode = instruc[0];
        int immediate = instruc[1];
        if (i == pc)
        {
            if (immediate == 0)
                printf("pc->%8s  |  0x%08x\n", instructions[opcode], program_memory[i]);
            else
                printf("pc->%8s %d |  0x%08x\n", instructions[opcode], immediate, program_memory[i]);
        }
        else
        {
            if (immediate == 0)
                printf("%12s   |  0x%08x\n", instructions[opcode], program_memory[i]);
            else
                printf("%12s %d |  0x%08x\n", instructions[opcode], immediate, program_memory[i]);
        }
        if (opcode == 0)
            break;
        i++;
    }
    printf("------------------------------------\n");
}

void saveInMemory(int instruction){
    program_memory[counter] = instruction;
    counter++;
}

// Stack implementaion
void push(int x) {
    //printf("-[%4s]-> pushing [%d] onto stack @sp [%d]\n", __func__, x, sp);
    printf("Das wird gepusht %d \n",x);
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
    printf("\n Stack\n");
    printf(".-------+--------.\n");
    for (int i=sp; i>=0; i--) {
        if (i==sp)
            printf("|sp->%3d| <empty>|\n", i);
        else if (i == fp && stack[i] == 0)
            printf("|fp->%3d| <empty>|\n", i);    
        else if (stack[i] == 0)
            printf("|%7d| <empty>|\n", i);
        else
            printf("|%7d| %5d |\n", i, stack[i]);
    }
    printf("'-------+--------'\n\n");
}

void openFile(char filename[])
{
    FILE * filep = NULL;
    if ((filep = fopen(filename, "r")) == NULL)
    {
        perror("ERROR - Die Datei kann nicht geöffnet werden");
        exit(1);
    }

    unsigned int info [4];
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

int main (int argc, char *argv[]) {
    if(strcmp(argv[1], "--help") == 0)
    {
        printf("usage: ./njvm [option] [option] ...\n"
               "--prog1          select program 1 to execute\n"
               "--prog2          select program 2 to execute\n"
               "--prog3          select program 3 to execute\n"
               "--version        show version and exit\n"
               "--help           show this help and exit\n");
        exit(0);
    }
    else if(strcmp(argv[1], "--version") == 0)
    {
        printf("Ninja Virtual Machine version 2\n");
        exit(0);
    }
    else if (strcmp(argv[1], "--prog1") == 0)
    {
        openFile("./prog1.bin");
        start();
    }
    else if (strcmp(argv[1], "--prog2") == 0)
    {
        openFile("./prog2.bin");
        start();
    }
    else if (strcmp(argv[1], "--prog3") == 0)
    {
        openFile("./prog3.bin");
        start();
    }
    else if (strcmp(argv[1], "--prog4") == 0)
    {
        openFile("./prog4.bin");
        start();
    }
    else if (strcmp(argv[1], "--prog5") == 0)
    {
        openFile("./prog5.bin");
        start();
    }
    else
    {
        /*saveInMemory((PUSHC << 24) | 4);                  !!zum testen der neuen Instruktionen!!
        saveInMemory((ASF << 24) | 2);
        saveInMemory((PUSHC << 24) | 7);
        saveInMemory((POPL << 24) | 1);
        saveInMemory((PUSHL << 24) | 1);
        saveInMemory((HALT << 24) | 0);
        start();*/
        printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
    }
    return 0;
}