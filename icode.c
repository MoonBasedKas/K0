#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icode.h"
#include "symNonTerminals.h"
#include "k0gram.tab.h"
#include "lex.h"
#include "symTab.h"
#include "typeHelpers.h"
#include "tac.h"
#include "errorHandling.h"

/**
 * @brief Reset the icodeDone flag for a tree node and all its children.
 *
 * This function recursively resets the icodeDone flag for the given node and
 * all its children. It ensures that the flag is set to 1 for all nodes in the
 * tree.
 */
void resetICodeDone(struct tree *node)
{
    node->icodeDone = 1;
    for (int i = 0; i < node->nkids; i++)
        resetICodeDone(node->kids[i]);
}

void buildICode(struct tree *node)
{

    struct instr *all = NULL;
    localAddr(node);
    resetICodeDone(node);
    // assignFirst(node);
    // assignFollow(node);
    // assignOnTrueFalse(node);

    basicBlocks(node);

    theGreatICodeMerge(node);

    if (node->prodrule == program)
    {
        // all = NULL;
        for (int i = 0; i < node->nkids; i++)
        {
            if (node->kids[i]->icode)
                all = appendInstrList(all, node->kids[i]->icode);
        }
        node->icode = all;
    }
}

void theGreatICodeMerge(struct tree *node)
{

    for (int i = 1; i < node->nkids; i++)
        node->icode = appendInstrList(
            node->icode,
            node->kids[i]->icode);
}

/**
 * @brief Prints the icode list.
 *
 * @param i
 */
void printIcode(struct instr *i)
{
    struct instr *j = i;
    while (j != NULL)
    {
        printf("%p-", j);
        printf("%d\t", j->opcode);
        j = j->next;
    }
    printf("\n");
}

void localAddr(struct tree *node)
{
    if (!node->table)
    {
        fprintf(stderr, "localAddr: called with null scope\n");
        exit(1);
    }
    struct symEntry *entry = NULL;
    switch (node->prodrule)
    {
    // handles both variable declarations and parameters
    case varDec:
    case varDecQuests:
        entry = contains(node->table, node->kids[0]->leaf->text); // symTab.c
        if (entry)
        {
            struct symTab *scope = node->table;
            while (scope->parent)
            {
                scope = scope->parent;
            }
            entry->addr = genLocal(
                typeSize(entry->type),
                scope); // tac.c typeHelpers.c
        }
        break;

    case funcDecAll:
    case funcDecParamType:
    case funcDecParamBody:
    case funcDecTypeBody:
    case funcDecType:
    case funcDecBody:
    {
        struct addr *lbl = genLabel();
        node->addr = lbl;

        entry = contains(node->table, node->kids[1]->leaf->text);
        if (entry)
            entry->addr = lbl;
        break;
    }
    case propDecAssign:
    {
        entry = contains(
            node->table,
            node->kids[1]->kids[0]->leaf->text);
        if (entry)
            entry->addr = genLocal(
                typeSize(entry->type),
                node->table);
        break;
    }
    case propDecReceiverAssign:
    {
        entry = contains(
            node->table,
            node->kids[2]->kids[0]->leaf->text);
        if (entry)
            entry->addr = genLocal(
                typeSize(entry->type),
                node->table);
    }
    case propDecTypeParamsAssign:
    {
        entry = contains(
            node->table,
            node->kids[2]->kids[0]->leaf->text);
        if (entry)
            entry->addr = genLocal(typeSize(entry->type), entry->scope);
        break;
    }
    case propDecAll:
    {
        entry = contains(node->table, node->kids[3]->kids[0]->leaf->text);
        if (entry)
            entry->addr = genLocal(typeSize(entry->type), entry->scope);
        break;
    }
    // WOAW arrays (based, based, based, based, based, based, based, ...)
    case arrayDec:
    case arrayDecValueless:
    entry = contains(node->table, node->kids[1]->kids[0]->leaf->text);
    if (entry)
        entry->addr = genLocal(8 * node->kids[2]->kids[1]->leaf->ival, node->table);
    break;

        break;

    case arrayDecEqual:
    case arrayDecEqualValueless:
    entry = contains(node->table, node->kids[1]->kids[0]->leaf->text);
    if (entry)
        entry->addr = genLocal(node->kids[4]->kids[1]->leaf->ival, node->table);
    break;
    default:
        break;
    }
    for (int i = 0; i < node->nkids; i++)
    {
        localAddr(node->kids[i]);
    }
}

/**
 * @brief Creates basic blocks for iCode
 *
 * @param node
 */
void basicBlocks(struct tree *node)
{
    struct addr *thenLabel = NULL;
    struct addr *elseLabel = NULL;
    struct addr *followLabel = NULL;
    struct addr *procName = NULL;
    struct addr *codeName = NULL;
    struct instr *code = NULL;
    struct instr *entryLabel = NULL;
    int paramCount = 0;
    int op = 0;
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
    case funcDecAll:
        // grab parameter count from your typeInfo
        
        if (node->type && node->type->basicType == FUNCTION_TYPE)
        {
            paramCount = node->type->u.func.numParams;
        }

        // build the PROC header using the real paramCount and varSize
        procName = malloc(sizeof *procName);
        procName->region = R_NAME;
        procName->u.name = node->kids[1]->leaf->text;

        code = genInstr(
            D_PROC,
            procName,
            genConst(paramCount),          // <— real # of params
            genConst(node->table->varSize) // <— real frame size
        );

        // now emit the “.code” marker
        codeName = malloc(sizeof *codeName);
        codeName->region = R_NAME;
        codeName->u.name = strdup(".code");
        code = appendInstrList(
            code,
            genInstr(D_LABEL, codeName, NULL, NULL));

        // entry label for the function
        entryLabel = genInstr(
            D_LABEL,
            procName,
            NULL,
            NULL);
        code = appendInstrList(code, entryLabel);

        // append the body’s IR and an implicit “return unit”
        if (node->kids[4] && node->kids[4]->icode)
        {
            code = appendInstrList(code, node->kids[4]->icode);
        }

        node->icode = code;
        node->icodeDone = 0;
        break;
    case funcDecParamType:
    case funcDecParamBody:
    case funcDecTypeBody:
        // grab parameter count from your typeInfo
        if (node->type && node->type->basicType == FUNCTION_TYPE)
        {
            paramCount = node->type->u.func.numParams;
        }

        // build the PROC header using the real paramCount and varSize
        procName = malloc(sizeof *procName);
        procName->region = R_NAME;
        procName->u.name = node->kids[1]->leaf->text;

        struct instr *code = genInstr(
            D_PROC,
            procName,
            genConst(paramCount),          // <— real # of params
            genConst(node->table->varSize) // <— real frame size
        );

        // now emit the “.code” marker
        codeName = malloc(sizeof *codeName);
        codeName->region = R_NAME;
        codeName->u.name = strdup(".code");
        code = appendInstrList(
            code,
            genInstr(D_LABEL, codeName, NULL, NULL));

        // entry label for the function
        entryLabel = genInstr(
            D_LABEL,
            procName,
            NULL,
            NULL);
        code = appendInstrList(code, entryLabel);

        // append the body’s IR and an implicit “return unit”
        if (node->kids[3] && node->kids[3]->icode)
        {
            code = appendInstrList(code, node->kids[3]->icode);
        }

        node->icode = code;
        node->icodeDone = 0;
        break;
    case funcDecType:
    case funcDecBody:
    {

        // grab parameter count from your typeInfo
        if (node->type && node->type->basicType == FUNCTION_TYPE)
        {
            paramCount = node->type->u.func.numParams;
        }

        // build the PROC header using the real paramCount and varSize
        struct addr *procName = malloc(sizeof *procName);
        procName->region = R_NAME;
        procName->u.name = node->kids[1]->leaf->text;

        code = genInstr(
            D_PROC,
            procName,
            genConst(paramCount),          // <— real # of params
            genConst(node->table->varSize) // <— real frame size
        );

        // now emit the “.code” marker
        codeName = malloc(sizeof *codeName);
        codeName->region = R_NAME;
        codeName->u.name = strdup(".code");
        code = appendInstrList(
            code,
            genInstr(D_LABEL, codeName, NULL, NULL));

        // entry label for the function
        entryLabel = genInstr(
            D_LABEL,
            procName,
            NULL,
            NULL);
        code = appendInstrList(code, entryLabel);

        // append the body’s IR and an implicit “return unit”
        if (node->kids[2] && node->kids[2]->icode)
        {
            code = appendInstrList(code, node->kids[2]->icode);
        }
        code = appendInstrList(
            code,
            genInstr(O_RET, genConst(0), NULL, NULL));

        node->icode = code;
        node->icodeDone = 0;
        break;
    }
    case varDec:
    case varDecQuests:
        // kids[0] = identifier, kids[1]=type, kids[2]=initializerExpr
        if (node->nkids >= 3 && node->kids[2]->icode)
        {
            // node->addr was set in localAddr()
            node->icode = concatInstrList(
                node->kids[2]->icode,
                genInstr(O_ASN,
                         node->addr,
                         node->kids[2]->addr,
                         NULL));
        }
        break;
    case propDecAssign:
        node->addr = node->kids[1]->kids[0]->addr;
        node->icode = concatInstrList(node->kids[2]->icode,
                                      genInstr(O_ASN, node->addr,
                                               node->kids[2]->addr,
                                               NULL)); // tac.c
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
    case postfixExpr:
    {
        struct tree *fnNode = node->kids[0];
        struct tree *argTree = node->kids[1];
        struct instr *code = NULL;

        /* if it really is an expressionList, walk it;
           otherwise treat it as a single-expression list */
        if (argTree->prodrule == expressionList)
        {
            if (argTree->icode)
                code = copyInstrList(argTree->icode);
            struct tree *cur = argTree;
            while (cur->prodrule == expressionList)
            {
                struct tree *oneArg = cur->kids[0];
                code = appendInstrList(code,
                                       genInstr(O_PARM, oneArg->addr, NULL, NULL));
                if (cur->nkids == 2)
                    cur = cur->kids[1];
                else
                    break;
            }
        }
        else
        {
            /* single argument */
            if (argTree->icode)
                code = copyInstrList(argTree->icode);
            code = appendInstrList(code,
                                   genInstr(O_PARM, argTree->addr, NULL, NULL));
        }

        int nargs = countExprList(argTree);

        /* build the CALL */
        struct addr *fnName = malloc(sizeof *fnName);
        fnName->region = R_NAME;
        fnName->u.name = fnNode->leaf->text;

        struct addr *retSlot = genLocal(typeSize(node->type), node->table);
        node->addr = retSlot;

        code = appendInstrList(code,
                               genInstr(O_CALL, fnName, genConst(nargs), retSlot));

        node->icode = code;
        node->icodeDone = 0;
        break;
    }
    case postfixNoExpr:
    {
        struct tree *fnNode = node->kids[0];
        struct addr *fnName = malloc(sizeof *fnName);
        fnName->region = R_NAME;
        fnName->u.name = fnNode->leaf->text;

        struct addr *retSlot = genLocal(typeSize(node->type), node->table);
        node->addr = retSlot;

        /* zero-arg call */
        node->icode = genInstr(O_CALL, fnName, genConst(0), retSlot);
        break;
    }

    case assignment:
    case assignAdd:
    case assignSub:
    node->addr = node->kids[0]->addr;
    node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode), // tac.c
                                genInstr(O_ASN, node->addr, node->kids[1]->addr, NULL));     // tac.c
                                break;
    // WHY? This literally checks for conditions which never occur.
    switch (node->kids[1]->prodrule)
        {
        case emptyIf:
        case if_k:
        case ifElse:
        case ifElseIf:
            break;

        default:

            break;
        }
    case arrayAssignSub:
    case arrayAssignAdd:
    case arrayAssignment:
        // Okay, 
        
        node->addr = node->kids[0]->kids[0]->addr;
        node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode), // tac.c
                                        genInstr(O_ASN, node->addr, node->kids[0]->kids[1]->addr, node->kids[1]->addr));     // tac.c
        break;
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
        node->addr = genLocal(typeSize(node->type), node->table);
        node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode),            // tac.c
                                      genInstr(O_BEQ, node->addr, node->kids[0]->addr, node->kids[1]->addr)); // tac.c
            break;
        // need to figure these out
        case STRING_TYPE:
        case ARRAY_TYPE:
            break;

        default:
            break;
        }
    case notEqual:
    node->addr = genLocal(typeSize(node->type), node->table);
    node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode),            // tac.c
                                  genInstr(O_BNE, node->addr, node->kids[0]->addr, node->kids[1]->addr)); // tac.c
    // checks equality of pointers for structures same as == for primatives
    break;
    case eqeqeq:
    node->addr = genLocal(typeSize(node->type), node->table);
    node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode),            // tac.c
                                  genInstr(O_BEQ, node->addr, node->kids[0]->addr, node->kids[1]->addr)); // tac.c
                                  break;
    case notEqeqeq:
    node->addr = genLocal(typeSize(node->type), node->table);
    node->icode = appendInstrList(concatInstrList(node->kids[0]->icode, node->kids[1]->icode),            // tac.c
                                  genInstr(O_BNE, node->addr, node->kids[0]->addr, node->kids[1]->addr)); // tac.c
        break;

    case less:
    case greater:
    case lessEqual:
    case greaterEqual:
        op = (node->prodrule == less           ? O_BLT
              : node->prodrule == greater      ? O_BGT
              : node->prodrule == greaterEqual ? O_BGE
                                               : O_BLE);
        node->addr = genLocal(typeSize(node->type), node->table);
        node->icode = appendInstrList(
            concatInstrList(node->kids[0]->icode, node->kids[1]->icode),
            genInstr(op,
                     node->addr,
                     node->kids[0]->addr,
                     node->kids[1]->addr));
        break;
    case in:
        node->addr = genLocal(typeSize(node->type), node->table);
        node->icode = appendInstrList(
            concatInstrList(node->kids[0]->icode, node->kids[1]->icode),
            genInstr(O_IN,
                     node->addr,
                     node->kids[0]->addr,
                     node->kids[1]->addr));
        // PART OF FOOR LOOP
        break;

    case infixFunction:
        // figure out what to do here
        // might not need for k0, ask jeffery
        // might need to add infix funcs to type check
        break;

    case range:
    case rangeUntil:
        op = node->prodrule == range ? O_RNG : O_RNU;
        // Don't need to determine array size, just a range from x->y
        node->addr = genLocal(typeSize(node->type), node->table);
        node->icode = appendInstrList(
            concatInstrList(node->kids[0]->icode, node->kids[1]->icode),
            genInstr(op,
                     node->addr,
                     node->kids[0]->addr,
                     node->kids[1]->addr));
        break;
    case add:
    case sub:
    case mult:
    case div_k:
    case mod:
    {
        op = (node->prodrule == add     ? O_ADD
              : node->prodrule == sub   ? O_SUB
              : node->prodrule == mult  ? O_MUL
              : node->prodrule == div_k ? O_DIV

                                        : O_MOD);
        node->addr = genLocal(typeSize(node->type), node->table);
        node->icode = appendInstrList(
            concatInstrList(node->kids[0]->icode, node->kids[1]->icode),
            genInstr(op,
                     node->addr,
                     node->kids[0]->addr,
                     node->kids[1]->addr));

        break;
    }
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
        node->addr = genConst(node->leaf->ival);
        break;
    case REAL_LITERAL:
        node->addr = genConstD(node->leaf->dval);
        break;
    case TRUE:
        node->addr = genConst(1);
        break;
    case FALSE:
    case NULL_K:
    node->addr = genConst(0);
        break;

    case LINE_STRING:
    case MULTILINE_STRING:
    {
        int len = strlen(node->leaf->sval) + 1;

        // Calculate the size of the string
        struct addr *sizeAddr = malloc(sizeof *sizeAddr);
        sizeAddr->region = R_CONST;
        sizeAddr->u.offset = len;
        struct instr *code = genInstr(D_GLOB, sizeAddr, NULL, NULL);

        // Create the string address with our dedicated String region
        struct addr *strAddr = malloc(sizeof *strAddr);
        strAddr->region = R_STRING;
        strAddr->u.name = strdup(node->leaf->sval);
        code = appendInstrList(code, genInstr(D_GLOB, strAddr, NULL, NULL));
        recordStringLiteral(node->leaf->sval); // tac.c for printing
        node->icode = code;
        node->addr = strAddr;
        break;
    }

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

    // Evil idea: We make this control.
    case forStmntWithVars:
    case forStmnt:
        
        node->kids[1]->addr = genLocal(typeSize(node->kids[1]->type), node->kids[1]->table);
        thenLabel = genLabel();
        followLabel = genLabel();
        node->first = thenLabel;
        node->follow = followLabel;
        node->icode = appendInstrList(genInstr(O_ASN, node->kids[1]->addr, node->kids[2]->kids[0]->addr, 0), node->kids[1]->icode);
        node->icode = appendInstrList(node->icode, genInstr(D_LABEL, thenLabel, 0, 0));
        if(node->kids[2]->prodrule == RANGE){ // RANGE
            node->icode = appendInstrList(node->icode, genInstr(O_BLT, node->kids[1]->addr, node->kids[2]->kids[1]->addr, 0));
        } else{ // RANGE UNTIL
            node->icode = appendInstrList(node->icode, genInstr(O_BLE, node->kids[1]->addr, node->kids[2]->kids[1]->addr, 0));
        }
        
        node->icode = appendInstrList(
            node->icode,
            genInstr(O_BNIF, followLabel, node->kids[1]->addr, NULL));
            node->icode = appendInstrList(
                node->icode,
                genInstr(O_GOTO, followLabel, NULL, NULL));
        node->icode = appendInstrList(node->icode, node->kids[3]->icode);
        node->icode = appendInstrList(node->icode, genInstr(O_ADD, node->kids[1]->addr, genConst(1), node->kids[1]->addr));
        node->icode = appendInstrList(node->icode, genInstr(O_GOTO, thenLabel, 0, 0));
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, followLabel, NULL, NULL));
        break;
    case whileStmntCtrlBody:
    thenLabel = genLabel();
    followLabel = genLabel();
    node->first = thenLabel;
    node->follow = followLabel;
    node->icode = appendInstrList(genInstr(D_LABEL, thenLabel, 0, 0), node->kids[1]->icode);
    node->icode = appendInstrList(
        node->icode,
        genInstr(O_BNIF, followLabel, node->kids[1]->addr, NULL));
        node->icode = appendInstrList(
            node->icode,
            genInstr(O_GOTO, followLabel, NULL, NULL));
    node->icode = appendInstrList(node->icode, node->kids[2]->icode);
    node->icode = appendInstrList(node->icode, genInstr(O_GOTO, thenLabel, 0, 0));
    node->icode = appendInstrList(
        node->icode,
        genInstr(D_LABEL, followLabel, NULL, NULL));
    break;
    case whileStmnt:
        thenLabel = genLabel();
        node->icode = appendInstrList(genInstr(D_LABEL, thenLabel, 0, 0), node->kids[1]->icode);
        followLabel = genLabel();
        node->icode = appendInstrList(node->icode, genInstr(O_BNIF, thenLabel, node->kids[1]->addr, 0));
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, followLabel, NULL, NULL));

        break;
    case doWhileStmnt:


        thenLabel = genLabel();
        followLabel = genLabel();
        node->first = thenLabel;
        node->follow = followLabel;
        node->icode = appendInstrList(genInstr(D_LABEL, thenLabel, 0, 0), node->kids[2]->icode);


        node->icode = appendInstrList(node->icode, node->kids[1]->icode);
        node->icode = appendInstrList(
            node->icode,
            genInstr(O_BNIF, thenLabel, node->kids[1]->addr, NULL));
            node->icode = appendInstrList(
                node->icode,
                genInstr(O_GOTO, thenLabel, NULL, NULL));
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, followLabel, NULL, NULL));
        break;
    case emptyIf:
    case if_k:
        // conditional without else

        node->icode = node->kids[1]->icode;
        thenLabel = genLabel();
        followLabel = genLabel();
        node->first = thenLabel;
        node->follow = followLabel;
        node->icode = appendInstrList(
            node->icode,
            genInstr(O_BNIF, followLabel, node->kids[1]->addr, NULL));
        node->icode = appendInstrList(node->icode, genInstr(O_GOTO, followLabel, 0, 0));
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, thenLabel, NULL, NULL));
        node->icode = appendInstrList(node->icode, node->kids[2]->icode);
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, followLabel, NULL, NULL));
        break;
    case ifElse:
    case ifElseIf:
        // conditional with else
        node->icode = node->kids[0]->icode;
        thenLabel = genLabel();
        elseLabel = genLabel();
        followLabel = genLabel();
        node->first = thenLabel;
        node->follow = followLabel;

        node->icode = appendInstrList(
            node->icode,
            genInstr(O_BNIF, elseLabel, node->kids[0]->addr, NULL));
        node->icode = appendInstrList(node->icode, genInstr(O_GOTO, elseLabel, 0, 0));
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, thenLabel, NULL, NULL));
        node->icode = appendInstrList(node->icode, node->kids[2]->icode);
        node->icode = appendInstrList(
            node->icode,
            genInstr(O_GOTO, followLabel, NULL, NULL));
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, elseLabel, NULL, NULL));
        node->icode = appendInstrList(node->icode, node->kids[4]->icode);
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, followLabel, NULL, NULL));

        break;
    case elvis:
        followLabel = genLabel();
        node->icode = node->kids[0]->icode;
        node->icode = appendInstrList(
            node->icode,
            genInstr(O_BNIF, elseLabel, node->kids[0]->addr, NULL));
        node->icode = appendInstrList(
            node->icode,
            genInstr(O_GOTO, followLabel, NULL, NULL));
        node->icode = appendInstrList(
            node->icode,
            node->kids[1]->icode);
        node->icode = appendInstrList(
            node->icode,
            genInstr(D_LABEL, followLabel, NULL, NULL));

        break;
    // case postfixExpr:
    // case postfixNoExpr:
    // case postfixDotID:
    // case postfixDotIDExpr:
    // case postfixDotIDNoExpr:
    // case postfixSafeDotID:
    // case postfixSafeDotIDExpr:
    // case postfixSafeDotIDNoExpr:
    // case funcBody:
    case returnVal:
    {
        // kid[0] is the expression being returned
        struct tree *expr = node->kids[1];
        // splice in its code…
        struct instr *code = expr->icode
                                 ? copyInstrList(expr->icode)
                                 : NULL;
        // code = appendInstrList(
        //     code,
        //     genInstr(O_RET, expr->addr, NULL, NULL));
        node->icode = code;
        // This may be very useless.
        node->icode = appendInstrList(node->kids[1]->icode, // tac.c
                                      genInstr(O_RET, node->kids[1]->addr, NULL, NULL));     // tac.c
        break;
    }
    case RETURN:
    {
        // emit a RET const:0
        node->icode = genInstr(O_RET, genConst(0), NULL, NULL);
        break;
    }
    default:
    {
        // THE DARK SECRET OF WHY BASIC INSTRUCTIONS WORK. PRAISE THY ANCIENT TEXT!
        if (node->nkids > 0)
        {
            node->icode = copyInstrList(node->kids[0]->icode);
            for (int i = 1; i < node->nkids; i++)
            {
                node->icode = appendInstrList(
                    node->icode,
                    node->kids[i]->icode);
            }
        }
        break;
    }
    }
    if (node->icode != NULL)
        node->icodeDone = 0;
}

void assignFirst(struct tree *node)
{
    for (int i = 0; i < node->nkids; i++)
    {
        assignFirst(node->kids[i]);
    }
    // This fixed assignFollow() anticipating a .first for "{" in if_k
    switch (node->prodrule)
    {
    case emptyIf:
    case if_k:
    case ifElse:
    case ifElseIf:
    case forStmnt:
    case forStmntWithVars:
    case whileStmnt:
    case whileStmntCtrlBody:
    case doWhileStmnt:
        /* only gen a .first if this node has icode, its parent does not,
        and that parent actually exists
        */
        node->first = genLabel();
        if (node->icode && node->parent && node->parent->icode == NULL)
        {
            node->first = genLabel();
        }
        break;
    default:
        break;
    }

    // do i need more than this??
    // are there any other times where you would need a first label
    // in assignFollow and assignOnTrueFasle print out message saying that something needs to be added here if something is NULL
}

/**
 * @brief Assigns the labels for on follow.
 *
 * @param node
 */
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
        last = node->nkids - 1;
        node->kids[last]->follow = node->first;

        break;
    case doWhileStmnt:
        int body = 1;
        int test = node->nkids - 1;
        node->kids[body]->follow = node->kids[test]->first;
        break;

    // need to do speceal something for assignment ifs???
    case emptyIf:
    case if_k:
        node->onTrue = node->first;
        if (node->onTrue == NULL)
        {
            debugICode("if_k(first): Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->follow;
        if (node->onFalse == NULL)
        {
            debugICode("if_k(follow): Missing something in assignFollow", node);
        }
        break;
    case ifElseIf:
        // node->onTrue = node->kids[2]->first;
        if (node->onTrue == NULL)
        {
            debugICode("ifelseif: Missing something in assignFirst", node->kids[2]);
        }
        node->onFalse = node->kids[4]->first;
        if (node->onFalse == NULL)
        {
            debugICode("ifelseif: Missing something in assignFirst", node->kids[4]);
        }
        break;

    case elvis:
        // TODO:
        // i think this goes here
        // idk tho
        //  go left: null -> right side first block, fall down
        node->kids[0]->follow = node->kids[1]->first;
        node->kids[1]->follow = node->follow;
        break;

    case postfixDotID:
    case postfixDotIDExpr:
    case postfixDotIDNoExpr:
    case postfixSafeDotID:
    case postfixSafeDotIDExpr:
    case postfixSafeDotIDNoExpr:
        // TODO
        // figure all this thing out
        // at this point might not need the intital part???

    case funcBody:
        // TODO
        // treat like return value??
        // ASSIGNMENT expression
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

/**
 * @brief Assigns the labels for on true and on false.
 *
 * @param node
 */
void assignOnTrueFalse(struct tree *node)
{
    for (int i = 0; i < node->nkids; i++)
    {
        assignOnTrueFalse(node->kids[i]);
    }
    return;

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
        node->onTrue = node->first;
        if (node->onTrue == NULL)
        {
            debugICode("if_k(T/F):Missing something in assignFirst", node);
        }
        node->onFalse = node->follow;
        if (node->onFalse == NULL)
        {
            debugICode("if_k(T/F):Missing something in assignFollow", node);
        }
        break;
    case ifElseIf:
        node->onTrue = node->first;
        if (node->onTrue == NULL)
        {
            debugICode("ifElseIf(T/F):Missing something in assignFirst", node);
        }
        node->onFalse = node->kids[4]->first;
        if (node->onFalse == NULL)
        {
            debugICode("ifElseIf(T/F):Missing something in assignFirst", node->kids[4]);
        }
        break;
    case elvis:
        // true is when the value isn't null, might change latter
        // node->onTrue = node->kids[0]->first;
        // if (node->onTrue == NULL)
        // {
        //     debugICode("Missing something in assignFirst", node->kids[2]);
        // }
        // false is the alternate value, might change latter
        // node->onFalse = node->kids[1]->first;
        // if (node->onFalse == NULL)
        // {
        //     debugICode("Missing something in assignFollow", node->kids[4]);
        // }
        break;

    default:
        break;
    }
}
