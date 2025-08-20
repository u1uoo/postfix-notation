#include "validate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_expression(const char* s, int* pos);
static int parse_term(const char* s, int* pos);
static int parse_factor(const char* s, int* pos);
static int parse_function(const char* s, int* pos);
static int parse_variable(const char* s, int* pos);
static int parse_number(const char* s, int* pos);
static void skip_whitespace(const char* s, int* pos);

static void skip_whitespace(const char* s, int* pos) {
    while (s[*pos] == ' ' || s[*pos] == '\t') {
        (*pos)++;
    }
}

static int parse_number(const char* s, int* pos) {
    skip_whitespace(s, pos);
    int start = *pos;
    int has_digit = 0;

    while (s[*pos] >= '0' && s[*pos] <= '9') {
        has_digit = 1;
        (*pos)++;
    }

    if (s[*pos] == '.') {
        (*pos)++;
        while (s[*pos] >= '0' && s[*pos] <= '9') {
            has_digit = 1;
            (*pos)++;
        }
    }

    if (!has_digit || *pos == start) return 0;
    return 1;
}

static int parse_variable(const char* s, int* pos) {
    skip_whitespace(s, pos);
    if (s[*pos] == 'x') {
        (*pos)++;
        return 1;
    }
    return 0;
}

static int parse_function(const char* s, int* pos) {
    skip_whitespace(s, pos);
    const char* funcs[] = {"sin", "cos", "tan", "ctg", "sqrt", "ln"};
    const int lens[] = {3, 3, 3, 3, 4, 2};
    int num_funcs = 6;

    for (int i = 0; i < num_funcs; i++) {
        int len = lens[i];
        if (strncmp(s + *pos, funcs[i], len) == 0) {
            *pos += len;
            skip_whitespace(s, pos);
            if (s[*pos] != '(') return 0;
            (*pos)++;
            if (!parse_expression(s, pos)) return 0;
            skip_whitespace(s, pos);
            if (s[*pos] != ')') return 0;
            (*pos)++;
            return 1;
        }
    }
    return 0;
}

static int parse_factor(const char* s, int* pos) {
    skip_whitespace(s, pos);
    char ch = s[*pos];
    if (ch == '\0') return 0;

    if ((ch >= '0' && ch <= '9') || ch == '.') {
        return parse_number(s, pos);
    } else if (ch == 'x') {
        return parse_variable(s, pos);
    } else if (ch >= 'a' && ch <= 'z') {
        return parse_function(s, pos);
    } else if (ch == '(') {
        (*pos)++;
        if (!parse_expression(s, pos)) return 0;
        skip_whitespace(s, pos);
        if (s[*pos] != ')') return 0;
        (*pos)++;
        return 1;
    } else if (ch == '-') {
        (*pos)++;
        return parse_factor(s, pos);
    } else {
        return 0;
    }
}

static int parse_term(const char* s, int* pos) {
    skip_whitespace(s, pos);
    if (!parse_factor(s, pos)) return 0;

    while (1) {
        skip_whitespace(s, pos);
        char ch = s[*pos];
        if (ch == '*' || ch == '/') {
            (*pos)++;
            if (!parse_factor(s, pos)) return 0;
        } else {
            break;
        }
    }
    return 1;
}

static int parse_expression(const char* s, int* pos) {
    skip_whitespace(s, pos);
    if (!parse_term(s, pos)) return 0;

    while (1) {
        skip_whitespace(s, pos);
        char ch = s[*pos];
        if (ch == '+' || ch == '-') {
            (*pos)++;
            if (!parse_term(s, pos)) return 0;
        } else {
            break;
        }
    }
    return 1;
}

int is_valid(const char* expr) {
    int pos = 0;
    int len = strlen(expr);
    if (parse_expression(expr, &pos)) {
        skip_whitespace(expr, &pos);
        return pos == len;
    }
    return 0;
}
