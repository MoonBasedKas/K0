#include <stdio.h>
#include <stdbool.h>

int main() {
    bool arr[2] = {true, false};
    arr[1] = !arr[0];
    printf("%d %d\n", arr[0], arr[1]);
    return 0;
}