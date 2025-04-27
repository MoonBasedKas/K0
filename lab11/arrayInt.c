#include <stdio.h>

int get_first(int arr[]) {
    return arr[0];
}

int main() {
    int arr[] = {1, 2, 3};
    int x = get_first(arr);
    printf("%d\n", x);
    return 0;
}