#include "symTab.h"
#include "tree.h"
#include "lex.h"
#include "symNonTerminals.h"
#include "type.h"
#include "typeHelpers.h"
#include "k0gram.tab.h"
#include "symTabHelper.h"
#include "errorHandling.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern int symError;
extern struct tree *root;
extern struct symTab *rootScope;
extern struct symTab *currentScope;

const char* supportedImports[] = {
    "String",
    "get",
    "equals",
    "length",
    "toString",
    "valueOf",
    "substring",
    "java",
    "util",
    "lang",
    "Math",
    "Random",
    "nextInt",
    "abs",
    "max",
    "min",
    "pow",
    "cos",
    "sin",
    "tan",
    NULL  // Sentinel value
};

/**
 * @brief Runs through the tree interesting into the table.
 *
 * @param node
 */
void buildSymTabs(struct tree *node, struct symTab *scope)
{
    node->table = scope;
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
            // Is it there?
            bool isValidImport = false;
            for (int i = 0; supportedImports[i] != NULL; i++) {
                if (strcmp(node->kids[0]->leaf->text, supportedImports[i]) == 0) {
                    isValidImport = true;
                    break;
                }
            }
            if (!isValidImport) {
                typeError("Invalid import", node);
                return;
            }
            if(contains(rootScope, node->kids[0]->leaf->text) == NULL) //symTab.c
            addSymTab(rootScope, node->kids[0]->leaf->text, VARIABLE); //symTab.c

            // So we need to check if the next leaf is a *...

            if (node->kids[2]->nkids != 0) {
                buildSymTabs(node->kids[2], scope);
            } else {
                if (strcmp(node->kids[2]->leaf->text, "*")) {
                    addSymTab(rootScope, node->kids[2]->leaf->text, VARIABLE);
                }
            }
            break;

        // Singular imports.
        case collapsedImport:
            if(node->kids[1]->nkids == 0){
                if (strcmp(node->kids[1]->leaf->text, "*")){
                    addSymTab(scope, node->kids[1]->leaf->text, VARIABLE); //symTab.c
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
            scope = addSymTab(scope, node->kids[1]->leaf->text, FUNCTION); //symTab.c
            for(int i = 2; i < node->nkids; i++)
            {
                buildSymTabs(node->kids[i], scope);
            }
            if (scope->parent != NULL) scope = scope->parent;
            break;

        case funcDecTypeBody:
        case funcDecType:
            scope = addSymTab(scope, node->kids[1]->leaf->text, FUNCTION); //symTab.c
            for(int i = 2; i < node->nkids; i++)
            {
                buildSymTabs(node->kids[i], scope);
            }

            if (scope->parent != NULL) scope = scope->parent;
            break;

        case funcDecParamBody:
        case funcDecBody:

            scope = addSymTab(scope, node->kids[1]->leaf->text, FUNCTION); //symTab.c
            for(int i = 2; i < node->nkids; i++)
            {
                buildSymTabs(node->kids[i], scope);
            }


            if (scope == NULL) scope = rootScope;

            break;

        //variable decalarations
        case varDec:
            addSymTab(scope, node->kids[0]->leaf->text, VARIABLE); //symTab.c
            break;

        case varDecQuests:
            addSymTab(scope, node->kids[0]->leaf->text, VARIABLE); //need nullable part of symTab
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
            if(contains(scope, node->leaf->text) != NULL) //symTab.c
            {
                declared = true;
                return declared;
            }
            scope = scope->parent;

        }
        if(!declared)
        {
            fprintf(stderr, "ERROR undeclared variable: %s at line %d\n", node->leaf->text, node->leaf->lineno);
            symError = 3;
        }
    }

    return 0;
}

/**
 * @brief If the math module is called add its functions to global.
 * This is done because I don't want scope looking to have a time
 * complexity of O(n).
 *
 * @return int
 */
// int addMathModule(){
//     struct symEntry *temp = NULL;
//     addSymTab(rootScope, "abs", VARIABLE); //symTab.c
//     temp = contains(rootScope, "abs");
//     temp->type = alcType(FUNCTION_TYPE);
//     addSymTab(rootScope, "max", VARIABLE);
//     temp = contains(rootScope, "max");
//     temp->type = alcType(FUNCTION_TYPE);
//     addSymTab(rootScope, "min", VARIABLE);
//     temp = contains(rootScope, "min");
//     temp->type = alcType(FUNCTION_TYPE);
//     addSymTab(rootScope, "pow", VARIABLE);
//     temp = contains(rootScope, "pow");
//     temp->type = alcType(FUNCTION_TYPE);
//     addSymTab(rootScope, "cos", VARIABLE);
//     temp = contains(rootScope, "cos");
//     temp->type = alcType(FUNCTION_TYPE);
//     addSymTab(rootScope, "sin", VARIABLE);
//     temp = contains(rootScope, "sin");
//     temp->type = alcType(FUNCTION_TYPE);
//     addSymTab(rootScope, "tan", VARIABLE);
//     temp = contains(rootScope, "tan");
//     temp->type = alcType(FUNCTION_TYPE);

//     return 0;
// }

/**
 * @brief Removed everything except for Array.
 *
 */
void populateTypes(){
    addSymTab(rootScope, "Array", VARIABLE);
}

/**
 * @brief Populates the standard library of what is there without includes.
 * 
 */
void populateStdlib(){
    struct symEntry *temp = NULL;
    struct param *params = NULL;
    // Functions
    addSymTab(rootScope, "print", VARIABLE); //symTab.c
    temp = contains(rootScope, "print");
    params = buildfuncParams(1, "line", STRING_TYPE);
    temp->type = evilAlcfunctype(UNIT_TYPE, "print", params, 1, rootScope);

    addSymTab(rootScope, "println", VARIABLE);
    temp = contains(rootScope, "println");
    params = buildfuncParams(1, "line", STRING_TYPE);
    temp->type = evilAlcfunctype(UNIT_TYPE, "println", params, 1, rootScope);

    addSymTab(rootScope, "get", VARIABLE);
    temp = contains(rootScope, "get");
    params = buildfuncParams(1, "index", INT_TYPE);
    temp->type = evilAlcfunctype(CHAR_TYPE, "get", params, 1, rootScope);

    addSymTab(rootScope, "equals", VARIABLE);
    temp = contains(rootScope, "equals");
    params = buildfuncParams(1, "strOne", STRING_TYPE);
    temp->type = evilAlcfunctype(BOOL_TYPE, "equals", params, 1, rootScope);

    addSymTab(rootScope, "length", VARIABLE);
    temp = contains(rootScope, "length");
    temp->type = evilAlcfunctype(INT_TYPE, "length", NULL, 0, rootScope);

    addSymTab(rootScope, "toString", VARIABLE);
    temp = contains(rootScope, "toString");
    params = buildfuncParams(1, "strOne", INT_TYPE); // TODO: Update this
    temp->type = evilAlcfunctype(BOOL_TYPE, "toString", params, 1, rootScope);

    addSymTab(rootScope, "valueOf", VARIABLE);
    temp = contains(rootScope, "valueOf"); // TODO: update this
    temp->type = evilAlcfunctype(INT_TYPE, "valueOf", NULL, 0, rootScope);

    addSymTab(rootScope, "substring", VARIABLE);
    temp = contains(rootScope, "substring");
    params = buildfuncParams(2, "iOne", INT_TYPE, "iTwo", INT_TYPE);
    temp->type = evilAlcfunctype(STRING_TYPE, "substring", params, 2, rootScope);

    addSymTab(rootScope, "readln", VARIABLE);
    temp = contains(rootScope, "readln");
    temp->type = evilAlcfunctype(STRING_TYPE, "readln", NULL, 0, rootScope);
}

/**
 * @brief Gets the string of a given table type.
 *
 * @param type
 * @return char*
 */
char *getTableType(int type){
    switch(type){
        case FUNCTION:
            return "Function";
        case PACKAGE:
            return "Package";
        case IMPORT:
            return "Import";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief The savior of null table issues. Fixes an issue where some nodes wouldn't
 * get a symbol table assigned. 
 * 
 * @param root 
 * @return int 
 */
int giveTables(struct tree *root){
    if (root->table == NULL){
        root->table = root->parent->table;

    }
    for(int i = 0; i < root->nkids; i++) {
        giveTables(root->kids[i]);
    }
    return 0;
}