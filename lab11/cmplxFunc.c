#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int x, y, z;
    x = 1;
    y = 2;
    z = 3;
    if (x < y || y < z) {
        printf("Hello, World!\n");
    }
    else {
        printf("Goodbye, World!\n");
    }
    return 0;
}


