#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "type.h"
#include "symTab.h"
#include "typeHelpers.h"
#include "typeCheck.h"
#include "k0gram.tab.h"
#include "symNonTerminals.h"
#include "lex.h"
#include "semanticBuild.h"

struct tree *createEmptyParam(void);

static void checkLeafType(struct tree *n)
{
    // If it's not a leaf or `leaf` is NULL, do nothing
    if (n->nkids != 0 || !n->leaf) {
        return;
    }

    // Switch on the leaf's category -> This is how we printed leaf types to the syntax tree
    switch (n->prodrule) {
        case INT:
            n->type = alcType(INT_TYPE);
            break;
        case STRING:
            n->type = alcType(STRING_TYPE);
            break;
        case BOOL:
            n->type = alcType(BOOL_TYPE);
            break;
        case CHAR:
            n->type = alcType(CHAR_TYPE);
            break;
        case BYTE:
            n->type = alcType(BYTE_TYPE);
            break;
        case DOUBLE:
            n->type = alcType(DOUBLE_TYPE);
            break;
        case NULL_K:
            n->type = alcType(NULL_TYPE);
            break;
        default:
            return;
    }
}

// /**
//  * @brief Coerces an assignment to a type
//  *
//  * @param lhs
//  * @param rhs
//  * @return typePtr
//  */
// typePtr coerceAssignment(struct tree *n, typePtr lhs, typePtr rhs) {
//     if (lhs == NULL) {
//         typeError("Left hand side of assignment is NULL", n);
//         return NULL;
//     }
//     if (typeEquals(lhs, rhs))
//         return lhs;
//     if (lhs->basicType == INT_TYPE && rhs->basicType == DOUBLE_TYPE)
//         return rhs;
//     if (rhs->basicType == UNIT_TYPE)
//         return lhs;
//     return NULL;
// }

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

    checkLeafType(n);
    
    switch (n->prodrule){
        case varDecQuests: // Sets the entry to nullable.

            if (n->kids[1]->prodrule == arrayTypeQuests){
                changeNullable(n->table, n->kids[0]->leaf->text, squareNullable);
            } else {
                changeNullable(n->table, n->kids[0]->leaf->text, nullable);
            }
            goto zaWorldo; // This is probably a bad idea.
        case varDec:
            /*
            kids[0] = IDENTIFIER
            kids[1] = type
            */
        {
            if (n->kids[1]->prodrule == arrayTypeQuests){
                changeNullable(n->table, n->kids[0]->leaf->text, indexNullable);
            }
            zaWorldo:
            n->type = n->kids[1]->type;
            assignEntrytype(n->table, n->kids[0]->leaf->text, n->type); // very nice!
            break;
        }
        case funcDecAll:
        {
            /*
            FUN IDENTIFIER functionValueParameters COLON type functionBody
            kids[0] = FUN
            kids[1] = IDENTIFIER
            kids[2] = functionValueParameters
            kids[3] = type
            kids[4] = functionBody
            */
            rootScope = contains(rootScope, n->kids[1]->leaf->text)->scope;
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope); //type.c
            break;
        }
        case funcDecParamType:
        {
            /*
            FUN IDENTIFIER functionValueParameters COLON type
            kids[0] = FUN
            kids[1] = IDENTIFIER
            kids[2] = functionValueParameters
            kids[3] = type
            */
            rootScope = contains(rootScope, n->kids[1]->leaf->text)->scope;
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope); //type.c
            break;
        }
        case funcDecParamBody:
        {
            /*
            FUN IDENTIFIER functionValueParameters functionBody
            kids[0] = FUN
            kids[1] = IDENTIFIER
            kids[2] = functionValueParameters
            kids[3] = functionBody
            */
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope); //type.c
            break;
        }
        case funcDecTypeBody:
        {
            /*
            FUN IDENTIFIER LPAREN RPAREN COLON type functionBody
            kids[0] = FUN
            kids[1] = IDENTIFIER
            kids[2] = type
            kids[3] = functionBody
            */

            // Create an empty param node
            struct tree *emptyParam = createEmptyParam();
            n->type = alcFuncType(n->kids[2], emptyParam, rootScope); //type.c
            assignEntrytype(n->table, n->kids[1]->leaf->text, n->type);
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
        /*
        kids[0] = variable
        kids[1] = variableDeclaration
        kids[2] = arraySize
        kids[3] = arrayValues
        */
        {   
       
            if (!n->kids[1]->type) {
                typeCheck(n->kids[1]);
            }
            if (!n->kids[1]->type) {
                typeError("Array type must be specified", n);
                break;
            }
            n->type = alcArrayType(n->kids[2], n->kids[1]->type); //type.c
            assignEntrytype(n->table, n->kids[1]->leaf->text, n->type);
            break;
        }
        case arrayDecValueless:
        /*
        kids[0] = variable
        kids[1] = variableDeclaration
        kids[2] = arraySize
        */
        {
            if (!n->kids[1]->type) {
                typeCheck(n->kids[1]);
            }
            if (!n->kids[1]->type) {
                typeError("Array declaration missing element type", n);
                break;
            }
            n->type = alcArrayType(n->kids[2], n->kids[1]->type);
            assignEntrytype(n->table, n->kids[0]->leaf->text, n->type);
            break;
        }

        case arrayDecEqual:
        {
            /*
            kids[0] = variable
            kids[1] = variableDeclaration
            kids[2] = IDENTIFIER
            kids[3] = primitiveType
            kids[4] = arraySize
            kids[5] = arrayValues
            */
            printf("arrayDecEqual called\n");
            if (!n->kids[3]->type) {
                typeCheck(n->kids[3]);
            }
            if (!n->kids[3]->type) {
                typeError("Array declaration missing element type", n);
                break;
            }
            n->type = alcArrayType(n->kids[4], n->kids[3]->type);
            assignEntrytype(n->table, n->kids[0]->leaf->text, n->type);
            break;
        }

        case arrayDecEqualValueless:
            /*
            kids[0] = variable
            kids[1] = variableDeclaration
            kids[2] = IDENTIFIER
            kids[3] = primitiveType
            kids[4] = arraySize
            */
        {
            if (!n->kids[3]->type) {
                typeCheck(n->kids[3]);
            }
            if (!n->kids[3]->type) {
                typeError("Array declaration missing element type", n);
                break;
            }
            n->type = alcArrayType(n->kids[4], n->kids[3]->type);
            assignEntrytype(n->table, n->kids[0]->leaf->text, n->type);
            break;
        }
        case returnVal:
        {
            // If we have return expression
            if (n->nkids >= 2) {
                typeCheck(n->kids[1]);
                n->type = n->kids[1]->type ? n->kids[1]->type : alcType(UNIT_TYPE);
            } else {
                n->type = alcType(UNIT_TYPE);
            }
            break;
        }
        case arrayType:
        /*
        kids[0] = IDENTIFIER
        kids[1] = type
        */
        {
            n->type = n->kids[1]->type;
            break;
        }
        case arrayTypeQuests:
        /*
        kids[0] = IDENTIFIER
        kids[1] = type
        kids[2] = quests
        */
        {
            n->type = alcArrayType(n->kids[1], n->kids[2]->type);
            break;
        }
        default:
        {
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
        int val;
        // Add and Sub assignments shouldn't be capable of producing null.
        case assignment:
            if (root->kids[1]->nkids == 0 && root->kids[1]->leaf->category == NULL_K) {
                val = checkNullable(root->table, root->kids[0]->leaf->text);
                if(!(val == nullable || val == squareNullable)){ // Not nullable is BAD
                    fprintf(stderr, "Error | %s is not nullable but was assigned to null.\n", root->kids[0]->leaf->text);
                    symError = 1;
                }
            }
            // else if (root->kids[1]->type->basicType == NULL_K){
            //     val = checkNullable(root->table, root->kids[0]->leaf->text);
            //     if(!(val == nullable || val == squareNullable)){ // Not nullable is BAD
            //         fprintf(stderr, "Error | %s is not nullable but the expression computed null.\n", root->kids[0]->leaf->text);
            //         symError = 1;
            //     }
            // }
            break;
        // Arrays
        case propDecAssign:
            // Get Identifier node
            struct tree *temp = root->kids[1]->kids[0];
            if (root->kids[2]->nkids == 0 && root->kids[2]->leaf->category == NULL_K) {
                val = checkNullable(root->table, root->kids[0]->leaf->text);
                if( !(val == nullable || val == squareNullable)){ // Not nullable is BAD
                    fprintf(stderr, "Error | %s is not nullable but was assigned to null.\n", temp->leaf->text);
                    symError = 1;
                }
            }
            //  else if (root->kids[1]->type->basicType == NULL_K){
            //     val = checkNullable(root->table, root->kids[0]->leaf->text);
            //     if(!(val == nullable || val == squareNullable)){ // Not nullable is BAD
            //         fprintf(stderr, "Error | %s is not nullable but the expression computed null.\n", root->kids[0]->leaf->text);
            //         symError = 1;
            //     }
            // }
            break;
        case arrayAssignment:
        case arrayAssignAdd:
        case arrayAssignSub:
            if (root->kids[1]->nkids == 0 && root->kids[1]->leaf->category == NULL_K) {
                val = checkNullable(root->table, root->kids[0]->kids[0]->leaf->text);
                if( !(val == indexNullable || val == squareNullable)){ // Not nullable is BAD
                    fprintf(stderr, "Error | %s is not nullable but was assigned to null.\n", root->kids[0]->kids[0]->leaf->text);
                    symError = 1;
                }
            }
            // else if (root->kids[1]->type->basicType == NULL_K){
            //     val = checkNullable(root->table, root->kids[0]->kids[0]->leaf->text);
            //     if(!(val == indexNullable || val == squareNullable)){ // Not nullable is BAD
            //         fprintf(stderr, "Error | %s is not index nullable but the expression computed null.\n", root->kids[0]->kids[0]->leaf->text);
            //         symError = 1;
            //     }
            // }
        default:
            break;
    }
    return 0;
}


/**
 * @brief Checks if something is mutable. This should be safe and avoid touching
 * declarations.
 *
 * NOTE: Arrays are weird in that their inner values can be updated but not the
 * array itself.
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
            if(checkMutable(root->table, root->kids[0]->leaf->text) == 0){
                fprintf(stderr, "Error | %s is not mutable but was changed.\n", root->kids[0]->leaf->text);
                symError = 1;
            }
            break;
        // String elements cannot be modified; This isn't legal anyways lol.
        // But array elements for some reason can be changed.
        case arrayAssignAdd:
        case arrayAssignSub:
        case arrayAssignment:
            // struct symEntry *entry = contains(root->table, root->kids[0]->kids[0]->leaf->text);
            // if(entry->type->basicType == STRING_TYPE){
                // fprintf(stderr, "Error | %s is a string and is not mutable", root->kids[0]->leaf->text);
                // symError = 1;
            // }
        default:
            break;
    }
    return 0;
}