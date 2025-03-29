/*
Type helpers for the semantic analysis phase.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typeHelpers.h"
#include "symTab.h"
#include "lex.h"
#include "type.h"
#include "tree.h"
#include "symNonTerminals.h"
#include "k0gram.tab.h"

struct symTab *globalSymTab = NULL;
extern typePtr nullType_ptr;
extern int symError;

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
 * Do we need to handle operators?
 * Elvis, dot
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
    struct symEntry *entry = contains(globalSymTab, n->leaf->text); //symTab.c
    if(!entry) {
        fprintf(stderr, "Type lookup failed: No symbol table\n");
        return nullType_ptr;
    }
    return entry->type;
}

/**
 * @brief Create a parameter from a tree node
 *
 * @param paramNode
 * @return struct param*
 */
struct param* createParamFromTree(struct tree *paramNode) {
    typePtr paramType = NULL;
    char *paramName = NULL;

    // Extract parameter type
    if (paramNode->nkids >= 1) {
        if (paramNode->kids[0]->type != NULL)
            paramType = paramNode->kids[0]->type;
        else {
            fprintf(stderr, "Parameter type not found\n");
            exit(3);
        }
    } else {
        paramType = nullType_ptr;
    }

    // Extract parameter name
    if (paramNode->nkids >= 2) {
        paramName = paramNode->kids[1]->symbolname;
    } else {
        paramName = strdup("unknown"); // Duplicate to ensure proper memory handling
    }

    // Allocate and initialize the parameter struct
    struct param *newParam = malloc(sizeof(struct param));
    if (newParam == NULL) {
        fprintf(stderr, "Out of memory in createParamFromTree\n");
        exit(1);
    }
    newParam->name = strdup(paramName);
    newParam->type = paramType;
    newParam->next = NULL;

    // If paramName was strdup-ed because of missing symbolname, free our temporary copy
    if (paramNode->nkids < 2) {
        free(paramName);
    }
    
    return newParam;
}

/**
 * @brief Determine the return type of a function
 *
 * @param r
 * @return typePtr
 */
typePtr determineReturnType(struct tree *r) {
    if (r != NULL) {
        if (r->type != NULL)
            return r->type;
        else {
            fprintf(stderr, "Function return type not found\n");
            symError = 3;
        }
    }
    return nullType_ptr; // Default to null type if no return type is provided
}

/**
 * @brief Extract the size of an array
 *
 * @param size
 * @return int
 */
int extractArraySize(struct tree *size) {
    if (size != NULL && size->leaf != NULL) {
        return size->leaf->ival;
    }
    return -1; // Unknown size
}


/**
 * @brief Check if two types are equal
 *
 * @param type1
 * @param type2
 * @return int
 */
int typeEquals(typePtr type1, typePtr type2)
{
    if(type1->basicType == type2->basicType)
    {
        if(type1->basicType != ARRAY_TYPE)
        {
            if(!typeEquals(type1->u.array.elemType, type2->u.array.elemType))
            {
                //if array type doesn't match types aren't equal
                return 0;
            }
        }
        if(type1->basicType != FUNCTION_TYPE)
        {

        }
        if(type1->basicType != ANY_TYPE)
        {

        }
    
        return 1;
    }
    //if basic types don't match then the types can't be equal
    return 0;
}

/**
 * @brief returns a copy of the passed type
 *
 * @param type
 * @return typePtr
 */
typePtr copyType(typePtr type)
{

}

/**
 * @brief frees memeory allocated for type
 *
 * @param type
 * @return typePtr
 */
void deleteType(typePtr type)
{

}

typePtr *typeCheckExpression(struct tree *node)
{
    if(node == NULL)
    {
        return NULL;
    }

    switch (node->prodrule)
    {
    case INTEGER_LITERAL:
    case HEX_LITERAL:
    case CHARACTER_LITERAL:
    case REAL_LITERAL:
    case TRUE:
    case FALSE:
    case NULL_K:
    case LINE_STRING:
    case MULTILINE_STRING:


    case disj: //bool bool, bool
    case conj: //bool bool, bool

    case equal: //? ?, bool
    case notEqual: //? ?, bool
    case eqeqeq: //? ?, bool
    case notEqeqeq: //? ?, bool

    case less: //int int, bool - double double, bool
    case greater: //int int, bool - double double, bool
    case lessEqual: //int int, bool - double double, bool
    case greaterEqual: //int int, bool - double double, bool

    case in: //? ?, bool

    case range:
    case rangeUntil:

    case add: 
    case sub:
    case mult:
    case div_k:


    case mod:


    case prefix: //assignable expression for ++ and --


    case arrayAccess:


    case postfixExpr:
    case postfixNoExpr:
    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
    case postfixArrayAccess:
    case postfixIncr:
    case postfixDecr:
        
    
    default:
        break;
    }
}