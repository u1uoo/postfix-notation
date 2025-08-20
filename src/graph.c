#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpn.h"
#include "validate.h"

#define FIELD_WIDTH 80
#define FIELD_HEIGHT 25
#define ORIGIN_X 0
#define ORIGIN_Y 12
#define X_MIN 0.0
#define X_MAX (4 * M_PI)
#define Y_MIN -1.0
#define Y_MAX 1.0

void world_to_screen(double x, double y, int* screen_x, int* screen_y) {
    *screen_x = (int)round((x - X_MIN) / (X_MAX - X_MIN) * (FIELD_WIDTH - 1));
    *screen_y = (int)round((Y_MAX - y) / (Y_MAX - Y_MIN) * (FIELD_HEIGHT - 1));
}

int is_within_bounds(int x, int y) { return x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT; }

void init_field(char field[FIELD_HEIGHT][FIELD_WIDTH]) {
    int i, j;
    for (i = 0; i < FIELD_HEIGHT; i++) {
        for (j = 0; j < FIELD_WIDTH; j++) {
            field[i][j] = '.';
        }
    }
}

int plot_point(char field[FIELD_HEIGHT][FIELD_WIDTH], double world_x, double y_value) {
    int screen_x, screen_y;

    if (y_value < Y_MIN || y_value > Y_MAX) {
        return 0;
    }

    world_to_screen(world_x, y_value, &screen_x, &screen_y);

    if (is_within_bounds(screen_x, screen_y)) {
        field[screen_y][screen_x] = '*';
        return 1;
    }

    return 0;
}

void plot_graph(char field[FIELD_HEIGHT][FIELD_WIDTH], char** rpn_expr, int rpn_size) {
    int screen_x;

    for (screen_x = 0; screen_x < FIELD_WIDTH; screen_x++) {
        double world_x = X_MIN + (screen_x * (X_MAX - X_MIN)) / (FIELD_WIDTH - 1);
        double res = evaluate_rpn(rpn_expr, rpn_size, world_x);

        if (isfinite(res)) {
            plot_point(field, world_x, res);
        }
    }
}

void print_field(char field[FIELD_HEIGHT][FIELD_WIDTH]) {
    int i, j;
    for (i = 0; i < FIELD_HEIGHT; i++) {
        for (j = 0; j < FIELD_WIDTH; j++) {
            printf("%c", field[i][j]);
        }
        printf("\n");
    }
}

void draw_graph(const char* expression) {
    if (!is_valid(expression)) {
        printf("n/a");
        return;
    }

    char field[FIELD_HEIGHT][FIELD_WIDTH];
    init_field(field);

    int rpn_size = 0;
    char** rpn_expr = infix_to_rpn(expression, &rpn_size);

    if (rpn_expr == NULL) {
        printf("n/a");
        return;
    }

    plot_graph(field, rpn_expr, rpn_size);
    free_rpn(rpn_expr, rpn_size);
    print_field(field);
}

int main() {
    char expression[256];

    if (fgets(expression, sizeof(expression), stdin) == NULL) {
        printf("n/a");
        return 0;
    }

    size_t len = strlen(expression);
    if (len > 0 && expression[len - 1] == '\n') {
        expression[len - 1] = '\0';
    }

    draw_graph(expression);
    return 0;
}