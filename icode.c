#include <stdio.h>
#include "icode.h"
#include "tree.h"
#include "symNonTerminals.h"
#include "k0gram.tab.h"
#include "tac.h"
#include "lex.h"
#include "symTab.h"
#include "typeHelpers.h"

void buildICode(struct tree *node)
{
    localAddr(node);
    basicBlocks(node);
    assignFirst(node);
    assignFollow(node);
    assignOnTrueFalse(node);
    control(node);
}

void localAddr(struct tree *node)
{
    switch (node->prodrule)
    {
    //handles both variable declarations and parameters
    case varDec:
    case varDecQuests:
        struct symEntry *entry = contains(node->table, node->kids[0]->leaf->text); //symTab.c
        entry->addr = genLocal(typeSize(entry->type), entry->scope); //tac.c typeHelpers.c

        break;

    case funcDecAll:
    case funcDecParamType:
    case funcDecParamBody:
    case funcDecTypeBody:
    case funcDecType:
    case funcDecBody:
        //do we do something with functions now or later?
        //i think we just need labels for functions
        //yes do the label now because then when i do addr in basicBlocks it will be done??
        //no because in basic blocks i can have the same code and it will just assign it to null again and 
        //then deal with all the labels at once??
        break;

    default:
        break;
    }
}

void basicBlocks(struct tree *node)
{
    for(int i = 0; i < node->nkids; i++)
    {
        basicBlocks(node->kids[i]);
    }

    switch (node->prodrule)
    {
    case propDecAssign:
    case propDecReceiverAssign:
    case propDecTypeParamsAssign:
    case propDecAll:
    case funcValParamAssign:
        //these are assignemnts
        break;

    //need to do speceal something for assignment ifs???
    //cause when this happens if hasn't been evaluated yet
    //does that cause problems here or can i leave this and then just deal later???
    case assignment:
    case assignAdd:
    case assignSub:
    case arrayAssignSub:
    case arrayAssignAdd:
    case arrayAssignment:
        node->addr = node->kids[0]->addr;
        node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode), 
                            genInstr(O_ASN, node->addr, node->kids[1]->addr, NULL)); //tac.c
        break;

    case disj:
    case conj:
    case equal:
    case notEqual:
    case eqeqeq:
    case notEqeqeq:
        //are we doing short circuting??
        break;

    case less:
    case greater:
    case lessEqual:
    case greaterEqual:
        //should be straight forward
        break;

    case in:
        //how do this??
        //does this need its own code or will it just be delt with in for loop
        //yes definitly
        //for range, if > min and < max and store val
        //idk what to do for array
        //walk down full array and store in register OR of == this element
        //could do it with goto if found but that seems not worth unless huge array
        break;

    case infixFunction:
        //figure out what to do here
        //might not need for k0, ask jeffery
        //might need to add infix funcs to type check
        break;

    case range:
    case rangeUntil:
        //does this creat an array of ints???
        //what it do tho?
        break;

    case add:
    case sub:
    case mult:
    case div_k:
    case mod:
        //should be fairly straight forward
        break;

    case prefix:
        //definitly need this one
        //will need sub switch
        break;

    case arrayAccess:
    case arrayIndex:
    case postfixArrayAccess:
        //calculate the correct memory address
        //need to figure out if doing pointer to arrays or not first
        break;

    case postfixIncr:
    case postfixDecr:
        //should be pretty easy
        break;

    case arraySizeInt:
    case arraySizeIdent:
        //might need, not sure
        break;

    case arrayDec:
        //def need since assignemnt
        break;

    case arrayDecValueless:
        //need cause we need to make an array of the correct size, alloc mem
        break;

    case arrayDecEqual:
    case arrayDecEqualValueless:
        //def need since have assignment
        break;

    case INTEGER_LITERAL:
    case HEX_LITERAL:
        node->addr = genConst(node->leaf->ival); //tac.c
        break;
        
    case CHARACTER_LITERAL:
    case REAL_LITERAL:
    case TRUE:
    case FALSE:
    case NULL_K:
    case LINE_STRING:
    case MULTILINE_STRING:
        //need to give these nodes addresses so thier parents can use them
        //also need to do whatever alloc needed for strings
        break;

    case IDENTIFIER:
        struct symTab *scope = node->table;
        struct symEntry *entry;

        while(scope != NULL)
        {
            entry = contains(scope, node->leaf->text); //symTab.c
            if(entry != NULL)
            {
                break;
            }
            scope = scope->parent;
        }

        node->addr = entry->addr;

        break;

    //when hit control flow, break without concating child code
    //these need to be null for later
    case forStmntWithVars:
    case forStmnt:
    case whileStmntCtrlBody:
    case whileStmnt:
    case doWhileStmnt:
    case emptyIf:
    case if_k:
    case ifElse:
    case ifElseIf:
    case elvis:
    case postfixExpr:
    case postfixNoExpr:
    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
    case funcBody:
    case returnVal:
    case RETURN:
        break;;

    default:
        if(node->nkids == 0)
        {
            break;
        }
        node->icode = copyInstrList(node->kids[0]); //tac.c
        for(int i = 1; i < node->nkids; i++)
        {
            node->icode = appendInstrList(node->icode, node->kids[i]); //tac.c
        }
        break;
    }
}

void assignFirst(struct tree *node)
{
    for(int i = 0; i < node->nkids; i++)
    {
        assignFirst(node->kids[i]);
    }

    //if this node has icode and the parent doesn't then the parent has control flow stuff
    //so its first child needs a first label
    //might also give labels to other children if they have the same prodrule as first child
    if(node->icode != NULL && node->parent->icode == NULL)
    {
        if(node->prodrule == node->parent->kids[0]->prodrule)
        {
            node->first = genLabel(); //tac.c
        }
    }

    //do i need more than this??
    //are there any other times where you would need a first label
    //in assignFollow and assignOnTrueFasle print out message saying that something needs to be added here if something is NULL
}

void assignFollow(struct tree *node)
{
    //currently just copied all of the prodrules that have control flow
    switch (node->prodrule)
    {
    case forStmntWithVars:
    case forStmnt:

    case whileStmntCtrlBody:
    case whileStmnt:
    case doWhileStmnt:

    //need to do speceal something for assignment ifs???
    case emptyIf:
    case if_k:
    case ifElse:
    case ifElseIf:

    //took out whens

    case elvis:
    //i think this goes here
    //idk tho

    case postfixExpr:
    case postfixNoExpr:
        //function call
        //figrue that shit out

    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
        //figure all this shit out
        //at this point might not need the intital part???

    case funcBody:
        //treat like return value??
        //ASSIGNMENT expression
        break;

    case returnVal:
    case RETURN:
        //definlty need this shit
        break;

    default:
        break;
    }

    for(int i = 0; i < node->kids; i++)
    {
        assignFollow(node->kids[i]);
    }
}

void assignOnTrueFalse(struct tree *node)
{
    for(int i = 0; i < node->kids; i++)
    {
        assignOnTrueFalse(node->kids[i]);
    }

    switch (node->prodrule)
    {
    case forStmntWithVars:
    case forStmnt:
        node->onTrue = node->kids[3]->first;
        if(node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[3]);
        }
        node->onFalse = node->follow;
        if(node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;

    case whileStmntCtrlBody:
        node->onTrue = node->kids[2]->first;
        if(node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->follow;
        if(node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;
    case whileStmnt:
        //how does this one work??? IDK
        break;
    case doWhileStmnt:
        node->onTrue = node->kids[1]->first;
        if(node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[1]);
        }
        node->onFalse = node->follow;
        if(node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;

    //need to do speceal something for assignment ifs??? since they can be assigned??
    case emptyIf:
    case if_k:
        node->onTrue = node->kids[2]->first;
        if(node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->follow;
        if(node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;

    case ifElse:
    case ifElseIf:
        node->onTrue = node->kids[2]->first;
        if(node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->kids[4]->first;
        if(node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node->kids[4]);
        }
        break;

    case elvis:
        //true is when the value isn't null, might change latter
        node->onTrue = node->kids[0]->first;
        if(node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        //false is the alternate value, might change latter
        node->onFalse = node->kids[1]->first;
        if(node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node->kids[4]);
        }
        break;

    default:
        break;
    }
}

//will need recursion of some sort
//for now just pulling out rules that have control flow stuff
//actual icode using previouly gen labels
void control(struct tree *node)
{
    switch (node->prodrule)
    {
    case forStmntWithVars:
    case forStmnt:

    case whileStmntCtrlBody:
    case whileStmnt:
    case doWhileStmnt:

    //need to do speceal something for assignment ifs???
    case emptyIf:
    case if_k:
    case ifElse:
    case ifElseIf:

    //took out whens

    case elvis:
    //i think this goes here
    //idk tho

    case postfixExpr:
    case postfixNoExpr:
        //function call
        //figrue that shit out

    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
        //figure all this shit out
        //at this point might not need the intital part???

    case funcBody:
        //treat like return value??
        //ASSIGNMENT expression
        break;

    case returnVal:
    case RETURN:
        //definlty need this shit
        break;

    default:
        break;
    }
}

void debugICode(char *string, struct tree *node)
{
    while(node->nkids != 0)
    {
        node = node->kids[0];
    }

    printf("Name: %s Line %d, Debug Message: %s\n", node->leaf->text, node->leaf->lineno, string);
}