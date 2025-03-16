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

/**
 * @brief Runs through the tree interesting into the table.
 * 
 * @param node 
 */
void buildSymTabs(struct tree *node, struct symTab *scope)
{
    if(node->nkids == 0)
    {
        handleLeaves(node, scope);
    }
    else
    {
        switch (node->prodrule)
        {
            //global
            case program:

                for (int i = 0; i < node->nkids; i++) {
                    buildSymTabs(node->kids[i], scope);
                }

                break;
            //function declarations
            case funcDecAll:
            case funcDecParamType:
                scope = addSymTab(scope, node->kids[1]->leaf->text, node->kids[3], FUNCTION);
                for(int i = 2; i < node->nkids; i++)
                {
                    buildSymTabs(node->kids[i], scope);
                }
                if (scope->parent != NULL) scope = scope->parent;
                break;

            case funcDecTypeBody:    
            case funcDecType:
                scope = addSymTab(scope, node->kids[1]->leaf->text, node->kids[2], FUNCTION);
                for(int i = 2; i < node->nkids; i++)
                {
                    buildSymTabs(node->kids[i], scope);
                }
                
                if (scope->parent != NULL) scope = scope->parent;
                break;

            case funcDecParamBody:
            case funcDecBody:

                scope = addSymTab(scope, node->kids[1]->leaf->text, NULL, FUNCTION);
                for(int i = 2; i < node->nkids; i++)
                {
                    buildSymTabs(node->kids[i], scope);
                }
                fprintf(stderr, "buildSymTabs: Popping scope for function '%s'\n", node->kids[1]->leaf->text);
                
                
                if (scope != NULL)
                    fprintf(stderr, "New current scope: '%s'\n", scope->name);
                else
                    fprintf(stderr, "Current scope is now NULL (global level reached)\n");


                if (scope == NULL) scope = rootScope;

                break;

            //variable decalarations
            case varDec:
                addSymTab(scope, node->kids[0]->leaf->text, node->kids[2], VARIABLE);
                break;

            case varDecQuests:
                addSymTab(scope, node->kids[0]->leaf->text, node->kids[2], VARIABLE); //need nullable part of symTab
                break;

            default:
                for(int i = 0; i < node->nkids; i++)
                {
                    buildSymTabs(node->kids[i], scope);
                }
                break;
        }
    }
}


/**
 * @brief Checks if leave nodes are already declared
 * 
 * @param node 
 * @param scope 
 * @return int 
 */
int handleLeaves(struct tree *node, struct symTab *scope){
    if(node->leaf->category == IDENTIFIER)
    {
        bool declared = false;
        while(scope != NULL)
        {
            if(contains(scope, node->leaf->text) != NULL)
            {
                declared = true;
                return 0;
            }
            scope = scope->parent;
        
        }
        if(!declared)
        {


            printf("ERROR undeclared variable: %s\n", node->leaf->text);
            // exit(1);
        }
    }

    return 0;
}

