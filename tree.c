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
int symError = 0;
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
    switch (node->prodrule)
    {
        //global
        case program:

            for (int i = 0; i < node->nkids; i++) {
                buildSymTabs(node->kids[i], scope);
            }

            break;

        // Import chains
        case expandingImportID:
            addSymTab(scope, node->kids[0]->leaf->text, NULL, VARIABLE);
            if (node->kids[2]->nkids != 0) {
                buildSymTabs(node->kids[2], scope);
            } else {
                if (strcmp(node->kids[2]->leaf->text, "*")) 
                    addSymTab(scope, node->kids[2]->leaf->text, NULL, VARIABLE);
            }
            
            // Check if next is valid.
            break;

        // Singular imports.
        case collapsedImport:
            if(node->kids[1]->nkids == 0){
                if (strcmp(node->kids[1]->leaf->text, "*")){ 
                    addSymTab(scope, node->kids[1]->leaf->text, NULL, VARIABLE);
                } else {
                    fprintf(stderr, "Line: %d | Cannot only import * need to specify a package.\n", node->kids[1]->leaf->lineno);
                    symError = 3;
                }
            } else {
                buildSymTabs(node->kids[1], scope);
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

/**
 * @brief Checks if everything in a tree has been declared.
 * 
 * @param node 
 * @param scope 
 * @return int 
 */
int verifyDeclared(struct tree *node, struct symTab *scope){
    if(node->nkids == 0){
        checkExistance(node, scope);
    } 
    switch(node->prodrule){
        // Entering a new scope.
        case funcDecAll:
        case funcDecParamType:
        case funcDecTypeBody:    
        case funcDecType:
        case funcDecParamBody:
        case funcDecBody:
            struct symTab *temp = scope;
            scope = contains(scope, node->kids[1]->leaf->text)->scope;
            for(int i = 2; i < node->nkids; i++)
            {
                verifyDeclared(node->kids[i], scope);
            }
            // Restore functions declared scope.
            scope = temp;
            // I don't think we'll need this.
            if (scope == NULL) scope = rootScope;
            break;

        default:
            for(int i = 0; i < node->nkids; i++) 
                verifyDeclared(node->kids[i], scope);
            break;
    }



    return 0;
}


/**
 * @brief Checks if something has been declared or not.
 * 
 * @param node 
 * @param scope 
 * @return int 
 */
int checkExistance(struct tree *node, struct symTab *scope){
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
            fprintf(stderr, "ERROR undeclared variable: %s\n", node->leaf->text);
            symError = 3;
        }
    }

    return 0;
}

