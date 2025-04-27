#include <stdio.h>

int adjust(int x) {
    if (x > 10)
        x = x - 10;
    else
        x = x + 10;
    return x;
}

int classify(int x) {
    if (x < 0)
        x = -1;
    else if (x == 0)
        x = 0;
    else
        x = 1;
    return x;
}

int main(void) {
    int a = adjust(5);
    int b = adjust(20);

    int ca = classify(a);
    int cb = classify(b);

    printf("a = %d, classify(a) = %d\n", a, ca);
    printf("b = %d, classify(b) = %d\n", b, cb);

    return 0;
}