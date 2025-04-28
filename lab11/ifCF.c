#include <stdio.h>
#include <stdbool.h>

bool troof(int x, int y) {
    if (x < y)
        return true;
}

bool troofhurts(double x, double y) {
    if (x != y)
        return false;
}

void main() {
   if(troof(1, 2) == true)
   {
    printf("Moana was an ok movie\n");
   }
   if(troofhurts(1, 1) == false) {
    printf("rip\n"); }
}