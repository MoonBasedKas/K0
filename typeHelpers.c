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
#include "typeCheck.h"
#include "errorHandling.h"

// Bingo
// struct symTab *globalSymTab = rootScope;
extern typePtr nullType_ptr;
extern int symError;

static struct param *copyParamList(struct param *params);

/**
 * @brief Get the name of a type
 *
 * @param t
 * @return char*
 */
char *typeName(typePtr t)
{
    static char *typeNames[] = {
        "null",
        "byte",
        "int",
        "double",
        "bool",
        "char",
        "string",
        "array",
        "function",
        "unit",
        "range"};
    if (!t)
        return "NULL";
    if (t->basicType < FIRST_TYPE || t->basicType > LAST_TYPE)
        return "UNKNOWN";
    return typeNames[t->basicType - NULL_TYPE];
}

/**
 * @brief Return the size of a type
 *
 * @param type
 * @return int
 */
int typeSize(struct typeInfo *type)
{
    switch (type->basicType)
    {
    case INT_TYPE:
    case DOUBLE_TYPE:
    case STRING_TYPE: //???
    case ARRAY_TYPE:
        // need to know how many elements in array
        // figure this one out later
        // for now do 8 as a pointer to the array somewhere else
        // we pass the size in all allocations (Erik 95% sure)
        return 8;
    case BOOL_TYPE:
    case CHAR_TYPE:
        return 1;

    case FUNCTION_TYPE:
        // do we need?? and if so what looks like
    case UNIT_TYPE:  // do we need?? We do not. This was essentially an "all/void" which we don't support.
    case RANGE_TYPE: // do we need? Number of elements? (1 to 20) - idk
        /* code */
        break;

    default:
        break;
    }
    return -1; // This only exists to remove a compilation warning.
}

/**
 * @brief Lookup the type of a node, finds it within its symbol table.
 *
 * TODO: Proper scope retrival
 *
 * @param n
 * @return typePtr
 */
typePtr lookupType(struct tree *n)
{
    struct symEntry *entry = NULL;
    if (n->prodrule == arrayIndex)
    {
        entry = contains(n->table, n->kids[0]->leaf->text);
        if (!entry)
        {
            fprintf(stderr, "Type lookup failed: No entry in table\n");
            return nullType_ptr;
        }
        return entry->type;
    }
    if (!n || !n->leaf || !n->leaf->text)
    {
        fprintf(stderr, "Type lookup failed: Invalid node\n");
        return nullType_ptr;
    }
    // It seems like there is a problem wiht finding the correct scope.
    entry = contains(n->table, n->leaf->text); // symTab.c
    if (!entry)
    {
        fprintf(stderr, "Type lookup failed: No entry in table\n");
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
struct param *createParamFromTree(struct tree *paramNode)
{
    typePtr paramType = NULL;
    char *paramName = NULL;

    // Extract parameter type
    if (paramNode->nkids >= 2)
    {
        if (paramNode->kids[1]->type != NULL)
            paramType = paramNode->kids[1]->type;
        else
        {
            typeError("Parameter type not found on paramNode\n", paramNode);
        }
        if (paramNode->kids[0] && paramNode->kids[0]->leaf)
            paramName = paramNode->kids[0]->leaf->text;
    }
    else
    {
        return NULL;
    }

    // Allocate and initialize the parameter struct
    struct param *newParam = malloc(sizeof(struct param));
    if (newParam == NULL)
    {
        fprintf(stderr, "Out of memory in createParamFromTree\n");
        exit(1);
    }
    newParam->name = strdup(paramName);
    newParam->type = paramType;
    newParam->next = NULL;

    return newParam;
}

/**
 * @brief A basic function which allocates param nodes.
 *
 * @param paramName
 * @param type
 * @return struct param*
 */
struct param *alcParams(char *paramName, int type)
{
    struct param *param = malloc(sizeof(struct param));
    param->name = strdup(paramName);
    param->type = alcType(type);
    param->next = NULL;
    return param;
}

/**
 * @brief A wrapper to allocate function params.
 *
 *
 * @param params - Number of params to input
 * @param i      - Param name
 * @param i + 1  - Param type
 * @return struct param*
 */
struct param *buildfuncParams(int params, ...)
{
    va_list args;
    char *name;
    int bigT;
    struct param *funParams = NULL;
    struct param *mostRecent = NULL;
    struct param *prev = NULL;
    va_start(args, params);
    for (int i = 0; i < params; i++)
    {
        name = va_arg(args, char *);
        bigT = va_arg(args, int);
        mostRecent = alcParams(name, bigT);
        if (!funParams)
        {
            funParams = mostRecent;
            prev = mostRecent;
        }
        else
        {
            prev->next = mostRecent;
            prev = prev->next;
        }
    }
    va_end(args);

    return 0;
}

/**
 * @brief Determine the return type of a function
 *
 * @param r
 * @return typePtr
 */
typePtr determineReturnType(struct tree *r)
{
    if (r != NULL)
    {
        if (r->type != NULL)
            return r->type;
        else if (r->prodrule == 264)
        {
            typeError("Void function return type not supported", r);
            symError = 3;
        }
        else
        {
            typeError("Function return type not found", r);
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
int extractArraySize(struct tree *size)
{
    if (size != NULL && size->leaf != NULL)
    {
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
    if (type1 == NULL || type2 == NULL)
        return 0;

    if (type1->basicType != type2->basicType)
        return 0;

    // Keep the switch train alive
    switch (type1->basicType)
    {
    case ARRAY_TYPE:
    {
        return typeEquals(type1->u.array.elemType, type2->u.array.elemType);
    }
    case FUNCTION_TYPE:
        if (!typeEquals(type1->u.func.returnType, type2->u.func.returnType))
            return 0;

        if (type1->u.func.numParams != type2->u.func.numParams)
            return 0;

        {
            // Need to check parameters
            struct param *param1 = type1->u.func.parameters;
            struct param *param2 = type2->u.func.parameters;
            while (param1 != NULL && param2 != NULL)
            {
                if (!typeEquals(param1->type, param2->type))
                    return 0;
                param1 = param1->next;
                param2 = param2->next;
            }
            if (param1 || param2)
                return 0;
            // If we get here, all parameters matched
            return 1;
        }
    case UNIT_TYPE:
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
    if (type == NULL)
        return NULL;

    typePtr copy = malloc(sizeof(*type));
    if (copy == NULL)
    {
        fprintf(stderr, "Out of memory in copyType\n");
        exit(EXIT_FAILURE);
    }

    copy->basicType = type->basicType;

    switch (type->basicType)
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
        if (type->u.func.name != NULL)
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
    if (params == NULL)
        return NULL;

    struct param *head = NULL;
    struct param **lastPtrRef = &head;

    while (params != NULL)
    {
        struct param *newParam = malloc(sizeof(struct param));
        if (newParam == NULL)
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
    if (type == NULL)
        return;

    switch (type->basicType)
    {
    case ARRAY_TYPE:
    {
        deleteType(type->u.array.elemType);
        break;
    }
    case FUNCTION_TYPE:
        if (type->u.func.name != NULL)
            free(type->u.func.name);
        deleteType(type->u.func.returnType);
        {
            struct param *p = type->u.func.parameters;
            while (p != NULL)
            {
                struct param *temp = p;
                p = p->next;
                if (temp->name != NULL)
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

/**
 * @brief Supported import functions struct for checking
 *
 */
typedef struct
{
    const char *funcName;
    int paramCount; // number of required parameters
} supportedImportFunc;

/**
 * @brief Supported import functions
 *
 */
static supportedImportFunc knownImports[] = {
    {"abs", 1},
    {"pow", 2},
    {"max", 2},
    {"min", 2},
    {"equals", 1},
    {"length", 0},
    {"get", 1},
    {NULL, 0} // sentinel
};

/**
 * @brief get the param count of an import function
 *
 * @param fnName
 * @return int
 */
int getImportParamCount(const char *fnName)
{
    for (int i = 0; knownImports[i].funcName != NULL; i++)
    {
        if (strcmp(fnName, knownImports[i].funcName) == 0)
        {
            return knownImports[i].paramCount;
        }
    }
    return -1; // means not a recognized import function (keep it movin)
}

/**
 * @brief count the number of expressions in an expression list
 *
 * @param exprList
 * @return int
 */
int countExprList(struct tree *exprList)
{
    if (!exprList)
        return 0;

    // If exprList->prodrule != expressionList, it may be a single expression (no comma)
    if (exprList->prodrule == expressionList)
    {
        return 1 + countExprList(exprList->kids[1]);
    }
    return 1;
}