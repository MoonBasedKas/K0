#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "symTab.h"
#include "lex.h"
#include "k0gram.tab.h"
#include "semanticBuild.h"
#include "typeHelpers.h"
#include "errorHandling.h"

struct typeInfo null_type = {NULL_TYPE};
struct typeInfo byte_type = {BYTE_TYPE};
struct typeInfo integer_type = {INT_TYPE};
struct typeInfo double_type = {DOUBLE_TYPE};
struct typeInfo boolean_type = {BOOL_TYPE};
struct typeInfo char_type = {CHAR_TYPE};
struct typeInfo string_type = {STRING_TYPE};
struct typeInfo unit_type = {UNIT_TYPE};
struct typeInfo range_type = {RANGE_TYPE};
extern struct symTab *rootScope;

typePtr nullType_ptr = &null_type;
typePtr byteType_ptr = &byte_type;
typePtr integerType_ptr = &integer_type;
typePtr doubleType_ptr = &double_type;
typePtr booleanType_ptr = &boolean_type;
typePtr charType_ptr = &char_type;
typePtr stringType_ptr = &string_type;
typePtr unitType_ptr = &unit_type;
typePtr rangeType_ptr = &range_type;

typePtr alcType(int baseType) {

    typePtr rv;

    switch (baseType) {
        case NULL_TYPE:
            return nullType_ptr;
        case BYTE_TYPE:
            return byteType_ptr;
        case INT_TYPE:
            return integerType_ptr;
        case DOUBLE_TYPE:
            return doubleType_ptr;
        case BOOL_TYPE:
            return booleanType_ptr;
        case CHAR_TYPE:
            return charType_ptr;
        case STRING_TYPE:
            return stringType_ptr;
        case UNIT_TYPE:
            return unitType_ptr;
        case RANGE_TYPE:
            return rangeType_ptr;
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
    if (r->prodrule == arrayType){
        rv->u.func.returnType = alcArrayType(NULL, r->kids[1]->type);
    } else {
        rv->u.func.returnType = determineReturnType(r); //typeHelpers.c
    }
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
    if (size == NULL) {
        rv->u.array.size = -1; // array as a return type
    } else {
        rv->u.array.size = extractArraySize(size); //typeHelpers.c
    }
    return rv;
}

/*
    Lily's Dummy Pointers
*/

/* Global array types for each basic type. 
   Using size -1 to indicate an unknown size. */
struct typeInfo array_integer_type = {
    .basicType = ARRAY_TYPE,
    .u.array = { .size = -1, .elemType = &integer_type }
};
struct typeInfo array_byte_type = {
    .basicType = ARRAY_TYPE,
    .u.array = { .size = -1, .elemType = &byte_type }
};
struct typeInfo array_double_type = {
    .basicType = ARRAY_TYPE,
    .u.array = { .size = -1, .elemType = &double_type }
};
struct typeInfo array_boolean_type = {
    .basicType = ARRAY_TYPE,
    .u.array = { .size = -1, .elemType = &boolean_type }
};
struct typeInfo array_char_type = {
    .basicType = ARRAY_TYPE,
    .u.array = { .size = -1, .elemType = &char_type }
};
struct typeInfo array_string_type = {
    .basicType = ARRAY_TYPE,
    .u.array = { .size = -1, .elemType = &string_type }
};

/* any array? idk */
static struct typeInfo any_elem_type = { .basicType = -1 }; // should be "any" elem
struct typeInfo array_any_type = {
    .basicType = ARRAY_TYPE,
    .u.array = { .size = -1, .elemType = &any_elem_type }
};

/* Range types */
struct typeInfo range_integer_type = {
    .basicType = RANGE_TYPE,
    .u.range = { .elemType = &integer_type }
};

/* Global range type pointers */
typePtr rangeIntegerType_ptr = &range_integer_type;

/* Global array type pointers */
typePtr arrayIntegerType_ptr = &array_integer_type;
typePtr arrayByteType_ptr    = &array_byte_type;
typePtr arrayDoubleType_ptr  = &array_double_type;
typePtr arrayBooleanType_ptr = &array_boolean_type;
typePtr arrayCharType_ptr    = &array_char_type;
typePtr arrayStringType_ptr  = &array_string_type;
typePtr arrayAnyType_ptr     = &array_any_type;

/* Global return type pointers; these alias the corresponding basic types */
typePtr returnIntegerType_ptr = &integer_type;
typePtr reuturnByteType_ptr   = &byte_type;
typePtr returnDoubleType_ptr  = &double_type;
typePtr returnBooleanType_ptr = &boolean_type;
typePtr returnCharType_ptr    = &char_type;
typePtr returnStringType_ptr  = &string_type;
typePtr returnUnitType_ptr    = &unit_type;