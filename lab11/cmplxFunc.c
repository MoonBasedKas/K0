#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool foo() {
    return true;
}

char* bar() {
    return "Hello, World!";
}

int main(int argc, char* argv[]) {
    int a = 1;
    int b = 2;
    int c = a + b;
    int d = c * b;
    double e = d / 2.0;
    bool f = foo();
    char* g = bar();
    printf(" Did e compute? %d\n", e);
    printf("%s\n", g);
    // God speed little buddy
    return e;
}


