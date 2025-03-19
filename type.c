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
    if (rv == NULL) {
        return NULL;
    
    rv->u.func.st = st;
    // Traverse subtrees
    }

