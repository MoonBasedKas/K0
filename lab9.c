#include "tac.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    struct instr *instrList;
    struct instr *nextInstr;

    //fake symbol table
    struct addr *addrI = genLocal(8);
    //.string 8
    {
        struct addr *global8 = malloc(sizeof(struct addr));
        global8->region = R_GLOBAL;
        global8->u.offset = 8;

        instrList = gen(D_GLOB, global8, NULL, NULL);
        nextInstr = instrList;
    }

    //"Variable i is %d.\000"
    {
        struct addr *strVar = malloc(sizeof(struct addr));
        strVar->region = R_NAME;
        strVar->u.name = "Variable i is %d.\000";

        struct instr *i = gen(D_GLOB, strVar, NULL, NULL);
        nextInstr->next = i;
        nextInstr = nextInstr->next;
    }

    // proc main,0,32
    {
        struct addr *procName = malloc(sizeof(struct addr));
        procName->region = R_NAME;
        procName->u.name = "main";

        struct addr *procParam = malloc(sizeof(struct addr));
        procParam->region = R_CONST;
        procParam->u.offset = 0;

        struct addr *procOffset = malloc(sizeof(struct addr));
        procOffset->region = R_CONST;
        procOffset->u.offset = 32;

        struct instr *procInstr = gen(D_PROC, procName, procParam, procOffset);
        nextInstr->next = procInstr;
        nextInstr = nextInstr->next;
    }
    //i = 5
    nextInstr->next = gen(O_ASN, addrI, NULL, NULL); //2nd arg needs to be a const
    nextInstr = nextInstr->next;

    //i * i
    nextInstr->next = gen(O_MUL, genLocal(8), addrI, addrI);
    nextInstr = nextInstr->next;

    //(i * i) + 1
    nextInstr->next = gen(O_MUL, genLocal(8), nextInstr->dest, NULL); //3rd arg needs to be a const
    nextInstr = nextInstr->next;

    // i = (i * i) + 1
    nextInstr->next = gen(O_ASN, addrI, nextInstr->dest, NULL);
    nextInstr = nextInstr->next;
}