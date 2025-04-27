#include <stdio.h>
#include <stdbool.h>

int main() {
    int  a     = 10;
    int  b     = 3;
    bool logic = (a > 5 && b < 5) || !(a == 10);
    int  mix   = (a + b) - (a * b) + ((a % b) * (a - b));
    printf("logic=%d mix=%d\n", logic, mix);
    return 0;
}