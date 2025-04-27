#include <stdio.h>

int main() {
    char buf[64];
    fgets(buf, 64, stdin);
    printf("%s", buf);
    printf("%s\n", buf);
    return 0;
}