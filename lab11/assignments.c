#include <stdio.h>

int main(void) {
    int a = 5;
    int b;
    b = a;
    a += 3;
    b -= 2;
    int c = a % 3;

    a++;
    b--;

    printf("a=%d, b=%d, c=%d\n", a, b, c);
    return 0;
}