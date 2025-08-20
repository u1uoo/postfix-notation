#ifndef STACK_H
#define STACK_H

typedef struct Stack Stack;

Stack* stack_new();
void stack_push(Stack* s, char* item);
char* stack_pop(Stack* s);
char* stack_top(Stack* s);
int stack_empty(Stack* s);
void stack_free(Stack* s);

#endif