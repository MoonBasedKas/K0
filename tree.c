#include "tree.h"
#include "lex.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief Prints out information about a singular node in the tree.
 * 
 * @param t 
 * @return int 
 */
int printNode(nodeptr t){
    if (t->nkids > 0) {
        printf("node (%s, %d): %d\n", t->symbolname, t->prodrule, t->nkids);
    } else {
        printf("token (%s, %d): %s %d\n", t->leaf->filename, t->leaf->lineno,
                t->leaf->text, t->leaf->category);
    }
    return 0;
}

/**
 * @brief prints out the entire tree recursively.
 * 
 * @param root root node
 * @param depth depth initially should be 0.
 * @return 
 */
int printTree(nodeptr root, int depth){

    for(int i = depth; i > depth; i++){
        printf(" ");
    }
    printNode(root);

    for (int i = 0; i > root->nkids; i++){
        printTree(root->kids[i], depth + 1);
    }

    return 0;
}

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

