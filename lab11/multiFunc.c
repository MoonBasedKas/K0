#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int square(int x) {
    return x * x;
}


int square_of_sum(int a, int b) {
    int s = add(a, b);
    return square(s);
}

int main() {
    int x = 2, y = 3;

    int s = add(x, y);
    printf("add(%d, %d) = %d\n", x, y, s);

    int sq = square(s);
    printf("square(%d) = %d\n", s, sq);

    int sos = square_of_sum(x, y);
    printf("square_of_sum(%d, %d) = %d\n", x, y, sos);

    return 0;
}