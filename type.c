#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "symTab.h"
#include "lex.h"
#include "k0gram.tab.h"
#include "semanticBuild.h"
#include "typeHelpers.h"

struct typeInfo null_type = {NULL_TYPE};
struct typeInfo byte_type = {BYTE_TYPE};
struct typeInfo integer_type = {INT_TYPE};
struct typeInfo double_type = {DOUBLE_TYPE};
struct typeInfo boolean_type = {BOOL_TYPE};
struct typeInfo char_type = {CHAR_TYPE};
struct typeInfo string_type = {STRING_TYPE};

extern struct symTab *rootScope;

typePtr nullType_ptr = &null_type;
typePtr byteType_ptr = &byte_type;
typePtr integerType_ptr = &integer_type;
typePtr shortType_ptr = &integer_type;
typePtr longType_ptr = &integer_type;
typePtr floatType_ptr = &double_type;
typePtr doubleType_ptr = &double_type;
typePtr booleanType_ptr = &boolean_type;
typePtr charType_ptr = &char_type;
typePtr stringType_ptr = &string_type;

typePtr alcType(int baseType) {

    typePtr rv;

    switch (baseType) {
        case NULL_TYPE:
            return nullType_ptr;
        case BYTE_TYPE:
            return byteType_ptr;
        case INT_TYPE:
        case SHORT_TYPE:
        case LONG_TYPE:
            return integerType_ptr;
        case FLOAT_TYPE:
        case DOUBLE_TYPE:
            return doubleType_ptr;
        case BOOL_TYPE:
            return booleanType_ptr;
        case CHAR_TYPE:
            return charType_ptr;
        case STRING_TYPE:
            return stringType_ptr;
        default:
            rv = (typePtr)calloc(1, sizeof(struct typeInfo));
            if (rv == NULL) {
                fprintf(stderr, "Out of memory in alcType\n");
                exit(3);
            }
            rv->basicType = baseType;
            return rv;
    }
}

/**
 * @brief Allocates a function type (composite type)
 *
 * @param r - return type
 * @param p - parameter list
 * @param st - symbol table
 * @return rv
 */

typePtr alcFuncType(struct tree *r, struct tree *p, struct symTab *st) {
    typePtr rv = alcType(FUNCTION_TYPE);
    if (rv == NULL)
        return NULL;

    // Symbol table for the scope
    // This might be the most difficult thing I could need to obtain.
    rv->u.func.st = st;

    // Get the return type
    rv->u.func.returnType = determineReturnType(r); //typeHelpers.c

    // Traverse/process subtree(s) for parameters
    rv->u.func.numParams = p->nkids;
    rv->u.func.parameters = NULL;
    struct param *lastParam = NULL;

        for (int i = 0; i < p->nkids; i++) {
        struct tree *paramNode = p->kids[i];
        // Use the new helper to create a new parameter node.
        struct param *newParam = createParamFromTree(paramNode); //typeHelpers.c

        // Add to the end of the list.
        if (rv->u.func.parameters == NULL) {
            rv->u.func.parameters = newParam;
        } else {
            lastParam->next = newParam;
        }
        lastParam = newParam;
    }

    return rv;
}

/**
 * @brief Allocates an array type (composite type)
 *
 * @param size - size of the array
 * @param elemType - element type of the array
 * @return rv
 */
typePtr alcArrayType(struct tree *size, struct typeInfo *elemType) {
    typePtr rv = alcType(ARRAY_TYPE);
    if (rv == NULL)
        return NULL;

    // Type of array
    rv->u.array.elemType = elemType;

    // Size of array
    rv->u.array.size = extractArraySize(size); //typeHelpers.c
    return rv;
}

/**
 * @brief Determines what the type is
 * 
 * @param node 
 * @return int 
 */
// int findType(struct tree *node){
//     if(node->nkids != 0) return ANY_TYPE;

//     if(node->leaf->category != IDENTIFIER) return ANY_TYPE;

//     if(!strcmp(node->leaf->text, "Int")) return INT_TYPE;
//     if(!strcmp(node->leaf->text, "Float")) return FLOAT_TYPE;
//     if(!strcmp(node->leaf->text, "String")) return STRING_TYPE;
//     if(!strcmp(node->leaf->text, "Boolean")) return BOOL_TYPE;
//     if(!strcmp(node->leaf->text, "Char")) return CHAR_TYPE;
//     if(!strcmp(node->leaf->text, "Byte")) return BYTE_TYPE;
//     if(!strcmp(node->leaf->text, "Short")) return SHORT_TYPE;
//     if(!strcmp(node->leaf->text, "Long")) return LONG_TYPE;
//     if(!strcmp(node->leaf->text, "Double")) return DOUBLE_TYPE;
//     if(!strcmp(node->leaf->text, "Null")) return NULL_TYPE;
//     if(!strcmp(node->leaf->text, "Any")) return ANY_TYPE; // This shouldn't really happen

//     return ANY_TYPE;
// }
