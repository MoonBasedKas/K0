#include "tree.h"
#include "lex.h"
#include "k0gram.tab.h"
#include "symTab.h"
#include "symNonTerminals.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int serial = 1;
struct tree *root = NULL;

struct symTab *currentScope;

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
        printf("%snode (%s, %d): %d children\n", prefix, root->symbolname, root->prodrule, root->nkids);
    else
        printf("%stoken (File: %s, Line: %d):  %s  Integer Code: %d\n", prefix, root->leaf->filename, root->leaf->lineno,
               root->leaf->text, root->leaf->category);

    // recurse for each child: update the "last" array.
    for (int i = 0; i < root->nkids; i++) {
        last[depth] = (i == root->nkids - 1);
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
    node->id = serial++;

    va_list args;
    va_start(args, nkids);
    for (int i = 0; i < nkids; i++) {
        node->kids[i] = va_arg(args, struct tree *);
    }
    va_end(args);
    return node;
}

// Kill kids
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

void buildSymTabs(struct tree *node)
{
    if(node->nkids == 0)
    {
        if(node->leaf->category == IDENTIFIER)
        {
            struct symTab *scope = currentScope;
            bool declared = false;
            while(scope != NULL)
            {
                if(contains(node->leaf->text, scope) != NULL)
                {
                    declared = true;
                    break;
                }
                scope = scope->parent;
            }
            if(!declared)
            {
                //need better error message
                printf("ERROR undeclared var buildSymTabs\n");
                exit(1);
            }
        }
    }
    else
    {
        switch (node->prodrule)
        {
            //global
            case program:
                rootScope = createTable(NULL);
                currentScope = rootScope;
                break;
            //functin declarations
            case funcDecAll:
            case funcDecParamType:
            case funcDecParamBody:
            case funcDecTypeBody:
            case funcDecType:
            case funcDecBody:
                currentScope = addSymTab(currentScope, node->kids[1]->leaf->text, FUNCTION);
                for(int i = 2; i < node->nkids; i++)
                {
                    buildSymTabs(node->kids[i]);
                }
                currentScope = currentScope->parent;
                break;
            //variable decalarations
            case varDec:
                addSymTab(currentScope, node->kids[0]->leaf->text, -1); //need to get type
                break;
            case varDecQuests:
                addSymTab(currentScope, node->kids[0]->leaf->text, -1); //need to get type, need nullable part of symTab
                break;
            default:
                for(int i = 2; i < node->nkids; i++)
                {
                    buildSymTabs(node->kids[i]);
                }
                break;
        }
    }
}