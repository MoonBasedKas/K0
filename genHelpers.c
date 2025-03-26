#include <stdlib.h>
#include <string.h>

char *my_strdup(const char *s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;  // +1 for the null terminator
    char *dup = malloc(len);
    if (dup != NULL) {
        memcpy(dup, s, len);
    }
    return dup;
}