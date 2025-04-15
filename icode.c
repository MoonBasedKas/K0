#include <stdio.h>
#include "icode.h"
#include "tree.h"
#include "symNonTerminals.h"
#include "k0gram.tab.h"
#include "tac.h"

void buildICode(struct tree *node)
{
    for(int i = 0; i < node->nkids; i++)
    {
        buildICode(node->kids[i]);
    }

    switch (node->prodrule)
    {
        case varDec:
        case varDecQuests:
            //give local addresses
            break;

        case propDecAssign:
        case propDecReceiverAssign:
        case propDecTypeParamsAssign:
        case propDecAll:
            //these are assignemnts
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

        case quests:
        case funcBody:

        case assignment:
        case assignAdd:
        case assignSub:
        case arrayAssignSub:
        case arrayAssignAdd:
        case arrayAssignment:
            node->addr = node->kids[0]->addr;
            node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode), genInstr(O_ASN, node->addr, node->kids[1]->addr, NULL));
            break;
            
        case forStmntWithVars:
        case forStmnt:

        case whileStmntCtrlBody:
        case whileStmnt:
        case doWhileStmnt:

        case disj:
        case conj:
        case equal:
        case notEqual:
        case eqeqeq:
        case notEqeqeq:
        case less:
        case greater:
        case lessEqual:
        case greaterEqual:

        case in:

        case elvis:
        case infixFunction:
        case range:
        case rangeUntil:

        case add:
        case sub:
        case mult:
        case div_k:
        case mod:

        case prefix:
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
        case expressionList:
        case parenthesizedExpression:

        case emptyIf:
        case if_k:
        case ifElse:
        case ifElseIf:
        
        //took out whens

        case controlStmnt:
        case returnVal:

        case arraySizeInt:
        case arraySizeIdent:
        case arrayValues:
        case arrayDec:
        case arrayDecValueless:
        case arrayDecEqual:
        case arrayDecEqualValueless:
        case arrayIndex:
        
        case arrayType:
        case arrayTypeQuests:

        case INTEGER_LITERAL:
        case HEX_LITERAL:
        case CHARACTER_LITERAL:
        case REAL_LITERAL:
        case TRUE:
        case FALSE:
        case NULL_K:
        case LINE_STRING:
        case MULTILINE_STRING:
        case IDENTIFIER:

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