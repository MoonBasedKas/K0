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
        "unit"
    };
    if(!t) return "NULL";
    if(t->basicType < FIRST_TYPE || t->basicType > LAST_TYPE) return "UNKNOWN";
    return typeNames[t->basicType - NULL_TYPE];
}

/**
 * @brief Lookup the type of a node, finds it within its symbol table.
 * 
 * TODO: Proper scope retrival
 * 
 * @param n
 * @return typePtr
 */
typePtr lookupType(struct tree *n){
    struct symEntry *entry = NULL;
    if (n->prodrule == arrayIndex) {
        entry = contains(n->table, n->kids[0]->leaf->text);
        if(!entry) {
            fprintf(stderr, "Type lookup failed: No entry in table\n");
            return nullType_ptr;
        }
        return entry->type;
    }
    if(!n || !n->leaf || !n->leaf->text) {
        fprintf(stderr, "Type lookup failed: Invalid node\n");
        return nullType_ptr;
    }
    // It seems like there is a problem wiht finding the correct scope.
    entry = contains(n->table, n->leaf->text); //symTab.c
    if(!entry) {
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

void typeCheckExpression(struct tree *node)
{
    //base case
    if(node == NULL)
    {
        return;
    }

    if(node->nkids == 0)
    {
        leafExpression(node);
        return;
    }

    switch (node->prodrule)
    {
    case prefix:   
        typeCheckExpression(node->kids[1]);
        switch (node->kids[0]->prodrule)
        {
        case INCR:
        case DECR:
            switch (node->kids[1]->type->basicType)
            {
            case INT_TYPE:
                node->type = alcType(INT_TYPE); //type.c
                break;
            case DOUBLE_TYPE:
                node->type = alcType(DOUBLE_TYPE); //type.c
                break;
            case CHAR_TYPE:
                node->type = alcType(CHAR_TYPE); //type.c
            default:
                //type error
                break;
            }
            break;
        case ADD:
        case SUB:
            switch (node->kids[1]->type->basicType)
            {
            case INT_TYPE:
                node->type = alcType(INT_TYPE); //type.c
                break;
            case DOUBLE_TYPE:
                node->type = alcType(DOUBLE_TYPE); //type.c
                break;
            default:
                //type error
                break;
            }
            break;
        case EXCL_NO_WS:
        case EXCL_WS:
            if(typeEquals(node->kids[1]->type, booleanType_ptr))
            {
                node->type = alcType(BOOL_TYPE); //type.c
            }
            else
            {
                //type error
            }
        default:
            break;
        }
        break;
    case postfixIncr:
    case postfixDecr:
        typeCheckExpression(node->kids[0]);
        switch (node->kids[0]->type->basicType)
        {
        case INT_TYPE:
            node->type = alcType(INT_TYPE); //type.c
            break;
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); //type.c
            break;
        case CHAR_TYPE:
            node->type = alcType(CHAR_TYPE); //type.c
        default:
            //type error
            break;
        }
        break;
    // function call
    case postfixExpr:
    case postfixNoExpr:
        
        break;
    //built in stuff
    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
        //need to add
        break;
    default:
        binaryExpression(node);
        break;
    }
}

void leafExpression(struct tree *node)
{
    switch (node->prodrule)
    {
    //literal base cases
    case INTEGER_LITERAL:
    case HEX_LITERAL:
        node->type = alcType(INT_TYPE); //type.c
        break;
    case CHARACTER_LITERAL:
        node->type = alcType(CHAR_TYPE); //type.c
        break;
    case REAL_LITERAL:
        node->type = alcType(DOUBLE_TYPE); //type.c
        break;
    case TRUE:
    case FALSE:
        node->type = alcType(BOOL_TYPE); //type.c
        break;
    case NULL_K:
        node->type = alcType(NULL_TYPE); //type.c
        break;
    case LINE_STRING:
    case MULTILINE_STRING:
        node->type = alcType(STRING_TYPE); //type.c
        break;

    //variable base case
    //will this work as function base case or do we need to seperate that out??
    //postfixExpr and postfixNoExpr - erik is going to pull out
    //for funcs check param types
    case IDENTIFIER:
        //todo
    }
}

void binaryExpression(struct tree *node)
{
    typeCheckExpression(node->kids[0]);
    typeCheckExpression(node->kids[1]);
    switch (node->prodrule)
    {
    case disj:
    case conj:
        if(!(typeEquals(node->kids[0]->type, booleanType_ptr) && typeEquals(node->kids[0]->type, booleanType_ptr)))
        {
            //type error
        }
        node->type = alcType(BOOL_TYPE); //type.c
        break;
    case equal: 
    case notEqual: 
    case eqeqeq: 
    case notEqeqeq: 
        if(!typeEquals(node->kids[0]->type, node->kids[1]->type))
        {
            //type error
        }
        node->type = alcType(BOOL_TYPE); //type.c
        break;
    case less: 
    case greater: 
    case lessEqual: 
    case greaterEqual: 
        if(!typeEquals(node->kids[0]->type, node->kids[1]->type))
        {
            if(!(typeEquals(node->kids[0]->type, integerType_ptr) || typeEquals(node->kids[0]->type, booleanType_ptr))
                    || !(typeEquals(node->kids[1]->type, integerType_ptr) || typeEquals(node->kids[1]->type, booleanType_ptr)))
            {
                //type error
            }
        }
        if(typeEquals(node->kids[0]->type, arrayAnyType_ptr) || typeEquals(node->kids[0]->type, returnUnitType_ptr))
        {
            //type error
        }
        node->type = alcType(BOOL_TYPE); //type.c
        break;
    case in: 
        switch(node->kids[0]->type->basicType)
        {
        case INT_TYPE:
            if(!typeEquals(node->kids[1]->type, arrayIntegerType_ptr))
            {
                //type error
            }
            break;
        case DOUBLE_TYPE:
            if(!typeEquals(node->kids[1]->type, arrayDoubleType_ptr))
            {
                //type error
            }
            break;
        case CHAR_TYPE:
            if(!typeEquals(node->kids[1]->type, arrayCharType_ptr) && !typeEquals(node->kids[1]->type, stringType_ptr))
            {
                //type error
            }
            break;
        case STRING_TYPE:
            if(!typeEquals(node->kids[1]->type, arrayStringType_ptr) && !typeEquals(node->kids[1]->type, stringType_ptr))
            {
                //type error
            }
            break;
        case BOOL_TYPE:
            if(!typeEquals(node->kids[1]->type, arrayBooleanType_ptr))
            {
                //type error
            }
            break;
        case ARRAY_TYPE:
        case UNIT_TYPE:
            //type error
            break;
        default:
            //type error
            break;
        }
        node->type = alcType(BOOL_TYPE); //type.c
    case range:
    case rangeUntil:
        //need range types
        break;
    case add:
        switch (node->kids[0]->type->basicType)
        {
        case INT_TYPE:
            switch (node->kids[1]->type->basicType)
            {
            case INT_TYPE:
                node->type = alcType(INT_TYPE); //type.c
                break;
            case DOUBLE_TYPE:
                node->type = alcType(DOUBLE_TYPE); //type.c
                break;
            default:
                //type error
                break;
            }
        case DOUBLE_TYPE:
            if(typeEquals(node->kids[1]->type, integerType_ptr) || typeEquals(node->kids[1]->type, doubleType_ptr))
            {
                node->type = alcType(DOUBLE_TYPE); //type.c
            }
            else
            {
                //type error
            }
            break;
        case CHAR_TYPE:
            switch (node->kids[1]->type->basicType)
            {
            case CHAR_TYPE:
                node->type = alcType(CHAR_TYPE); //type.c
                break;
            case STRING_TYPE:
                node->type = alcType(STRING_TYPE); //type.c
                break;
            default:
                //type error
                break;
            }
            break;
        case STRING_TYPE:
            if(typeEquals(node->kids[1]->type, unitType_ptr))
            {
                //type error
            }
            node->type = alcType(STRING_TYPE); //type.c
            break;
        case BOOL_TYPE:
            //type error
            break;
        case ARRAY_TYPE:
            switch (node->kids[1]->type->basicType)
            {
            case INT_TYPE:
                if(typeEquals(node->kids[0]->type, arrayIntegerType_ptr))
                {
                    node->type = copyType(node->kids[0]->type);
                }
                else
                {
                    //type error
                }
                break;
            case DOUBLE_TYPE:
                if(typeEquals(node->kids[0]->type, arrayDoubleType_ptr))
                {
                    node->type = copyType(node->kids[0]->type);
                }
                else
                {
                    //type error
                }
                break;
            case CHAR_TYPE:
                if(typeEquals(node->kids[0]->type, arrayCharType_ptr))
                {
                    node->type = copyType(node->kids[0]->type);
                }
                else
                {
                    //type error
                }
                break;
            case STRING_TYPE:
                if(typeEquals(node->kids[0]->type, arrayStringType_ptr))
                {
                    node->type = copyType(node->kids[0]->type);
                }
                else
                {
                    //type error
                }
                break;
            case BOOL_TYPE:
                if(typeEquals(node->kids[0]->type, arrayBooleanType_ptr))
                {
                    node->type = copyType(node->kids[0]->type);
                }
                else
                {
                    //type error
                }
                break;
            case ARRAY_TYPE:
                if(typeEquals(node->kids[0]->type, node->kids[1]->type))
                {
                    node->type = copyType(node->kids[0]->type);
                }
                else
                {
                    //type error
                }
                break;
            case UNIT_TYPE:
                //type error
                break;
            default:
                //type error
                break;
            }
            break;
        case UNIT_TYPE:
            //type error
            break;
        default:
            //type error
            break;
        }
        break;
    case sub:
        switch (node->kids[0]->type->basicType)
        {
            case INT_TYPE:
                switch (node->kids[1]->type->basicType)
                {
                case INT_TYPE:
                    node->type = alcType(INT_TYPE); //type.c
                    break;
                case DOUBLE_TYPE:
                    node->type = alcType(DOUBLE_TYPE); //type.c
                    break;
                default:
                    //type error
                    break;
                }
                break;
            case DOUBLE_TYPE:
                switch (node->kids[1]->type->basicType)
                {
                case INT_TYPE:
                case DOUBLE_TYPE:
                    node->type = alcType(DOUBLE_TYPE); //type.c
                    break;
                default:
                    //type error
                    break;
                }
                break;
            case CHAR_TYPE:
                switch (node->kids[1]->type->basicType)
                {
                case INT_TYPE:
                    node->type = alcType(CHAR_TYPE); //type.c
                    break;
                case CHAR_TYPE:
                    node->type = alcType(INT_TYPE); //type.c
                    break;
                default:
                    //type error
                    break;
                }
                break;
            default:
                //type error
                break;
        }
        break;
    case mult:
    case div_k:
    case mod:
        switch (node->kids[0]->type->basicType)
        {
        case INT_TYPE:
            switch (node->kids[1]->type->basicType)
            {
            case INT_TYPE:
                node->type = alcType(INT_TYPE); //type.c
                break;
            case DOUBLE_TYPE:
                node->type = alcType(DOUBLE_TYPE); //type.c
                break;
            default:
                //type error
                break;
            }
            break;
        case DOUBLE_TYPE:
            switch (node->kids[1]->type->basicType)
            {
            case INT_TYPE:
            case DOUBLE_TYPE:
                node->type = alcType(DOUBLE_TYPE); //type.c
                break;
            default:
                //type error
                break;
            }
            break;
        default:
            //type error
            break;
        }
        break;
    case arrayAccess:
    case postfixArrayAccess:
        if(!typeEquals(node->kids[0]->type, arrayAnyType_ptr))
        {
            //type error
            break;
        }
        if(!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            //type error
            break;
        }
        node->type = alcType(node->kids[0]->type->u.array.elemType->basicType);  //type.c
    default:
        //error???
        break;
    }
}