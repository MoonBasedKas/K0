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
        case propDecEmpty:
        case propDecTypeless:
        case propDecAssign:
        case propDecReceiver:
        case propDecReceiverAssign:
        case propDecTypeParams:
        case propDecTypeParamsAssign:
        case propDecTypeParamsReceiver:
        case propDecAll:

        case constantVal:
        case typeParameters:

        case funcDecAll:
        case funcDecParamType:
        case funcDecParamBody:
        case funcDecTypeBody:
        case funcDecType:
        case funcDecBody:
        case funcValParams:
        case funcValParamList:
        case funcValParamAssign:

        case userType:
        case simpleUserType:
        case typeArgumentsList:

        case functionTypeDot:
        case functionType:
        case functionTypeParameters:
        case functionTypeParamsEmpty:
        case functionTypeParamList:
        case parenTypeQuests:
        case parenType:
        case quests:
        case funcBody:

        case assignment:
        case assignAdd:
        case assignSub:
        case arrayAssignSub:
        case arrayAssignAdd:
        case arrayAssignment:
            node->addr = node->kids[0]->addr;
            node->icode = append(concat(node->kids[0]->icode, node->kids[1]->icode), gen(O_ASN, node->addr, node->kids[1]->addr, NULL));
            break;
            
        case forStmntWithVars:
        case forStmnt:

        case whileStmntCtrlBody:
        case whileStmnt:
        case doWhileStmnt:

        case varDec:
        case varDecQuests:
        case multiVarDec:
        case varDecList:

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
        node->icode = copylist(node->kids[0]);
        for(int i = 1; i < node->nkids; i++)
        {
            node->icode = append(node->icode, copylist(node->kids[i])); //does the 2nd need to be coppied?
        }
        break;
    }
}