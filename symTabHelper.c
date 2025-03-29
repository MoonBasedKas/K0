#include "symTab.h"
#include "tree.h"
#include "lex.h"
#include "symNonTerminals.h"
#include "type.h"
#include "typeHelpers.h"
#include "k0gram.tab.h"
#include "symTabHelper.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern int symError;
extern struct tree *root;
extern struct symTab *rootScope;
extern struct symTab *currentScope;

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
            // Is it there?
            if(contains(rootScope, node->kids[0]->leaf->text) == NULL)
            scope = addSymTab(rootScope, node->kids[0]->leaf->text, NULL, PACKAGE);

            // So we need to check if the next leaf is a *...

            if (node->kids[2]->nkids != 0) {
                buildSymTabs(node->kids[2], scope);
            } else {
                if (strcmp(node->kids[2]->leaf->text, "*")) {
                    scope = addSymTab(rootScope, node->kids[2]->leaf->text, NULL, PACKAGE);
                } else {
                    if (contains(rootScope, "Math")) addMathModule();
                }
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
int addMathModule(){

    addSymTab(rootScope, "abs", NULL, FUNCTION);
    addSymTab(rootScope, "max", NULL, FUNCTION);
    addSymTab(rootScope, "min", NULL, FUNCTION);
    addSymTab(rootScope, "pow", NULL, FUNCTION);
    addSymTab(rootScope, "cos", NULL, FUNCTION);
    addSymTab(rootScope, "sin", NULL, FUNCTION);
    addSymTab(rootScope, "tan", NULL, FUNCTION);

    return 0;
}

/**
 * @brief Preloads the standard library.
 *
 */
void populateTypes(){
    // Types
    addSymTab(rootScope, "Int", NULL, VARIABLE);
    addSymTab(rootScope, "String", NULL, VARIABLE);
    addSymTab(rootScope, "Byte", NULL, VARIABLE);
    addSymTab(rootScope, "Short", NULL, VARIABLE);
    addSymTab(rootScope, "Long", NULL, VARIABLE);
    addSymTab(rootScope, "Float", NULL, VARIABLE);
    addSymTab(rootScope, "Boolean", NULL, VARIABLE);
    addSymTab(rootScope, "Double", NULL, VARIABLE);
    addSymTab(rootScope, "Array", NULL, VARIABLE);
}

/**
 * @brief Populates the standard library of what is there without includes.
 * 
 */
void populateStdlib(){
    // Functions
    addSymTab(rootScope, "print", NULL, VARIABLE);
    addSymTab(rootScope, "println", NULL, VARIABLE);
    addSymTab(rootScope, "get", NULL, VARIABLE);
    addSymTab(rootScope, "equals", NULL, VARIABLE);
    addSymTab(rootScope, "length", NULL, VARIABLE);
    addSymTab(rootScope, "toString", NULL, VARIABLE);
    addSymTab(rootScope, "valueOf", NULL, VARIABLE);
    addSymTab(rootScope, "substring", NULL, VARIABLE);
    addSymTab(rootScope, "readln", NULL, VARIABLE);
}


/**
 * @brief Pre adds all libraries in k0
 *
 */
void populateLibraries(){
    //Predefined libraries
    addSymTab(rootScope, "java", NULL, PACKAGE);
    addSymTab(rootScope, "util", NULL, PACKAGE);
    addSymTab(rootScope, "lang", NULL, PACKAGE);
    addSymTab(rootScope, "math", NULL, PACKAGE);
    addSymTab(rootScope, "Random", NULL, PACKAGE);

    // Functions within predfined libraries.
    addSymTab(rootScope, "nextInt", NULL, VARIABLE);
    addSymTab(rootScope, "abs", NULL, VARIABLE);
    addSymTab(rootScope, "max", NULL, VARIABLE);
    addSymTab(rootScope, "min", NULL, VARIABLE);
    addSymTab(rootScope, "pow", NULL, VARIABLE);
    addSymTab(rootScope, "cos", NULL, VARIABLE);
    addSymTab(rootScope, "sin", NULL, VARIABLE);
    addSymTab(rootScope, "tan", NULL, VARIABLE);
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
        default:
            return "UNKNOWN";
    }
}