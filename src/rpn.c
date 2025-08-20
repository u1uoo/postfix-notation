#include "rpn.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

int my_isdigit(char ch) { return ch >= '0' && ch <= '9'; }

int my_isalpha(char ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }

int my_isspace(char ch) { return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'; }

static int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    if (strchr("sctl", op)) return 4;
    return 0;
}

static int is_operator(char ch) { return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^'; }

static int is_function(const char* token) {
    return strcmp(token, "sin") == 0 || strcmp(token, "cos") == 0 || strcmp(token, "tan") == 0 ||
           strcmp(token, "ctg") == 0 || strcmp(token, "sqrt") == 0 || strcmp(token, "ln") == 0;
}

static void cleanup_and_return_null(char** output, int rpn_size, Stack* stack) {
    int i;
    for (i = 0; i < rpn_size; i++) {
        free(output[i]);
    }
    free(output);
    stack_free(stack);
}

static int process_number(const char* expr, int* pos, int len, char** output, int* rpn_size) {
    int start = *pos;
    while (*pos < len && (my_isdigit(expr[*pos]) || expr[*pos] == '.')) {
        (*pos)++;
    }
    char* num = malloc(*pos - start + 1);
    strncpy(num, expr + start, *pos - start);
    num[*pos - start] = '\0';
    output[(*rpn_size)++] = num;
    return 1;
}

static int process_variable(char** output, int* rpn_size) {
    char* var = malloc(2);
    var[0] = 'x';
    var[1] = '\0';
    output[(*rpn_size)++] = var;
    return 1;
}

static int process_function(const char* expr, int* pos, int len, char** output, int* rpn_size, Stack* stack) {
    int start = *pos;
    while (*pos < len && my_isalpha(expr[*pos])) {
        (*pos)++;
    }
    char* func = malloc(*pos - start + 1);
    strncpy(func, expr + start, *pos - start);
    func[*pos - start] = '\0';

    if (is_function(func)) {
        stack_push(stack, func);
        return 1;
    } else {
        free(func);
        cleanup_and_return_null(output, *rpn_size, stack);
        return 0;
    }
}

static int process_parenthesis(const char* expr, int* pos, char** output, int* rpn_size, Stack* stack) {
    if (expr[*pos] == '(') {
        char* paren = malloc(2);
        paren[0] = '(';
        paren[1] = '\0';
        stack_push(stack, paren);
        (*pos)++;
        return 1;
    }

    if (expr[*pos] == ')') {
        while (!stack_empty(stack) && strcmp(stack_top(stack), "(") != 0) {
            output[(*rpn_size)++] = stack_pop(stack);
        }
        if (stack_empty(stack)) {
            cleanup_and_return_null(output, *rpn_size, stack);
            return 0;
        }
        free(stack_pop(stack));

        if (!stack_empty(stack) && is_function(stack_top(stack))) {
            output[(*rpn_size)++] = stack_pop(stack);
        }
        (*pos)++;
        return 1;
    }

    return 0;
}

static int process_operator(const char* expr, int* pos, char** output, int* rpn_size, Stack* stack) {
    if (!is_operator(expr[*pos])) {
        return 0;
    }

    char* op = malloc(2);
    op[0] = expr[*pos];
    op[1] = '\0';

    while (!stack_empty(stack) && precedence(stack_top(stack)[0]) >= precedence(op[0])) {
        output[(*rpn_size)++] = stack_pop(stack);
    }
    stack_push(stack, op);
    (*pos)++;
    return 1;
}

char** infix_to_rpn(const char* expr, int* rpn_size) {
    Stack* stack = stack_new();
    char** output = malloc(sizeof(char*) * strlen(expr) * 2);
    *rpn_size = 0;
    int pos = 0;
    int len = strlen(expr);

    while (pos < len) {
        if (my_isspace(expr[pos])) {
            pos++;
            continue;
        }

        if (my_isdigit(expr[pos]) || expr[pos] == '.') {
            if (!process_number(expr, &pos, len, output, rpn_size)) {
                return NULL;
            }
            continue;
        }

        if (expr[pos] == 'x') {
            if (!process_variable(output, rpn_size)) {
                return NULL;
            }
            pos++;
            continue;
        }

        if (my_isalpha(expr[pos])) {
            if (!process_function(expr, &pos, len, output, rpn_size, stack)) {
                return NULL;
            }
            continue;
        }

        if (process_parenthesis(expr, &pos, output, rpn_size, stack)) {
            continue;
        }

        if (process_operator(expr, &pos, output, rpn_size, stack)) {
            continue;
        }

        cleanup_and_return_null(output, *rpn_size, stack);
        return NULL;
    }

    while (!stack_empty(stack)) {
        if (strcmp(stack_top(stack), "(") == 0) {
            cleanup_and_return_null(output, *rpn_size, stack);
            return NULL;
        }
        output[(*rpn_size)++] = stack_pop(stack);
    }

    stack_free(stack);
    return output;
}

double evaluate_rpn(char** rpn, int rpn_size, double x_value) {
    Stack* stack = stack_new();

    for (int i = 0; i < rpn_size; i++) {
        char* token = rpn[i];

        if (my_isdigit(token[0]) || (token[0] == '-' && my_isdigit(token[1]))) {
            double num = atof(token);
            char* num_str = malloc(32);
            sprintf(num_str, "%f", num);
            stack_push(stack, num_str);
            continue;
        }

        if (strcmp(token, "x") == 0) {
            char* num_str = malloc(32);
            sprintf(num_str, "%f", x_value);
            stack_push(stack, num_str);
            continue;
        }

        if (is_operator(token[0])) {
            if (stack_empty(stack)) {
                stack_free(stack);
                return NAN;
            }
            char* b_str = stack_pop(stack);
            double b = atof(b_str);
            free(b_str);

            if (stack_empty(stack)) {
                stack_free(stack);
                return NAN;
            }
            char* a_str = stack_pop(stack);
            double a = atof(a_str);
            free(a_str);

            double result;

            switch (token[0]) {
                case '+':
                    result = a + b;
                    break;
                case '-':
                    result = a - b;
                    break;
                case '*':
                    result = a * b;
                    break;
                case '/':
                    result = a / b;
                    break;
                case '^':
                    result = pow(a, b);
                    break;
                default:
                    stack_free(stack);
                    return NAN;
            }

            char* res_str = malloc(32);
            sprintf(res_str, "%f", result);
            stack_push(stack, res_str);
            continue;
        }

        if (is_function(token)) {
            if (stack_empty(stack)) {
                stack_free(stack);
                return NAN;
            }
            char* a_str = stack_pop(stack);
            double a = atof(a_str);
            free(a_str);

            double result;

            if (strcmp(token, "sin") == 0)
                result = sin(a);
            else if (strcmp(token, "cos") == 0)
                result = cos(a);
            else if (strcmp(token, "tan") == 0)
                result = tan(a);
            else if (strcmp(token, "ctg") == 0)
                result = 1.0 / tan(a);
            else if (strcmp(token, "sqrt") == 0)
                result = sqrt(a);
            else if (strcmp(token, "ln") == 0)
                result = log(a);
            else {
                stack_free(stack);
                return NAN;
            }

            char* res_str = malloc(32);
            sprintf(res_str, "%f", result);
            stack_push(stack, res_str);
            continue;
        }
    }

    if (stack_empty(stack)) {
        stack_free(stack);
        return NAN;
    }

    char* final_str = stack_pop(stack);
    double final_result = atof(final_str);
    free(final_str);

    while (!stack_empty(stack)) {
        char* remaining = stack_pop(stack);
        free(remaining);
    }

    stack_free(stack);
    return final_result;
}

void free_rpn(char** rpn, int rpn_size) {
    for (int i = 0; i < rpn_size; i++) {
        free(rpn[i]);
    }
    free(rpn);
}