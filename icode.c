#include <stdio.h>
#include "icode.h"
#include "tree.h"
#include "symNonTerminals.h"
#include "k0gram.tab.h"
#include "tac.h"
#include "lex.h"

void localAddr(struct tree *node)
{
    switch (node->prodrule)
    {
    case varDec:
    case varDecQuests:
        //give local addresses
        break;

    case funcDecAll:
    case funcDecParamType:
    case funcDecParamBody:
    case funcDecTypeBody:
    case funcDecType:
    case funcDecBody:
    case funcValParams:
    case funcValParamList:
    case funcValParamAssign:
        //whatever we need to do with this idk
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
        node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode), genInstr(O_ASN, node->addr, node->kids[1]->addr, NULL));
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
        node->addr = genConst(node->leaf->ival);
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
        //need to give these nodes addresses so thier parents can use them
        break;

    //when hit control flow, return without concating child code
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
        return;

    default:
        if(node->nkids == 0)
        {
            break;
        }
        node->icode = copyInstrList(node->kids[0]);
        for(int i = 1; i < node->nkids; i++)
        {
            node->icode = appendInstrList(node->icode, node->kids[i]);
        }
        break;
    }
}

//will need recursion of some sort
//for now just pulling out rules that have control flow stuff
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