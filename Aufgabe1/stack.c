#include <stdlib.h>
#include <stdio.h>

#define MAXITEMS 100
int sp=0; // the stackpointer
int stack[MAXITEMS];
void push(int x) {
printf("-[%4s]-> pushing [%d] onto stack @sp [%d]\n", __func__, x, sp);
stack[sp]=x;
printf("-[%4s]-> inc stack pointer [%d -> ", __func__, sp);
sp++;
printf("%d]\n", sp);
}
int pop(void) {
printf("-[%4s]-> dec stack pointer [%d -> ", __func__, sp);
sp--;
printf("%d]\n", sp);
int tmp = stack[sp];
printf("-[%4s]-> popping [%d] from stack @sp [%d]\n", __func__, tmp, sp);
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

