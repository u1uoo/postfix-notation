#ifndef RPN_H
#define RPN_H

#include <stdlib.h>

int my_isdigit(char ch);
int my_isalpha(char ch);
int my_isspace(char ch);

char** infix_to_rpn(const char* expr, int* rpn_size);
double evaluate_rpn(char** rpn, int rpn_size, double x_value);
void free_rpn(char** rpn, int rpn_size);

#define RPN_SUCCESS 0
#define RPN_INVALID_EXPRESSION 1
#define RPN_MEMORY_ERROR 2
#define RPN_MISMATCHED_PARENTHESES 3

#endif