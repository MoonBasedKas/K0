#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "symTab.h"
#include "lex.h"
#include "k0gram.tab.h"

struct typeInfo null_type = {NULL_TYPE};
struct typeInfo byte_type = {BYTE_TYPE};
struct typeInfo integer_type = {INT_TYPE};
struct typeInfo double_type = {DOUBLE_TYPE};
struct typeInfo boolean_type = {BOOL_TYPE};
struct typeInfo char_type = {CHAR_TYPE};
struct typeInfo string_type = {STRING_TYPE};

/* Aliases?
Shorts and Longs are aliases for Integers
Floats are aliases for Doubles

Short is supposed to be an alias for Int (per Kotlin docs), but k0 spec says different.

*/
typePtr null_typePtr = &null_type;
typePtr byte_typePtr = &byte_type;
typePtr integer_typePtr = &integer_type;
typePtr short_typePtr = &integer_type;
typePtr long_typePtr = &integer_type;
typePtr float_typePtr = &double_type;
typePtr double_typePtr = &double_type;
typePtr boolean_typePtr = &boolean_type;
typePtr char_typePtr = &char_type;
typePtr string_typePtr = &string_type;

char *typeName[] = {"null",
                    "byte",
                    "int",
                    "short",
                    "long",
                    "float",
                    "double",
                    "boolean",
                    "char",
                    "string",
                    "function",
                    "array",
                    "any"};

typePtr alcType(int baseType) {

    typePtr rv;

    switch (baseType) {
        case NULL_TYPE:
            return null_typePtr;
        case BYTE_TYPE:
            return byte_typePtr;
        case INT_TYPE:
        case SHORT_TYPE:
        case LONG_TYPE:
            return integer_typePtr;
        case FLOAT_TYPE:
        case DOUBLE_TYPE:
            return double_typePtr;
        case BOOL_TYPE:
            return boolean_typePtr;
        case CHAR_TYPE:
            return char_typePtr;
        case STRING_TYPE:
            return string_typePtr;
        default:
            rv = (typePtr)calloc(1, sizeof(struct typeInfo));
            if (rv == NULL) {
                fprintf(stderr, "Out of memory in alcType\n");
                exit(1);
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
    rv->u.func.st = st;

    // Get the return type
    if (r != NULL) {
        if (r->type != NULL)
            rv->u.func.returnType = r->type;
        else
            rv->u.func.returnType = alcType(r->id);
    } else {
        // If no return type is provided, use a null_type (this was used interchangeably with NONE_TYPE in examples)
        rv->u.func.returnType = null_typePtr;
    }
    // Traverse/process subtree(s) for parameters
    rv->u.func.numParams = p->nkids;
    rv->u.func.parameters = NULL;
    struct param *lastParam = NULL;

    for (int i = 0; i < p->nkids; i++) {
        struct tree *paramNode = p->kids[i];
        typePtr paramType = NULL;
        char *paramName = NULL;

    /*
    Hide yo kids, hide yo wife
    We need synthesize types from the children of the paramNode
    */
    if (paramNode->nkids >= 1) {
        if (paramNode->kids[0]->type != NULL)
            paramType = paramNode->kids[0]->type;
        else
            paramType = alcType(paramNode->kids[0]->id);

    } else {
        paramType = null_typePtr;
    }

    // Get the name of the parameter
    if (paramNode->nkids >= 2) {
        paramName = paramNode->kids[1]->symbolname;
    } else {
        paramName = "unknown"; // Not sure if this is correct or what we want to name it
    }

    // New node for the parameter
    struct param *newParam = malloc(sizeof(struct param));
    if (newParam == NULL) {
        fprintf(stderr, "Out of memory in alcFuncType (param)\n");
        exit(1);
    }
    newParam->name = strdup(paramName);
    newParam->type = paramType;
    newParam->next = NULL;

    // Add to the end of the list
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
    if (size != NULL && size->leaf != NULL) {
        rv->u.array.size = size->leaf->ival; // Assume int value?
    } else {
        rv->u.array.size = -1; // Unknown size
    }
    return rv;
}

