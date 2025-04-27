#include <stdio.h>
#include <string.h>

int main() {
    const char *s = "test";
    int eq = strcmp(s, "test") == 0;
    size_t len = strlen(s);
    printf("%d %zu\n", eq, len);
    return 0;
}