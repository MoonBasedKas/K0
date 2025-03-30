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

static struct param *copyParamList(struct param *params);


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
    if(type1 == NULL || type2 == NULL)
        return 0;

    if(type1->basicType != type2->basicType)
        return 0;

    // Keep the switch train alive
    switch(type1->basicType)
    {
        case ARRAY_TYPE:
        {
            return typeEquals(type1->u.array.elemType, type2->u.array.elemType);
        }
        case FUNCTION_TYPE:
            if(!typeEquals(type1->u.func.returnType, type2->u.func.returnType))
                return 0;

            if(type1->u.func.numParams != type2->u.func.numParams)
                return 0;

        {
            // Need to check parameters
            struct param *param1 = type1->u.func.parameters;
            struct param *param2 = type2->u.func.parameters;
            while(param1 != NULL && param2 != NULL)
            {
                if(!typeEquals(param1->type, param2->type))
                    return 0;
                param1 = param1->next;
                param2 = param2->next;
            }
            if(param1 || param2)
                return 0;
            // If we get here, all parameters matched
            return 1;
        }
        case ANY_TYPE:
            return 1;
        default:
            // If we get here, basic types match
            return 1;
    }
}

/**
 * @brief returns a copy of the passed type
 *
 * @param type
 * @return typePtr
 */
typePtr copyType(typePtr type)
{
    if(type == NULL)
        return NULL;

    typePtr copy = malloc(sizeof(*type));
    if(copy == NULL)
    {
        fprintf(stderr, "Out of memory in copyType\n");
        exit(EXIT_FAILURE);
    }

    copy->basicType = type->basicType;

    switch(type->basicType)
    {
        case ARRAY_TYPE:
        {
            copy->u.array.size = type->u.array.size;
            copy->u.array.elemType = copyType(type->u.array.elemType);
            break;
        }
        case FUNCTION_TYPE:
        {
            // Defined flag
            copy->u.func.defined = type->u.func.defined;

            // Name
            if(type->u.func.name != NULL)
                copy->u.func.name = strdup(type->u.func.name);
            else
                copy->u.func.name = NULL;

            // Does this need to be deep?
            copy->u.func.st = type->u.func.st;

            // Return type
            copy->u.func.returnType = copyType(type->u.func.returnType);

            // Number of parameters
            copy->u.func.numParams = type->u.func.numParams;

            // Copy linked list of parameters
            copy->u.func.parameters = copyParamList(type->u.func.parameters);
            break;
        }
        default:
            break;
    }
    return copy;
}

/**
 * @brief Copy a linked list of parameters
 *
 * @param params
 * @return struct param*
 */
static struct param *copyParamList(struct param *params)
{
    if(params == NULL)
        return NULL;

    struct param *head = NULL;
    struct param **lastPtrRef = &head;

    while(params != NULL)
    {
        struct param *newParam = malloc(sizeof(struct param));
        if(newParam == NULL)
        {
            fprintf(stderr, "Out of memory in copyParamList\n");
            exit(EXIT_FAILURE);
        }
        // Name
        newParam->name = (params->name != NULL) ? strdup(params->name) : NULL;

        // Type
        newParam->type = copyType(params->type);

        // Next
        newParam->next = NULL;

        // Add to list
        *lastPtrRef = newParam;
        lastPtrRef = &newParam->next;

        // Move to next parameter
        params = params->next;
    }
    return head;
}

/**
 * @brief frees memeory allocated for type
 *
 * @param type
 * @return typePtr
 */
void deleteType(typePtr type)
{
    if(type == NULL)
        return;

    switch(type->basicType)
    {
        case ARRAY_TYPE:
        {
            deleteType(type->u.array.elemType);
            break;
        }
        case FUNCTION_TYPE:
            if(type->u.func.name != NULL)
                free(type->u.func.name);
            deleteType(type->u.func.returnType);
        {
            struct param *p = type->u.func.parameters;
            while(p != NULL)
            {
                struct param *temp = p;
                p = p->next;
                if(temp->name != NULL)
                    free(temp->name);
                deleteType(temp->type);
                free(temp);
            }
            break;
        }
        default:
            break;
    }
    free(type);
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