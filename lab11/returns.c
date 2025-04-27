#include <stdio.h>

int return_const() {
    return 42;
}

int return_expr(int x, int y) {
    return x * 2 + y;
}

int return_cond(int x) {
    if (x < 0)
        return -x;
    return x;
}

int main() {
    int a = return_const();
    int b = return_expr(3, 4);
    int c = return_cond(-5);
    printf("%d %d %d\n", a, b, c);
    return 0;
}