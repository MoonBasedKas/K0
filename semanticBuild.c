/*
This file will handle our type checking and type assignment.
*/

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


/**
 * @brief Assigns a type to a node
 *
 * @param node
 */
void assignType(struct tree *n){ // Many composite types to handle
    if (n == NULL) return;

    for (int i = 0; i < n->nkids; i++){
        assignType(n->kids[i]);
    }

    switch (n->prodrule){
        case INT:
            n->type = alcType(INT_TYPE);
            break;
        case FLOAT:
            n->type = alcType(FLOAT_TYPE);
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
        case SHORT:
            n->type = alcType(SHORT_TYPE);
            break;
        case LONG:
            n->type = alcType(LONG_TYPE);
            break;
        case DOUBLE:
            n->type = alcType(DOUBLE_TYPE);
            break;
        case NULL_K:
            n->type = alcType(NULL_TYPE);
            break;
        case varDec:
            n->type = n->kids[1]->type;
            break;
        case assignAdd:
        case assignSub:
        case arrayAssignment:
        case arrayAssignAdd:
        case arrayAssignSub:
        case assignment:
            typePtr lhsType = lookupType(n->kids[0]);
            typePtr rhsType = n->kids[1]->type;
            if(!compatible(lhsType, rhsType)){
                fprintf(stderr, "Type error: %s and %s are not compatible\n",
                typeName(lhsType), typeName(rhsType));
                exit(3);
            }
            n->type = alcType(lhsType->basicType); // TODO: Check if this is correct
            break;
        case funcDecAll:
            /*
            FUN IDENTIFIER functionValueParameters COLON type functionBody
            */
            typePtr declaredReturnType = n->kids[3]->type;
            typePtr bodyType = n->kids[4]->type;
            if(!compatible(declaredReturnType, bodyType)){
                fprintf(stderr, "Type error in function %s: body type %s does not match the return type %s.\n",
                n->kids[1]->leaf->text, typeName(bodyType),
                typeName(declaredReturnType));
                exit(3);
            }
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope);
            break;
        case funcDecParamType:
            /*
            FUN IDENTIFIER functionValueParameters COLON type
            */
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope);
            break;
        case funcDecParamBody:
            /*
            FUN IDENTIFIER functionValueParameters functionBody
            */
            n->type = alcFuncType(n->kids[3], n->kids[2], rootScope);
            break;
        case funcDecTypeBody:
            /*
            FUN IDENTIFIER LPAREN RPAREN COLON type functionBody
            */
            typePtr declaredReturnType = n->kids[2]->type;
            typePtr bodyType = n->kids[3]->type;
            if(!compatible(declaredReturnType, bodyType)){
                fprintf(stderr, "Type error in function %s: body type %s does not match the return type %s.\n",
                n->kids[1]->leaf->text, typeName(bodyType),
                typeName(declaredReturnType));
                exit(3);
            }
            // Create an empty param node
            struct tree *emptyParam = malloc(sizeof(struct tree));
            if(emptyParam == NULL){
                fprintf(stderr, "Failed to allocate memory for empty param node.\n");
                exit(EXIT_FAILURE);
            }
            emptyParam->nkids = 0;
            emptyParam->type = NULL;
            n->type = alcFuncType(n->kids[2], emptyParam, rootScope);
            free(emptyParam);
            break;
        case funcDecType:
            /*
            FUN IDENTIFIER LPAREN RPAREN COLON type
            */
            struct tree *emptyParam = malloc(sizeof(struct tree));
            if(emptyParam == NULL){
                fprintf(stderr, "Failed to allocate memory for empty param node.\n");
                exit(EXIT_FAILURE);
            }
            emptyParam->nkids = 0;
            emptyParam->type = NULL;
            n->type = alcFuncType(n->kids[2], emptyParam, rootScope);
            break;
        case funcDecBody:
            /*
            FUN IDENTIFIER functionValueParameters functionBody
            */
            struct tree *emptyParam = malloc(sizeof(struct tree));
            if(emptyParam == NULL){
                fprintf(stderr, "Failed to allocate memory for empty param node.\n");
                exit(EXIT_FAILURE);
            }
            emptyParam->nkids = 0;
            emptyParam->type = NULL;
            n->type = alcFuncType(n->kids[2], emptyParam, rootScope);
            free(emptyParam);
            break;
        default:
            if(n->nkids > 0){
                n->type = n->kids[0]->type;
            } else {
                n->type = alcType(ANY_TYPE);
            }
            break;
    }
}

// Free empty param node
