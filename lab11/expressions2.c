#include <stdio.h>
#include <stdbool.h>

int main() {
    int a    = 10;
    int b    = 3;
    int quot = a / b;
    int mod  = a % b;
    bool gt  = a > b;
    bool eq  = a == b;
    printf("quot=%d mod=%d gt=%d eq=%d\n", quot, mod, gt, eq);
    return 0;
}