#include "stack.h"

#include <stdlib.h>

struct Stack {
    char** items;
    int top;
    int capacity;
};

Stack* stack_new() {
    Stack* s = malloc(sizeof(Stack));
    s->capacity = 100;
    s->items = malloc(sizeof(char*) * s->capacity);
    s->top = -1;
    return s;
}

void stack_push(Stack* s, char* item) {
    if (s->top + 1 >= s->capacity) {
        s->capacity *= 2;
        s->items = realloc(s->items, sizeof(char*) * s->capacity);
    }
    s->items[++s->top] = item;
}

char* stack_pop(Stack* s) {
    if (s->top < 0) return NULL;
    return s->items[s->top--];
}

char* stack_top(Stack* s) {
    if (s->top < 0) return NULL;
    return s->items[s->top];
}

int stack_empty(Stack* s) { return s->top < 0; }

void stack_free(Stack* s) {
    free(s->items);
    free(s);
}