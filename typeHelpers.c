/*
Type helpers for the semantic analysis phase.
*/

#include <stdio.h>
#include "typeHelpers.h"
#include "symTab.h"
#include "lex.h"
#include "type.h"
#include "tree.h"

struct symTab *globalSymTab = NULL;
extern typePtr nullType_ptr;

/**
 * @brief Check if a type is numeric (used for compatibility checks)
 *
 * @param t
 * @return int
 */
static int isNumeric(typePtr t){
    if(!t) return 0;
    switch(t->basicType){
        case INT_TYPE:
        case DOUBLE_TYPE:
        case FLOAT_TYPE:
        case LONG_TYPE:
        case SHORT_TYPE:
        case BYTE_TYPE:
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief Get the name of a type
 *
 * @param t
 * @return char*
 */
char *typeName(typePtr t){
    static char *typeNames[] = {
        "null",
        "byte",
        "int",
        "short",
        "long",
        "float",
        "double",
        "bool",
        "char",
        "string",
        "array",
        "function",
        "any"
    };
    if(!t) return "NULL";
    if(t->basicType < FIRST_TYPE || t->basicType > LAST_TYPE) return "UNKNOWN";
    return typeNames[t->basicType - FIRST_TYPE];
}

/**
 * @brief Check if two types are compatible
 *
 * @param type1
 * @param type2
 * @return int
 */
int compatible(typePtr type1, typePtr type2){
    if (!type1 || !type2) return 0;
    if (type1 == type2) return 1;
    if (type1->basicType == ANY_TYPE || type2->basicType == ANY_TYPE) return 1;
    if (isNumeric(type1) && isNumeric(type2)) return 1;
    return 0;
}

/**
 * @brief Lookup the type of a node
 *
 * @param n
 * @return typePtr
 */
typePtr lookupType(struct tree *n){
    if(!n || !n->leaf || !n->leaf->text) {
        fprintf(stderr, "Type lookup failed: Invalid node\n");
        return nullType_ptr;
    }
    struct symEntry *entry = contains(globalSymTab, n->leaf->text);
    if(!entry) {
        fprintf(stderr, "Type lookup failed: No symbol table\n");
        return nullType_ptr;
    }
    return entry->type;
}