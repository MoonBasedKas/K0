#include "tree.h"
#include "lex.h"
#include "k0gram.tab.h"
#include "symTab.h"
#include "symNonTerminals.h"
#include "type.h"
#include "typeHelpers.h"
#include "symTabHelper.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int serial = 1;
extern int symError;
struct tree *root = NULL;

struct symTab *currentScope;

/**
 * @brief Prints the tree
 *
 * @param root
 * @param depth
 * @return int
 */
int printTree(nodeptr root, int depth) {
    static int last[256];
    if (root == NULL)
        return 0;
    if (depth == 0) {
        memset(last, 0, sizeof(last));
    }

    char prefix[1024] = "";
    for (int i = 0; i < depth - 1; i++) {
        if (last[i])
            strcat(prefix, "    ");
        else
            strcat(prefix, "│   ");
    }

    if (depth > 0) {
        if (last[depth - 1])
            strcat(prefix, "└── ");
        else
            strcat(prefix, "├── ");
    }

    if (root->nkids > 0)
        printf("%snode (%s, %d): %d children | Type %s\n", prefix, root->symbolname, root->prodrule, root->nkids, typeName(root->type)); //typeHelpers.c
    else
        printf("%stoken (File: %s, Line: %d):  %s  Integer Code: %d | Type %s\n", prefix, root->leaf->filename, root->leaf->lineno,
               root->leaf->text, root->leaf->category, typeName(root->type)); //typeHelpers.c

    // recurse for each child: update the "last" array.
    for (int i = 0; i < root->nkids; i++) {
        last[depth] = (i == root->nkids - 1);
        printTree(root->kids[i], depth + 1);
    }
    return 0;
}

/**
 * @brief Alocates a token.
 *
 * @param prodrule
 * @param symbolname
 * @param nkids
 * @param ...
 * @return struct tree*
 */
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
    node->id = serial++;

    va_list args;
    va_start(args, nkids);
    for (int i = 0; i < nkids; i++) {
        node->kids[i] = va_arg(args, struct tree *);
    }
    va_end(args);
    node->table = NULL;  // Done for debugging
    return node;
}

/**
 * @brief Frees the whole tree...
 *
 * @param node
 */
void freeTree(nodeptr node) {
    if (node == NULL)
        return;

    for (int i = 0; i < node->nkids; i++) {
        freeTree(node->kids[i]);
    }

    free(node->symbolname);
    if (node->leaf) {
        free(node->leaf->text);
        free(node->leaf->filename);
        if (node->leaf->sval) {
            free(node->leaf->sval);
        }
        free(node->leaf);
    }
    free(node);
}