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

static void checkLeafType(struct tree *n)
{
    // If it's not a leaf or `leaf` is NULL, do nothing
    if (n->nkids != 0 || !n->leaf) {
        return;
    }

    // Switch on the leaf's category -> This is how we printed leaf types to the syntax tree
    switch (n->leaf->category) {
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
        case VOID:
            n->type = alcType(UNIT_TYPE);
            break;
        /* inc sus code */
        // case IDENTIFIER:
        //     printf("Identifier: %s\n",
        //             n->leaf->text ? n->leaf->text : "null");
        //     printf("Table: %s\n", n->table->name);
        //     struct symTab *scope = n->table;
        //     struct symEntry *entry = NULL;
        //     while (scope && !entry) {
        //         printf("Current scope: %s\n", scope->name);
        //         entry = contains(scope, n->leaf->text);
        //         scope = scope->parent;
        //     }
        //     if (entry) {
        //         n->type = entry->type;
        //         printf("Type of %s: %s\n", n->leaf->text, typeName(n->type));
        //     } else {
        //         fprintf(stderr, "Error | %s is not declared.\n", n->leaf->text);
        //         symError = 1;
        //     }
        //     break;
        default:
            return;
    }
}

/**
 * @brief Coerces an assignment to a type
 *
 * @param lhs
 * @param rhs
 * @return typePtr
 */
typePtr coerceAssignment(typePtr lhs, typePtr rhs) {
    if (typeEquals(lhs, rhs))
        return lhs;
    if (lhs->basicType == INT_TYPE && rhs->basicType == DOUBLE_TYPE)
        return rhs;
    if (rhs->basicType == UNIT_TYPE)
        return lhs;
    return NULL;
}

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
            if (n->kids[1]->prodrule == arrayTypeQuests){
                changeNullable(n->table, n->kids[0]->leaf->text, indexNullable);
            }
            zaWorldo:
            n->type = n->kids[1]->type;
            printf("varDec: %s: %s -> line %d\n", n->kids[0]->leaf->text, typeName(n->type), n->kids[0]->leaf->lineno);
            assignEntrytype(n->table, n->kids[0]->leaf->text, n->type); // very nice!
            break;

        case assignAdd:
        case assignSub:
        case arrayAssignment:
        case arrayAssignAdd:
        case arrayAssignSub:
        case assignment:
        /*
        kids[0] = IDENTIFIER
        kids[1] = expression
        */
        {
            typePtr lhsType = lookupType(n->kids[0]); //typeHelpers.c
            if (n->kids[1] == NULL) {
                printf("%d\n", n->kids[1]->prodrule);
            }
            typeCheckExpression(n->kids[1]);
            typePtr rhsType = n->kids[1]->type;
            printf("lhsType: %s: %s -> line %d\n", n->kids[0]->leaf->text, typeName(lhsType), n->kids[0]->leaf->lineno);
            typePtr coercedType = coerceAssignment(lhsType, rhsType);
            if(!coercedType){ //typeHelpers.c
                fprintf(stderr, "Type error: %s and %s are not compatible at line %d\n",
                typeName(lhsType), typeName(rhsType), n->kids[0]->leaf->lineno); //typeHelpers.c
                symError = 3;
                return ;
                // exit(3);
            }
            n->type = coercedType; // TODO: Check if this is correct - type.c
            if (!typeEquals(lhsType, coercedType)) {
                assignEntrytype(n->table, n->kids[0]->leaf->text, coercedType);
            }
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
            typePtr declaredReturnType = n->kids[3]->type;
            typePtr bodyType = n->kids[4]->type;
            if(!typeEquals(declaredReturnType, bodyType)){ //typeHelpers.c
                fprintf(stderr, "(funcDecAll) Type error in function %s: body type %s does not match the return type %s.\n",
                n->kids[1]->leaf->text, typeName(bodyType),
                typeName(declaredReturnType)); //typeHelpers.c
                symError = 3;
                return ;
                // exit(3);
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
            kids[0] = FUN
            kids[1] = IDENTIFIER
            kids[2] = type
            kids[3] = functionBody
            */
            typePtr declaredReturnType = n->kids[2]->type;
            typePtr bodyType = n->kids[3]->type;

            if(!typeEquals(declaredReturnType, bodyType)){ //typeHelpers.c
                if (bodyType->basicType == UNIT_TYPE) {
                    bodyType = declaredReturnType;
                } else {
                    fprintf(stderr, "(funcDecTypeBody) Type error in function %s: body type %s does not match the return type %s.\n",
                    n->kids[1]->leaf->text, typeName(bodyType),
                    typeName(declaredReturnType)); //typeHelpers.c
                    symError = 3;
                }
            }
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
        case range:
        case rangeUntil:
        {
            typePtr leftType  = n->kids[0]->type;
            typePtr rightType = n->kids[1]->type;

            if (leftType->basicType == INT_TYPE && rightType->basicType == INT_TYPE) {
                // n->type = rangeType_ptr;
                n->type = alcType(RANGE_TYPE);
            } else {
                typeError("range or rangeUntil requires two Int expressions", n);
            }
            break;
        }
        case funcBody:
        {
            // kids[0] = '=' token, kids[1] = expression
            if (n->nkids >= 2 && n->kids[1]->type) {
                n->type = n->kids[1]->type;
            } else {
                n->type = alcType(UNIT_TYPE);
            }
            break;
        }
        case blockEmpty:
        {
            // No statements => Unit
            n->type = alcType(UNIT_TYPE);
            break;
        }
        case blockStmnts:
        {
            // { statements }
            // type if it exists, else default to Unit
            typePtr stmtsType = n->kids[1]->type;
            if (!stmtsType) {
                stmtsType = alcType(UNIT_TYPE);
            }
            n->type = stmtsType;
            break;
        }
        case statementsMultiple:
        {
            /*
            statements -> statement SEMICOLON statements
            kids[0] is the first statement, kids[1] is the rest
            adopt the type of the last child for expression blocks?
            or we always do Unit?
            */
            typePtr tailType = n->kids[1]->type;
            if (tailType) {
                n->type = tailType;
            } else if (n->kids[0]->type) {
                n->type = n->kids[0]->type;
            } else {
                n->type = alcType(UNIT_TYPE);
            }
            break;
        }
        case statement:
        {
            if (n->nkids > 0 && n->kids[0]->type != NULL) {
                n->type = n->kids[0]->type;
            } else {
                // loops and declarations are allowed to be Unit
                n->type = alcType(UNIT_TYPE);
            }
            break;
        }
        case returnVal:
        {
            // If we have return expression
            if (n->nkids >= 2) {
                typeCheckExpression(n->kids[1]);
                n->type = n->kids[1]->type ? n->kids[1]->type : alcType(UNIT_TYPE);
            } else {
                n->type = alcType(UNIT_TYPE);
            }
            break;
        }
        /*
        Need IFelse stuff
        If can be a statement or an expression => check children?
        Statement = Unit
        Expression = Type of the expression
        */
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