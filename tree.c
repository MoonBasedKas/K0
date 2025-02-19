#include "tree.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

struct tree *alctoken(int prodrule, char* symbolname, int nkids, ...){
    struct tree *node = malloc(sizeof(struct tree));
    if (!node) {
        fprintf(stderr, "Error: Failed to allocate memory for tree node\n");
        exit(1);
    }

    node->prodrule = prodrule;
    node->symbolname = strdup(symbolname);
    node->nkids = nkids;
    for (int i = 0; i < nkids; i++) {
        node->kids[i] = NULL;
    }
    node->leaf = NULL;

    va_list args;
    va_start(args, nkids);
    for (int i = 0; i < nkids; i++) {
        node->kids[i] = va_arg(args, struct tree *);
    }
    va_end(args);

    return node;
}