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
    node->parent = NULL;

    va_list args;
    va_start(args, nkids);
    for (int i = 0; i < nkids; i++) {
        node->kids[i] = va_arg(args, struct tree *);
    }
    va_end(args);

    for (int i = 0; i < nkids; i++) {
        if (node->kids[i] != NULL) {
            node->kids[i]->parent = node;
        }
    }

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


int assignMutability(struct tree *root){
    struct tree *temp = NULL;
    struct tree *id = NULL;
    for(int i = 0; i < root->nkids; i++){
        assignMutability(root->kids[i]);
    }
    switch(root->prodrule){
        // case propDecEmpty: Can't be unmutabble
        // case propDecTypeless:
        case propDecAssign:
            temp = root->kids[0];
            id = root->kids[1];
            if (!(temp->nkids == 0 && temp->leaf->category == VAR))
            makeEntryNonMutable(id->table, id->kids[0]->leaf->text);
            break;
        case arrayDec:
        case arrayDecValueless:
        case arrayDecEqual:
        case arrayDecEqualValueless:
            temp = root->kids[0];
            id = root->kids[1];
            if (!(temp->kids == 0 && temp->leaf->category == VAR))
            makeEntryNonMutable(id->table, id->kids[0]->leaf->text);
            break;
        default:
            break;
    }
    return 0;
}


/**
 * @brief A C hacker type method of pushing types up the tree. We just 
 * smash and dash everything that isn't a type after its been assigned
 * and forcibly take the type of the first element or leaf.
 * 
 * The digging for guns of kotlin! -H.S.
 * 
 * @param root 
 * @return int 
 */
int typeTheft(struct tree *root){
    for(int i = 0; i < root->nkids; i++) typeTheft(root->kids[i]);
    int assigned = 0;
    if (root->type == NULL && root->nkids!= 0){
        for (int i = 0; i < root->nkids; i++){
            if (root->kids[i]->type->basicType != UNIT_TYPE){
                root->type = root->kids[0]->type;
                assigned = 1;
                break;
            } 
        }
        if (!assigned) root->type = alcType(UNIT_TYPE);
    } else if (root->type == NULL){
        switch (root->leaf->category)
        {
        case INTEGER_LITERAL:
            root->type = alcType(INT_TYPE);
            break;

        case CHARACTER_LITERAL:
            root->type=alcType(CHARACTER_LITERAL);
            break;

        case REAL_LITERAL:
            root->type=alcType(DOUBLE_TYPE);
            break;

        case FALSE:
        case TRUE:
            root->type=alcType(BOOL_TYPE);
            break;

        case NULL_K:
            root->type=alcType(NULL_TYPE);
            break;

        case LINE_STRING:
        case MULTILINE_STRING:
            root->type=alcType(STRING_TYPE);
            break;
        
        case IDENTIFIER:
            struct symEntry *temp = contains(root->table, root->leaf->text);
                if (temp != NULL)
                    root->type = alcType(temp->type->basicType);


        default:
            root->type = alcType(UNIT_TYPE);
            break;
        }
        return 0;
    }
    return 0;
}

/**
 * @brief Grabs variables types and assigns them in all places where they appear in the tree.
 * The other version of digging for guns in kotlin! -H.S.
 * 
 * @param root 
 * @return int 
 */
int varTypeTheft(struct tree *root){
    for (int i = 0; i < root->nkids; i++){
        varTypeTheft(root->kids[i]);
    }
    if (root->nkids == 0){
        struct symEntry *temp;
        if (root->leaf->category == IDENTIFIER){
            temp = contains(root->table, root->leaf->text);
            if (temp != NULL){
                if (temp->type->basicType == FUNCTION_TYPE){
                    root->type = temp->type;
                } else if (temp->type->basicType == ARRAY_TYPE) {
                    root->type = temp->type;
                }else {
                    root->type = alcType(temp->type->basicType);
                }
            } else if (!strcmp(root->leaf->text, "Array")){
                root->type = arrayAnyType_ptr;
            }
        }
    }
    return 0;
}