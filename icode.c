#include <stdio.h>
#include "icode.h"
#include "symNonTerminals.h"
#include "k0gram.tab.h"
#include "lex.h"
#include "symTab.h"
#include "typeHelpers.h"
#include "tac.h"

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
    struct symEntry *entry = NULL;
    switch (node->prodrule)
    {
    // handles both variable declarations and parameters
    case varDec:
    case varDecQuests:
        entry = contains(node->table, node->kids[0]->leaf->text); // symTab.c
        entry->addr = genLocal(typeSize(entry->type), entry->scope);               // tac.c typeHelpers.c

        break;

    case funcDecAll:
    case funcDecParamType:
    case funcDecParamBody:
    case funcDecTypeBody:
    case funcDecType:
    case funcDecBody:
        // do we do something with functions now or later?
        // i think we just need labels for functions
        // yes do the label now because then when i do addr in basicBlocks it will be done??
        // no because in basic blocks i can have the same code and it will just assign it to null again and
        // then deal with all the labels at once??
        struct addr *addr = genLabel(); // tac.c
        node->addr = addr;
        entry = contains(node->table, node->kids[1]->leaf->text); // symTab.c
        if (entry)
            entry->addr = addr;
        break;

    default:
        break;
    }
}

void basicBlocks(struct tree *node)
{
    for (int i = 0; i < node->nkids; i++)
    {
        basicBlocks(node->kids[i]);
    }

    if (node->icodeDone == 0 && node->parent != NULL)
    {
        node->parent->icodeDone = 0;
        return;
    }

    switch (node->prodrule)
    {
    case propDecAssign:
        node->addr = node->kids[1]->kids[0]->addr;
        node->icode = concatInstrList(node->kids[2]->icode, genInstr(O_ASN, node->addr, node->kids[2]->addr, NULL)); // tac.c
        break;
    case propDecReceiverAssign:
    case propDecTypeParamsAssign:
        node->addr = node->kids[2]->kids[0]->addr;
        node->icode = concatInstrList(node->kids[3]->icode, genInstr(O_ASN, node->addr, node->kids[3]->addr, NULL)); // tac.c
        break;
    case propDecAll:
        node->addr = node->kids[3]->kids[0]->addr;
        node->icode = concatInstrList(node->kids[4]->icode, genInstr(O_ASN, node->addr, node->kids[4]->addr, NULL)); // tac.c
        break;
    case funcValParamAssign:
        node->addr = node->kids[0]->kids[0]->addr;
        node->icode = concatInstrList(node->kids[1]->icode, genInstr(O_ASN, node->addr, node->kids[1]->addr, NULL)); // tac.c
        break;

    // need to do speceal something for assignment ifs???
    // cause when this happens if hasn't been evaluated yet
    // does that cause problems here or can i leave this and then just deal later???
    // maybe this is fine here cause it will just append NULL to the end of the list and then
    // when we get to if latter we can fix it????
    // no that won't work cause we copy this later into the upper code so it can't be fixed latter
    // fuck
    // maybe i just check if child is an if, and if so we break out and deal latter and otherwise handle now
    case assignment:
    case assignAdd:
    case assignSub:
    case arrayAssignSub:
    case arrayAssignAdd:
    case arrayAssignment:
        switch (node->kids[1]->prodrule)
        {
        case emptyIf:
        case if_k:
        case ifElse:
        case ifElseIf:
            break;

        default:
            node->addr = node->kids[0]->addr;
            node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode), // tac.c
                                          genInstr(O_ASN, node->addr, node->kids[1]->addr, NULL));     // tac.c
            break;
        }
        break;

    // are we doing short circuting??
    // YES this needs to move then
    // TOD)
    case disj:
        node->addr = genLocal(typeSize(node->type), node->table);
        node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode),           // tac.c
                                      genInstr(O_OR, node->addr, node->kids[0]->addr, node->kids[1]->addr)); // tac.c
        break;
    case conj:
        node->addr = genLocal(typeSize(node->type), node->table);
        node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode),            // tac.c
                                      genInstr(O_AND, node->addr, node->kids[0]->addr, node->kids[1]->addr)); // tac.c
        break;

    // NOT DONE!!!!
    // same as equals()
    case equal:
        switch (node->kids[0]->type->basicType)
        {
        case INT_TYPE:
        case DOUBLE_TYPE:
        case CHAR_TYPE:
        case BOOL_TYPE:

        // need to figure these out
        case STRING_TYPE:
        case ARRAY_TYPE:
            break;

        default:
            break;
        }
    case notEqual:
    // checks equality of pointers for structures same as == for primatives
    case eqeqeq:
    case notEqeqeq:

        break;

    case less:
    case greater:
    case lessEqual:
    case greaterEqual:
        // should be straight forward
        break;

    case in:
        // NOT PART OF FOR LOOP
        // BOOLEAN EXPRESSION
        break;

    case infixFunction:
        // figure out what to do here
        // might not need for k0, ask jeffery
        // might need to add infix funcs to type check
        break;

    case range:
    case rangeUntil:
        // does this creat an array of ints???
        // what it do tho?
        break;

    case add:
    case sub:
    case mult:
    case div_k:
    case mod:
        // should be fairly straight forward
        break;

    case prefix:
        // definitly need this one
        // will need sub switch
        break;

    case arrayAccess:
    case arrayIndex:
    case postfixArrayAccess:
        // calculate the correct memory address
        // need to figure out if doing pointer to arrays or not first
        break;

    case postfixIncr:
    case postfixDecr:
        // should be pretty easy
        break;

    case arraySizeInt:
    case arraySizeIdent:
        // might need, not sure
        break;

    case arrayDec:
        // def need since assignemnt
        break;

    case arrayDecValueless:
        // need cause we need to make an array of the correct size, alloc mem
        break;

    case arrayDecEqual:
    case arrayDecEqualValueless:
        // def need since have assignment
        break;

    case INTEGER_LITERAL:
    case HEX_LITERAL:
        node->addr = genConst(node->leaf->ival); // tac.c
        break;

    case CHARACTER_LITERAL:
    case REAL_LITERAL:
    case TRUE:
    case FALSE:
    case NULL_K:
    case LINE_STRING:
    case MULTILINE_STRING:
        // need to give these nodes addresses so thier parents can use them
        // also need to do whatever alloc needed for strings
        break;

    case IDENTIFIER:
        struct symTab *scope = node->table;
        struct symEntry *entry;

        while (scope != NULL)
        {
            entry = contains(scope, node->leaf->text); // symTab.c
            if (entry != NULL)
            {
                break;
            }
            scope = scope->parent;
        }

        node->addr = entry->addr;

        break;

    // when hit control flow, break without concating child code
    // these need to be null for later
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
        node->icodeDone = 0;
        node->parent->icodeDone = 0;
        break;
    default:
        if (node->nkids == 0)
        {
            break;
        }
        node->icode = copyInstrList(node->kids[0]->icode); // tac.c
        for (int i = 1; i < node->nkids; i++)
        {
            node->icode = appendInstrList(node->icode, node->kids[i]->icode); // tac.c
        }
        break;
    }
}

void assignFirst(struct tree *node)
{
    for (int i = 0; i < node->nkids; i++)
    {
        assignFirst(node->kids[i]);
    }

    if (node->icode != NULL && node->icodeDone == 1 && node->parent->icodeDone == 0) // && or ||?
    {
        node->first = genLabel(); // tac.c
    }

    // do i need more than this??
    // are there any other times where you would need a first label
    // in assignFollow and assignOnTrueFasle print out message saying that something needs to be added here if something is NULL
}

void assignFollow(struct tree *node)
{
    int last = 0;
    // currently just copied all of the prodrules that have control flow
    switch (node->prodrule)
    {
    case forStmntWithVars:
    case forStmnt:
        last = node->nkids - 1;
        node->kids[last]->follow = node->first;
        break;

    case whileStmntCtrlBody:
        last = node->nkids - 1;
        node->kids[last]->follow = node->first;
        break;
    case whileStmnt:
        break;
    case doWhileStmnt:
        int body = 1;
        int test = node->nkids - 1;
        node->kids[body]->follow = node->kids[test]->first;
        break;

    // need to do speceal something for assignment ifs???
    case emptyIf:
    case if_k:
        node->kids[2]->follow = node->follow;
        break;
    case ifElse:
        node->kids[2]->follow = node->kids[4]->first;
        node->kids[4]->follow = node->follow;
        break;
    case ifElseIf:
        node->kids[2]->follow = node->kids[4]->first;
        node->kids[4]->follow = node->follow;
        break;

    case elvis:
        // TODO
        // i think this goes here
        // idk tho
        //  go left: null -> right side first block, fall down
        node->kids[0]->follow = node->kids[1]->first;
        node->kids[1]->follow = node->follow;
        break;
    case postfixExpr:
    case postfixNoExpr:
        // TODO
        // function call
        // figrue that shit out
        //  Are these control flow?
    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
        // TODO
        // figure all this shit out
        // at this point might not need the intital part???

    case funcBody:
        // TODO
        // treat like return value??
        // ASSIGNMENT expression
        break;

    case returnVal:
    case RETURN:
        // TODO
        // definlty need this shit
        node->follow = NULL;
        if (node->nkids > 0)
            node->kids[0]->follow = NULL;
        break;

    default:
        break;
    }

    for (int i = 0; i < node->nkids; i++)
    {
        if (node->kids[i]->follow == NULL)
            node->kids[i]->follow = node->follow;
        assignFollow(node->kids[i]);
    }
}

void assignOnTrueFalse(struct tree *node)
{
    for (int i = 0; i < node->nkids; i++)
    {
        assignOnTrueFalse(node->kids[i]);
    }

    switch (node->prodrule)
    {
    case forStmntWithVars:
    case forStmnt:
        node->onTrue = node->kids[3]->first;
        if (node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[3]);
        }
        node->onFalse = node->follow;
        if (node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;

    case whileStmntCtrlBody:
        node->onTrue = node->kids[2]->first;
        if (node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->follow;
        if (node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;
    case whileStmnt:
        // how does this one work??? IDK
        // TODO
        break;
    case doWhileStmnt:
        node->onTrue = node->kids[1]->first;
        if (node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[1]);
        }
        node->onFalse = node->follow;
        if (node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;

    // need to do speceal something for assignment ifs??? since they can be assigned??
    case emptyIf:
    case if_k:
        node->onTrue = node->kids[2]->first;
        if (node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->follow;
        if (node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node);
        }
        break;

    case ifElse:
    case ifElseIf:
        node->onTrue = node->kids[2]->first;
        if (node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->kids[4]->first;
        if (node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node->kids[4]);
        }
        break;

    case elvis:
        // true is when the value isn't null, might change latter
        node->onTrue = node->kids[0]->first;
        if (node->onTrue == NULL)
        {
            debugICode("Missing something in assignFirst", node->kids[2]);
        }
        // false is the alternate value, might change latter
        node->onFalse = node->kids[1]->first;
        if (node->onFalse == NULL)
        {
            debugICode("Missing something in assignFollow", node->kids[4]);
        }
        break;

    default:
        break;
    }
}

// will need recursion of some sort
// for now just pulling out rules that have control flow stuff
// actual icode using previouly gen labels
void control(struct tree *node)
{
    switch (node->prodrule)
    {
    case forStmntWithVars:
    case forStmnt:
        // TODO
        // how do this??
        // for range, if > min and < max and store val
        // idk what to do for array
        // walk down full array and store in register OR of == this element
        // could do it with goto if found but that seems not worth unless huge array

    case whileStmntCtrlBody:
    case whileStmnt:
    case doWhileStmnt:
        // TODO

    // need to do speceal something for assignment ifs???
    // YES
    // need to check if parent is assignment and if so add code for it
    case emptyIf:
    case if_k:
    case ifElse:
    case ifElseIf:
        // TODO

    case elvis:
        // TOOD
        // i think this goes here
        // idk tho

    case postfixExpr:
    case postfixNoExpr:
        // TODO
        // function call
        // figrue that shit out

    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
        // TODO
        // figure all this shit out
        // at this point might not need the intital part???

    case funcBody:
        // TODO
        // treat like return value??
        // ASSIGNMENT expression
        break;

    case returnVal:
    case RETURN:
        // TODO
        // definlty need this shit
        break;

    default:
        break;
    }
}

void debugICode(char *string, struct tree *node)
{
    while (node->nkids != 0)
    {
        node = node->kids[0];
    }

    printf("Name: %s Line %d, Debug Message: %s\n", node->leaf->text, node->leaf->lineno, string);
}
