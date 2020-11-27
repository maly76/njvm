// gcc -g -Wall -std=c99 -pedantic -o njvm njvm.c
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
#define IMMEDIATE(x)((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i)((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#define OPCODE(x)((x) >> 24)

// <--------------------Functions(Deklaration)----------------------->
int pop();
void push(int i);
void print_stack(void);
int *cutInstructions(unsigned int instr);
void calculate(char o);
int execute(int i);
void start(int programnumber);
void saveInMemory(int instruction);
void print_step(void);


// ------------------------Variables--------------------->
int sp=0; // the stackpointer
int stack[MAXITEMS];
const char *instructions[] = {"halt", "pushc", "add", "sub", "mul", "div", "mod", "rdint", "wrint", "rdchr", "wrchr"};
unsigned int program_memory[100];
int counter = 0;
int pc = 0;

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
    if (opcode == 0)
    {
        return 0;
    }
    else
    {
        switch (opcode)
        {
        case 0: exit(0);break; //halt
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
        default: break;
        }
        return 1;
    }
}

void start(int programnumber){
    if (programnumber == 1)

    {
        int operationsP1[] = {PUSHC, PUSHC, ADD, PUSHC, PUSHC, SUB, MUL, WRINT, PUSHC, WRCHR, HALT};
        int numbersP1[] = {3, 4, 0, 10, 6, 0, 0, 0, 10, 0, 0};
        for (size_t i = 0; i <= sizeof(numbersP1)/sizeof(numbersP1[0]) - 1; i++)
        {
            saveInMemory((operationsP1[i] << 24) | IMMEDIATE(numbersP1[i]));
        }
    }
    else if (programnumber == 2)
    {
        int operationsP2[] = {PUSHC, RDINT, MUL, PUSHC, ADD, WRINT, PUSHC, WRCHR, HALT};
        int numbersP2[] = {-2, 0, 0, 3, 0, 0, 0, 0, 0};
        for (size_t j = 0; j <= sizeof(numbersP2)/sizeof(numbersP2[0]) - 1; j++)
        {
            saveInMemory((operationsP2[j] << 24)  | IMMEDIATE(numbersP2[j]));
        }
    }
    else if (programnumber == 3)
    {
        int operationsP3[] = {RDCHR, WRINT, PUSHC, WRCHR, HALT};
        int numbersP3[] = {0, 0, 0, 0, 0};
        for (size_t m = 0; m <= sizeof(numbersP3)/sizeof(numbersP3[0]) - 1; m++)
        {
          saveInMemory((operationsP3[m]  << 24)  | IMMEDIATE(numbersP3[m]));
        }
    }

    while (1)                   // while true = unendliche Schleife
    {
        print_stack();
        print_step();
        if (execute(program_memory[counter]) == 0)  // wenn die Instruktion halt vorkommt
        {
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

    for (int i = 0; i <= 10; i++)
    {
    }
}

void saveInMemory(int instruction){
    printf("%d", instruction);
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
else
printf("|%7d| %5d |\n", i, stack[i]);
}
printf("'-------+--------'\n\n");
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
        printf("Ninja Virtual Machine version 1\n");
        exit(0);
    }
    else if (strcmp(argv[1], "--prog1") == 0)
    {

        start(1);
    }
    else if (strcmp(argv[1], "--prog2") == 0)
    {
        start(2);
    }
    else if (strcmp(argv[1], "--prog3") == 0)
    {
        start(3);
    }
    else
    {
        printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
    }
    return 0;
}