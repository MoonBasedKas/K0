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

extern int symError;

/**
 * @brief Recursivly assign types to an expression
 *
 * @param node
 */
void typeCheck(struct tree *node)
{
    struct symEntry *entry;

    //base case
    if(node == NULL)
    {
        return;
    }

    for(int i = 0; i < node->nkids; i++)
    {
        typeCheck(node->kids[i]);
    }

    if(node->nkids == 0)
    {
        leafExpression(node);
        return;
    }

    switch (node->prodrule)
    {
    case prefix:
        prefixExpression(node);
        break;
    case postfixIncr:
    case postfixDecr:
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
            typeError("Postfix ++ and -- operators can only be applied to types: Int, Double, Char", node);
            break;
        }
        break;
    // function call
    case postfixNoExpr:
        entry = returnType(node->kids[0]);
        if(entry->type->u.func.numParams !=0)
        {
            typeError("Function call missing arguments", node);
        }
        break;
    case postfixExpr:
        paramTypeCheck(node->kids[0], node->kids[1]);
        break;
    //built in stuff, don't deal with safety here
    case postfixDotID:
        checkImport(node->kids[0], node->kids[2]);
        node->type = copyType(node->kids[2]->type);
        break;
    case postfixSafeDotID:
        checkImport(node->kids[0], node->kids[3]);
        node->type = copyType(node->kids[3]->type);
        break;
    case postfixDotIDExpr:
        checkImport(node->kids[0], node->kids[2]);
        paramTypeCheck(node->kids[2], node->kids[3]); 
    case postfixSafeDotIDExpr:
        checkImport(node->kids[0], node->kids[3]);
        paramTypeCheck(node->kids[3], node->kids[4]); 
        break;
    case postfixDotIDNoExpr:
        checkImport(node->kids[0], node->kids[2]);
        entry = returnType(node->kids[2]);
        if(entry->type->u.func.numParams !=0)
        {
            typeError("Function call missing arguments", node);
        }
        break;
    case postfixSafeDotIDNoExpr:
        checkImport(node->kids[0], node->kids[3]);
        entry = returnType(node->kids[3]);
        if(entry->type->u.func.numParams !=0)
        {
            typeError("Function call missing arguments", node);
        }
        break;

    //assigment
    case assignment:
    case arrayAssignment:
        if(!typeEquals(node->kids[0]->type, node->kids[1]->type))
        {
            typeError("Types must match for assigmnet", node);
        }
        node->type = copyType(node->kids[0]->type); //typeHelpers.c
        break;
    case assignAdd:
    case arrayAssignAdd:
        assignAddExpression(node);
        break;
    case assignSub:
    case arrayAssignSub:
        assignSubExpression(node);
        break;

    //statments
    case forStmntWithVars:
        //might need something different here??? Not sure
    case forStmnt:
        forStatement(node);
        break;
    case whileStmntCtrlBody:
    case whileStmnt:
        if(!typeEquals(node->kids[1]->type, booleanType_ptr))
        {
            typeError("While condition must be of type Boolean", node);
        }
        break;
    case doWhileStmnt:
        if(!typeEquals(node->kids[3]->type, booleanType_ptr))
        {
            typeError("While condition must be of type Boolean", node);
        }
        break;

    //need ifs
    case emptyIf:
    case if_k:
        if(ifAssigned(node))
        {
            if(node->kids[2]->type == NULL)
            {
                typeError("When assigned if statment bodies must be expressions", node);
            }
            node->type = copyType(node->kids[2]->type); //typeHelpers.c
        }
        if(!typeEquals(node->kids[1]->type, booleanType_ptr))
        {
            typeError("If condition must be of type Boolean", node);
        }
        break;
    case ifElse:
    case ifElseIf:
        if(ifAssigned(node))
        {
            if(node->kids[2]->type == NULL)
            {
                typeError("When assigned if statment bodies must be expressions", node);
            }
            if(node->kids[4]->type == NULL)
            {
                typeError("When assigned if statment bodies must be expressions", node);
            }
            if(!typeEquals(node->kids[2]->type, node->kids[4]->type))
            {
                typeError("When assigned if statment bodies must have matching types", node);
            }
            node->type = copyType(node->kids[2]->type); //typeHelpers.c
        }
        if(!typeEquals(node->kids[1]->type, booleanType_ptr))
        {
            typeError("If condition must be of type Boolean", node);
        }
        break;
    case blockStmnts:
    case statement:
        node->type = copyType(node->kids[0]->type); //typeHelpers.c
        break;
    case disj:
    case conj: // Changed kids[0] && kids[0] to kids[0] && kids[1]
        if(!(typeEquals(node->kids[0]->type, booleanType_ptr) && typeEquals(node->kids[1]->type, booleanType_ptr)))
        {
            typeError("|| and && operators must have arguments of type Boolean", node);
        }
        node->type = alcType(BOOL_TYPE); //type.c
        break;
    case equal:
    case notEqual:
    case eqeqeq:
    case notEqeqeq:
        if(!typeEquals(node->kids[0]->type, node->kids[1]->type))
        {
            typeError("Equality operators must have arguments of the same type", node);
        }
        node->type = alcType(BOOL_TYPE); //type.c
        break;
    case less:
    case greater:
    case lessEqual:
    case greaterEqual:
        if(!typeEquals(node->kids[0]->type, node->kids[1]->type))
        {
            if(!(typeEquals(node->kids[0]->type, integerType_ptr) && typeEquals(node->kids[0]->type, doubleType_ptr))
                    && !(typeEquals(node->kids[1]->type, integerType_ptr) && typeEquals(node->kids[1]->type, doubleType_ptr)))
            {
                typeError("Comparison operators must have arguments of the same type or one Int and one Double arugment", node);
            }
        }
        if(typeEquals(node->kids[0]->type, arrayAnyType_ptr) || typeEquals(node->kids[0]->type, returnUnitType_ptr))
        {
            typeError("Cannot compare Array or Unit types", node);
        }
        node->type = alcType(BOOL_TYPE); //type.c
        break;
    case in:
        inExpression(node);
        break;
    case range:
        if(!typeEquals(node->kids[0]->type, integerType_ptr))
        {
            typeError("Range must be of type Int", node);
        }
        if(!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Range types must match", node);
        }
        node->type = alcType(RANGE_TYPE);
        node->type->u.range.elemType = alcType(INT_TYPE);
        node->type->u.range.until = 0;
        break;
    case rangeUntil:
        if(!typeEquals(node->kids[0]->type, integerType_ptr))
        {
            typeError("Range must be of type Int", node);
        }
        if(!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Range types must match", node);
        }
        node->type = alcType(RANGE_TYPE);
        node->type->u.range.elemType = alcType(INT_TYPE);
        node->type->u.range.until = 1;
        break;
    case add:
        addExpression(node);
        break;
    case sub:
        subExpression(node);
        break;
    case mult:
    case div_k:
    case mod:
        multaplicativeExpression(node);
        break;
    case arrayAccess:
    case postfixArrayAccess:
    case arrayIndex:
        if(!typeEquals(node->kids[0]->type, arrayAnyType_ptr))
        {
            typeError("Array access must be performed on an array", node);
            break;
        }
        if(!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Must use Int to determine the index of array element", node);
            break;
        }
        node->type = alcType(node->kids[0]->type->u.array.elemType->basicType);  //type.c

    case propDecAssign:
        if(!typeEquals(node->kids[1]->kids[0]->type, node->kids[2]->type))
        {
            typeError("Types must match for assigmnet", node);
        }
        break;
    case propDecReceiverAssign:
    case propDecTypeParamsAssign:
        if(!typeEquals(node->kids[2]->kids[0]->type, node->kids[3]->type))
        {
            typeError("Types must match for assigmnet", node);
        }
        break;
    case propDecAll:
        if(!typeEquals(node->kids[3]->kids[0]->type, node->kids[4]->type))
        {
            typeError("Types must match for assigmnet", node);
        }
        break;
    case funcValParamAssign:
        if(!typeEquals(node->kids[0]->kids[0]->type, node->kids[1]->type))
        {
            typeError("Types must match for assigmnet", node);
        }
        break;

    case funcBody:
        if(!typeEquals(node->parent->kids[1]->type->u.func.returnType, node->kids[1]->type))
        {
            typeError("Types must match for assigmnet", node);
        }
        break;

    case returnVal:
        returnCheck(node, node->kids[1]->type);
        break;
    case RETURN:
        returnCheck(node, unitType_ptr);    

    case arrayDecValueless:
        if(!typeEquals(node->kids[1]->kids[0]->type, arrayAnyType_ptr))
        {
            typeError("Cannot assign Array to non-Array variable", node);
        }
        break;
    case arrayDecEqualValueless:
        if(!typeEquals(node->kids[1]->kids[0]->type, arrayAnyType_ptr))
        {
            typeError("Cannot assign Array to non-Array variable", node);
        }
        break;
    case arrayDec:
        arrayDeclaration(node->kids[1]->kids[0], node->kids[3]->kids[1]);
        break;
    case arrayDecEqual: 
        arrayDeclaration(node->kids[1]->kids[0], node->kids[5]->kids[1]);
        break;
    
    case arraySizeIdent: 
        if(!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Array size must be of type Int", node);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief Returns true if the parent of the node is an assigment. (Or great-great ... grandparent for else if)
 *
 * @return boolean
 * @param node
 */
int ifAssigned(struct tree *node)
{
    switch (node->parent->prodrule)
    {
    case ifElseIf:
        return ifAssigned(node->parent);
        break;
    case assignment:
    case arrayAssignment:
    case assignAdd:
    case arrayAssignAdd:
    case assignSub:
    case arrayAssignSub:
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

/**
 * @brief Assigns return type to a fucntion call expression
 *
 * @param node
 */
struct symEntry *returnType(struct tree *node) //change to identifier node
{
    struct symTab *scope = node->table; //symTab.h
    struct symEntry *entry;             //symTab.h
    int found = 0;
    while(scope->parent != rootScope && found == 0)
    {   
        printf("scope: %s\n", scope->name);
        entry = contains(scope, node->leaf->text); //symTab.h
        if(entry != NULL)
        {
            node->parent->type = entry->type->u.func.returnType;
            found = 1;
        }
    }

    entry = contains(rootScope, node->leaf->text); //symTab.h
    if(entry != NULL)
    {
        node->parent->type = entry->type->u.func.returnType;
        found = 1;
    }
    if(found == 0)
    {
        typeError("Function not found", node);
    }
    return entry;
}

/**
 * @brief Assigns return type and checks the parameters of a function call expression
 *
 * @param node
 */
void paramTypeCheck(struct tree *id, struct tree *exprList)
{
    struct symEntry *entry = returnType(id);
    // struct tree *exprList = node->kids[1];
    struct param *paramList = id->type->u.func.parameters;
    if (entry->type->u.func.numParams == 0)
    {
        typeError("Function call has too many arguments", id->parent);
        return;
    }
    if (entry->type->u.func.numParams > 1)
    {
        for (int i = 0; i < entry->type->u.func.numParams - 1; i++)
        {
            if (exprList->prodrule != expressionList)
            {
                typeError("Function call missing arguments", id->parent);
                return;
            }
            if (!typeEquals(exprList->kids[0]->type, paramList->type))
            {
                typeError("Paramater type mismatch", id->parent);
            }
            exprList = exprList->kids[1];
            paramList = paramList->next;
        }
    }
    if (exprList->prodrule == expressionList)
    {
        typeError("Function call has too many arguments", id->parent);
        return;
    }
    if (!typeEquals(exprList->type, paramList->type))
    {
        typeError("Paramater type mismatch", id->parent);
    }
}

/**
 * @brief Checks that an imported function or variable exists and has been imported
 * 
 * @param import
 * @param name
 */
void checkImport(struct tree *import, struct tree *element)
{

}

/**
 * @brief Recursivly (with typeCheck) assigns types to prefix expressions
 *
 * @param node
 */
void prefixExpression(struct tree *node)
{
    switch (node->kids[0]->prodrule)
    {
    case INCR:
    case DECR:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            node->type = alcType(INT_TYPE); // type.c
            break;
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); // type.c
            break;
        case CHAR_TYPE:
            node->type = alcType(CHAR_TYPE); // type.c
        default:
            typeError("Prefex ++ and -- operators can only be applied to types: Int, Double, Char", node);
            break;
        }
        break;
    case ADD:
    case SUB:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            node->type = alcType(INT_TYPE); // type.c
            break;
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); // type.c
            break;
        default:
            typeError("- and + prefix operators must have arugment of type Int or Double", node);
            break;
        }
        break;
    case EXCL_NO_WS:
    case EXCL_WS:
        if (typeEquals(node->kids[1]->type, booleanType_ptr))
        {
            node->type = alcType(BOOL_TYPE); // type.c
        }
        else
        {
            typeError("! prefix operators must have arugment of type Boolean", node);
        }
        break;
    default:
        break;
    }
}

/**
 * @brief Assign types to leaf nodes
 *
 * @param node
 */
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
    case IDENTIFIER:
        // // If table is NULL it shouldn't need to be checked
        // // ie if it is a function call IDENTIFIER

        // //if this is in the function declaratoin it should be looking in the scope that the function was declared in
        // //so the node should point to that table
        // //ask erik if thats not how it works
        // //tho i guess it doesn't really matter since the declaration won't be checked
        // if (node->table == NULL) {
        //     break;
        // }
        // struct symTab *scope = node->table; //symTab.h
        // struct symEntry *entry;             //symTab.h
        // while(scope->parent != NULL)
        // {
        //     entry = contains(scope, node->leaf->text); //symTab.h
        //     if(entry != NULL)
        //     {
        //         node->type = entry->type;
        //         break;
        //     }
        // }
        // break;
    default:
        break;
    }
}

/**
 * @brief Assign types to add assigment expressions
 *
 * @param node
 */
void assignAddExpression(struct tree *node)
{
    switch (node->kids[0]->type->basicType)
    {
    case INT_TYPE:
        if (!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Int += x, x must be type Int", node);
        }
        node->type = alcType(INT_TYPE); //type.c
        break;
    case DOUBLE_TYPE:
        if (!(typeEquals(node->kids[1]->type, integerType_ptr) || typeEquals(node->kids[1]->type, doubleType_ptr)))
        {
            typeError("Double += x, x must be type Int or Double", node);
        }
        node->type = alcType(DOUBLE_TYPE); //type.c
        break;
    case CHAR_TYPE:
        if (!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Char += x, x must be type Int", node);
        }
        node->type = alcType(CHAR_TYPE); //type.c
        break;
    case STRING_TYPE:
        node->type = alcType(STRING_TYPE); //type.c
        break;
    case ARRAY_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            if (typeEquals(node->kids[0]->type, arrayIntegerType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only += type Int to Array<Int>", node);
            }
            break;
        case DOUBLE_TYPE:
            if (typeEquals(node->kids[0]->type, arrayDoubleType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only += type Double to Array<Double>", node);
            }
            break;
        case CHAR_TYPE:
            if (typeEquals(node->kids[0]->type, arrayCharType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only += type Char to Array<Char>", node);
            }
            break;
        case STRING_TYPE:
            if (typeEquals(node->kids[0]->type, arrayStringType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only += type String to Array<String>", node);
            }
            break;
        case BOOL_TYPE:
            if (typeEquals(node->kids[0]->type, arrayBooleanType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only += type Boolean to Array<Boolean>", node);
            }
            break;
        case ARRAY_TYPE:
            if (typeEquals(node->kids[0]->type, node->kids[1]->type))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only += arrays that have the same subtype", node);
            }
            break;
        case UNIT_TYPE:
            typeError("Cannot add with Unit type", node);
            break;
        default:
            typeError("", node);
            break;
        }
        break;
    default:
        typeError("The += operator cannot be used with types Boolean or Unit", node);
        break;
    }
}

/**
 * @brief Assign types sub assignment expressions
 *
 * @param node
 */
void assignSubExpression(struct tree *node)
{
    switch (node->kids[0]->type->basicType)
    {
    case INT_TYPE:
        if(!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Int -= x, x must be type Int", node);
        }
        node->type = alcType(INT_TYPE); //type.c
        break;
    case DOUBLE_TYPE:
        if (!(typeEquals(node->kids[1]->type, integerType_ptr) || typeEquals(node->kids[1]->type, doubleType_ptr)))
        {
            typeError("Double -= x, x must be type Int or Double", node);
        }
        node->type = alcType(DOUBLE_TYPE); //type.c
        break;
    case CHAR_TYPE:
        if (!typeEquals(node->kids[1]->type, integerType_ptr))
        {
            typeError("Char += x, x must be type Int", node);
        }
        node->type = alcType(CHAR_TYPE); //type.c
        break;
    default:
        typeError("The -= operator can only be used with types Int, Double, and Char", node);
        break;
    }
}

/**
 * @brief Type checks for statments
 *
 * @param node
 */
void forStatement(struct tree *node)
{
    switch (node->kids[1]->type->basicType)
    {
    case INT_TYPE:
        if (!(typeEquals(node->kids[2]->type, arrayIntegerType_ptr) || typeEquals(node->kids[2]->type, rangeIntegerType_ptr)))
        {
            typeError("Expression in four statment must be itterable over Ints", node);
        }
        break;
    case DOUBLE_TYPE:
        if (!typeEquals(node->kids[2]->type, arrayDoubleType_ptr))
        {
            typeError("Expression in four statment must be itterable over Doubles", node);
        }
        break;
    case CHAR_TYPE:
        if (!(typeEquals(node->kids[2]->type, arrayCharType_ptr) || typeEquals(node->kids[2]->type, stringType_ptr)))
        {
            typeError("Expression in four statment must be itterable over Chars", node);
        }
        break;
    case STRING_TYPE:
        if (!typeEquals(node->kids[2]->type, arrayStringType_ptr))
        {
            typeError("Expression in four statment must be itterable over Strings", node);
        }
        break;
    case BOOL_TYPE:
        if (!typeEquals(node->kids[2]->type, arrayBooleanType_ptr))
        {
            typeError("Expression in four statment must be itterable over Booleans", node);
        }
        break;
    default:
        typeError("For loop - Invalid variable type", node);
        break;
    }
}

/**
 * @brief Assigns types to in expressions
 *
 * @param node
 */
void inExpression(struct tree *node)
{
    switch (node->kids[0]->type->basicType)
    {
    case INT_TYPE:
        if (!typeEquals(node->kids[1]->type, arrayIntegerType_ptr))
        {
            typeError("Array type must match element type - Int", node);
        }
        break;
    case DOUBLE_TYPE:
        if (!typeEquals(node->kids[1]->type, arrayDoubleType_ptr))
        {
            typeError("Array type must match element type - Double", node);
        }
        break;
    case CHAR_TYPE:
        if (!typeEquals(node->kids[1]->type, arrayCharType_ptr) && !typeEquals(node->kids[1]->type, stringType_ptr))
        {
            typeError("Array type must match element type - Char", node);
        }
        break;
    case STRING_TYPE:
        if (!typeEquals(node->kids[1]->type, arrayStringType_ptr) && !typeEquals(node->kids[1]->type, stringType_ptr))
        {
            typeError("Array type must match element type - String", node);
        }
        break;
    case BOOL_TYPE:
        if (!typeEquals(node->kids[1]->type, arrayBooleanType_ptr))
        {
            typeError("Array type must match element type - Boolean", node);
        }
        break;
    case ARRAY_TYPE:
    case UNIT_TYPE:
        typeError("Cannot have the first argument of the in operator be of type Array or Unit", node);
        break;
    default:
        typeError("", node);
        break;
    }
    node->type = alcType(BOOL_TYPE); // type.c
}

/**
 * @brief Assigns types to add expressions
 *
 * @param node
 */
void addExpression(struct tree *node)
{
    switch (node->kids[0]->type->basicType)
    {
    case INT_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            node->type = alcType(INT_TYPE); // type.c
            break;
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); // type.c
            break;
        default:
            typeError("Can only add Int or Double to type Int", node);
            break;
        }
    case DOUBLE_TYPE:
        if (typeEquals(node->kids[1]->type, integerType_ptr) || typeEquals(node->kids[1]->type, doubleType_ptr))
        {
            node->type = alcType(DOUBLE_TYPE); // type.c
        }
        else
        {
            typeError("Can only add Int or Double to type Double", node);
        }
        break;
    case CHAR_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case CHAR_TYPE:
            node->type = alcType(CHAR_TYPE); // type.c
            break;
        case STRING_TYPE:
            node->type = alcType(STRING_TYPE); // type.c
            break;
        default:
            typeError("Can only add Char or String to type Char", node);
            break;
        }
        break;
    case STRING_TYPE:
        node->type = alcType(STRING_TYPE); // type.c
        break;
    case BOOL_TYPE:
        typeError("Cannot add type Boolean", node);
        break;
    case ARRAY_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            if (typeEquals(node->kids[0]->type, arrayIntegerType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only add type Int to Array<Int>", node);
            }
            break;
        case DOUBLE_TYPE:
            if (typeEquals(node->kids[0]->type, arrayDoubleType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only add type Double to Array<Double>", node);
            }
            break;
        case CHAR_TYPE:
            if (typeEquals(node->kids[0]->type, arrayCharType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only add type Char to Array<Char>", node);
            }
            break;
        case STRING_TYPE:
            if (typeEquals(node->kids[0]->type, arrayStringType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only add type String to Array<String>", node);
            }
            break;
        case BOOL_TYPE:
            if (typeEquals(node->kids[0]->type, arrayBooleanType_ptr))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only add type Boolean to Array<Boolean>", node);
            }
            break;
        case ARRAY_TYPE:
            if (typeEquals(node->kids[0]->type, node->kids[1]->type))
            {
                node->type = copyType(node->kids[0]->type); //typeHelpers.c
            }
            else
            {
                typeError("Can only add arrays that have the same subtype", node);
            }
            break;
        case UNIT_TYPE:
            typeError("Cannot add with Unit type", node);
            break;
        default:
            typeError("", node);
            break;
        }
        break;
    case UNIT_TYPE:
        typeError("Cannot add with Unit type", node);
        break;
    default:
        typeError("", node);
        break;
    }
}

/**
 * @brief Assigns types to sub expressions
 *
 * @param node
 */
void subExpression(struct tree *node)
{
    switch (node->kids[0]->type->basicType)
    {
    case INT_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            node->type = alcType(INT_TYPE); // type.c
            break;
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); // type.c
            break;
        default:
            typeError("Can only subtract Int or Double from type Int", node);
            break;
        }
        break;
    case DOUBLE_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); // type.c
            break;
        default:
            typeError("Can only subtract Int or Double from type Double", node);
            break;
        }
        break;
    case CHAR_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            node->type = alcType(CHAR_TYPE); // type.c
            break;
        case CHAR_TYPE:
            node->type = alcType(INT_TYPE); // type.c
            break;
        default:
            typeError("Can only subtract Int or Char from type Char", node);
            break;
        }
        break;
    default:
        typeError("Subtraction can only be performed on types Int, Double, and Char", node);
        break;
    }
}

/**
 * @brief Assigns types to multaplicative expressions
 *
 * @param node
 */
void multaplicativeExpression(struct tree *node)
{
    switch (node->kids[0]->type->basicType)
    {
    case INT_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
            node->type = alcType(INT_TYPE); // type.c
            break;
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); // type.c
            break;
        default:
            typeError("Multiplication, division, and modulo can only have operators of type Int and Double", node);
            break;
        }
        break;
    case DOUBLE_TYPE:
        switch (node->kids[1]->type->basicType)
        {
        case INT_TYPE:
        case DOUBLE_TYPE:
            node->type = alcType(DOUBLE_TYPE); // type.c
            break;
        default:
            typeError("Multiplication, division, and modulo can only have operators of type Int and Double", node);
            break;
        }
        break;
    default:
        typeError("Multiplication, division, and modulo can only have operators of type Int and Double", node);
        break;
    }
}

/**
 * @brief Type checks returns
 *
 * @param node
 */
void returnCheck(struct tree *node, struct typeInfo *type)
{
    while(node->parent != NULL)
    {   
        switch (node->prodrule)
        {
        case funcDecAll:
        case funcDecParamBody:
        case funcDecTypeBody:
        case funcDecBody:
            if(!typeEquals(node->kids[1]->type->u.func.returnType, type))
            {
                typeError("Return type does not match", node);
            }
            return;
        default:
            break;
        }
        node = node->parent;
    }
    typeError("Non-function blocks cannot return", node);
}

/**
 * @brief Type checks array declarations
 *
 * @param node
 */
void arrayDeclaration(struct tree *ident, struct tree *exprList)
{
    if(!typeEquals(ident->type, arrayAnyType_ptr))
    {
        typeError("Cannot assign Array to non-Array variable", ident);
    }

    if(exprList->prodrule != expressionList)
    {
        if(!typeEquals(ident->type->u.array.elemType, exprList->type))
        {
            typeError("Array elements must match array type", ident);
        }
    }
    else
    {
        while(1)
        {
            if(!typeEquals(ident->type->u.array.elemType, exprList->kids[0]->type))
            {
                typeError("Array elements must match array type", ident);
            }
            exprList = exprList->kids[1];
            if(exprList->prodrule != expressionList)
            {
                if(!typeEquals(ident->type->u.array.elemType, exprList->type))
                {
                    typeError("Array elements must match array type", ident);
                }
                break;
            }
        }
    }
}

/**
 * @brief Prints type error message and sets symError to 1
 *
 * @param node
 */
void typeError(char *message, struct tree *node)
{
    while(node->nkids != 0)
    {
        node = node->kids[0];
    }

    fprintf(stderr, "Line %d, Type Error: %s\n", node->leaf->lineno, message);
    symError = 1;
}