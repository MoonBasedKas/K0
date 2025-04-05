#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "type.h"
#include "symTab.h"
#include "typeHelpers.h"
#include "k0gram.tab.h"
#include "symNonTerminals.h"
#include "lex.h"
#include "semanticBuild.h"

struct tree *createEmptyParam(void);

/**
 * @brief Assigns a type to a node
 *
 * @param node
 */
void assignType(struct tree *n, struct symTab *rootScope){ // Many composite types to handle
    if (n == NULL) return;

    for (int i = 0; i < n->nkids; i++){
        assignType(n->kids[i], rootScope);
    }

    switch (n->prodrule){
        case INT:
            n->type = alcType(INT_TYPE); //type.c
            break;
        case STRING:
            n->type = alcType(STRING_TYPE); //type.c
            break;
        case BOOL:
            n->type = alcType(BOOL_TYPE); //type.c
            break;
        case CHAR:
            n->type = alcType(CHAR_TYPE); //type.c
            break;
        case BYTE:
            n->type = alcType(BYTE_TYPE); //type.c
            break;
        case DOUBLE:
            n->type = alcType(DOUBLE_TYPE); //type.c
            break;
        case NULL_K:
            n->type = alcType(NULL_TYPE); //type.c
            break;
        case varDecQuests: // Sets the entry to nullable.
            makeEntryNullable(n->table, n->kids[0]->leaf->text);
        case varDec:
            n->type = n->kids[1]->type;
            assignEntrytype(n->table, n->kids[0]->leaf->text, n->type); // very nice!
            
            break;
        case assignAdd:
        case assignSub:
        case arrayAssignment:
        case arrayAssignAdd:
        case arrayAssignSub:
        case assignment:
        {
            typePtr lhsType = lookupType(n->kids[0]); //typeHelpers.c
            typePtr rhsType = n->kids[1]->type;
            if(!typeEquals(lhsType, rhsType)){ //typeHelpers.c
                fprintf(stderr, "Type error: %s and %s are not compatible\n",
                typeName(lhsType), typeName(rhsType)); //typeHelpers.c
                symError = 3;
                // return 3;
                exit(3);
            }
            n->type = alcType(lhsType->basicType); // TODO: Check if this is correct - type.c
            break;
        }
        case funcDecAll:
        {
            /*
            FUN IDENTIFIER functionValueParameters COLON type functionBody
            */
            typePtr declaredReturnType = n->kids[3]->type;
            typePtr bodyType = n->kids[4]->type;
            if(!typeEquals(declaredReturnType, bodyType)){ //typeHelpers.c
                fprintf(stderr, "Type error in function %s: body type %s does not match the return type %s.\n",
                n->kids[1]->leaf->text, typeName(bodyType),
                typeName(declaredReturnType)); //typeHelpers.c
                symError = 3;
                // return 3;
                exit(3);
            }
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope); //type.c
            break;
        }
        case funcDecParamType:
        {
            /*
            FUN IDENTIFIER functionValueParameters COLON type
            */
            rootScope = contains(rootScope, n->kids[1]->leaf->text)->scope;
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope); //type.c
            break;
        }
        case funcDecParamBody:
        {
            /*
            FUN IDENTIFIER functionValueParameters functionBody
            */
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope); //type.c
            break;
        }
        case funcDecTypeBody:
        {
            /*
            FUN IDENTIFIER LPAREN RPAREN COLON type functionBody
            */
            typePtr declaredReturnType = n->kids[2]->type;
            typePtr bodyType = n->kids[3]->type;
            if(!typeEquals(declaredReturnType, bodyType)){ //typeHelpers.c
                fprintf(stderr, "Type error in function %s: body type %s does not match the return type %s.\n",
                n->kids[1]->leaf->text, typeName(bodyType),
                typeName(declaredReturnType)); //typeHelpers.c
                symError = 3;
                // return 3;
                exit(3);
            }
            // Create an empty param node
            struct tree *emptyParam = createEmptyParam();
            n->type = alcFuncType(n->kids[2], emptyParam, rootScope); //type.c
            break;
        }
        case funcDecType:
        {
            /*
            FUN IDENTIFIER LPAREN RPAREN COLON type
            */
            struct tree *emptyParam = createEmptyParam();
            n->type = alcFuncType(n->kids[2], emptyParam, rootScope); //type.c
            break;
        }
        case funcDecBody:
        {
            /*
            FUN IDENTIFIER functionValueParameters functionBody
            */
            struct tree *emptyParam = createEmptyParam();
            n->type = alcFuncType(n->kids[2], emptyParam, rootScope); //type.c
            break;
        }
        case arrayDec:
        {
            n->type = alcArrayType(n->kids[2], n->kids[1]->type); //type.c
            break;
        }
        case arrayDecValueless:
        {
            n->type = alcArrayType(n->kids[2], n->kids[1]->type); //type.c
            break;
        }
        case arrayDecEqual:
        {
            n->type = alcArrayType(n->kids[7], n->kids[1]->type); //type.c
            break;
        }
        case arrayDecEqualValueless:
        {
            n->type = alcArrayType(n->kids[7], n->kids[1]->type); //type.c
            break;
        }
        default:
        {
            
            if(n->nkids > 0){
                n->type = n->kids[0]->type;
            } else {
                n->type = alcType(ANY_TYPE); //type.c
            }
            break;
        }
    }
}

/**
 * @brief Creates an empty parameter node
 *
 * @return struct tree*
 */
struct tree *createEmptyParam(void) {
    struct tree *emptyParam = malloc(sizeof(struct tree));
    if (emptyParam == NULL) {
        fprintf(stderr, "Failed to allocate memory for empty parameter node.\n");
        exit(EXIT_FAILURE);
    }
    emptyParam->nkids = 0;
    emptyParam->type = NULL;
    return emptyParam;
}


/**
 * @brief Checks if something not nullable is set to null
 * 
 * TODO: make this cover every sub case.
 * 
 * @param root 
 * @return int 
 */
int checkNullability(struct tree *root){
    for(int i = 0; i < root->nkids; i++){
        checkNullability(root->kids[i]);
    }
    switch(root->prodrule){
        case assignment:
            if (root->kids[1]->nkids == 0 && root->kids[1]->leaf->category == NULL_K) {
                if(!checkNullable(root->table, root->kids[0]->leaf->text)){ // Not nullable is BAD
                    fprintf(stderr, "Error | %s is not nullable but was assigned to null.\n", root->kids[0]->leaf->text);
                    symError = 1;
                }
            }
            break;
        // Arrays
        case propDecAssign:
            // Get Identifier node
            struct tree *temp = root->kids[1]->kids[0];
            if (root->kids[2]->nkids == 0 && root->kids[2]->leaf->category == NULL_K) {
                if(!checkNullable(root->table, temp->leaf->text)){ // Not nullable is BAD
                    fprintf(stderr, "Error | %s is not nullable but was assigned to null.\n", temp->leaf->text);
                    symError = 1;
                }
            }
            break;
        default:
            break;
    }
    return 0;
}


/**
 * @brief Checks if something is mutable.
 * 
 * @param root 
 * @return int 
 */
int checkMutability(struct tree *root){
    for(int i = 0; i < root->nkids; i++){
        checkMutability(root->kids[i]);
    }
    switch(root->prodrule){
        case assignAdd:
        case assignSub:
        case assignment:
            if(!checkMutable(root->table, root->kids[0]->leaf->text)){ 
                fprintf(stderr, "Error | %s is not mutable but was changed.\n", root->kids[0]->leaf->text);
                symError = 1;
            }
            break;
        // String elements cannot be modified; This isn't legal anyways lol.
        // But array elements for some reason can be changed.
        case arrayAssignAdd:
        case arrayAssignSub:
        case arrayAssignment:
            struct symEntry *entry = contains(root->table, root->kids[0]->leaf->text);
            if(entry->type->basicType == STRING_TYPE){
                fprintf(stderr, "Error | %s is a string and is not mutable", root->kids[0]->leaf->text);
                symError = 1;
            }
        default:
            break;
    }
    return 0;    
}