#include "tac.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    struct instr *instrList;
    struct instr *nextInstr;

    /* Team:
            I kept offset as the primary variable for whatever int
            representation we need it for per region, instead of adding
            new fields like "size", "params", etc. For example:

            R_CONST:
                offset is the value of the constant
            R_LOCAL:
                offset is the offset in local region (this grows)
            R_GLOBAL:
                offset is number of words of space
            etc.
                -Erik
    */

    //.string 8
    {
        struct addr *string8 = malloc(sizeof(struct addr));
        string8->region = R_GLOBAL;
        string8->u.offset = 8;

        instrList = gen(D_GLOB, string8, NULL, NULL);
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
    //.code
    {
        struct addr *code = malloc(sizeof(struct addr));
        code->region = R_NAME;
        code->u.name = ".code";

        struct instr *codeInstr = gen(D_LABEL, code, NULL, NULL);
        nextInstr->next = codeInstr;
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

    //fake symbol table
    struct addr *addrI = genLocal(8);

    //i = 5
    {
        struct addr *const5 = malloc(sizeof(struct addr));
        const5->region = R_CONST;
        const5->u.offset = 5;

        struct instr *iAsn = gen(O_ASN, addrI, const5, NULL); 
        nextInstr->next = iAsn;
        nextInstr = nextInstr->next;
    }
    //t1 = i * i
    {
        struct addr *t1 = genLocal(8);
        struct instr *iMult = gen(O_MUL, t1, addrI, addrI);
        nextInstr->next = iMult;
        nextInstr = nextInstr->next;
    }

    //(i * i) + 1
    {
        struct addr *t1result = nextInstr->dest;
        struct addr *t2 = genLocal(8);
        struct addr *const1 = malloc(sizeof(struct addr));
        const1->region = R_CONST;
        const1->u.offset = 1;

        struct instr *iAdd = gen(O_ADD, t2, t1result, const1);
        nextInstr->next = iAdd;
        nextInstr = nextInstr->next;
    }

    // i = t2
    {
        struct addr *t2result = nextInstr->dest;
        struct instr *iAsnT2 = gen(O_ASN, addrI, t2result, NULL);
        nextInstr->next = iAsnT2;
        nextInstr = nextInstr->next;
    }

    // push param 2 (i)
    {
        struct instr *Param2 = gen(O_PARM, addrI, NULL, NULL);
        nextInstr->next = Param2;
        nextInstr = nextInstr->next;
    }

    // push param 1 ("Variable i is %d.")
    {
        struct addr *str0 = malloc(sizeof(struct addr));
        str0->region = R_NAME;
        str0->u.offset = 0;

        struct instr *Param1 = gen(O_PARM, str0, NULL, NULL);
        nextInstr->next = Param1;
        nextInstr = nextInstr->next;
    }

    // t5 = printf()
    // I'm pretty sure this is (name, params, (optional) return location?)
    {
        struct addr *printfName = malloc(sizeof(struct addr));
        printfName->region = R_NAME;
        printfName->u.name = "printf";

        struct addr *callParams = malloc(sizeof(struct addr));
        callParams->region = R_CONST;
        callParams->u.offset = 2;

        struct addr *rvLocation = malloc(sizeof(struct addr));
        rvLocation->region = R_LOCAL;
        rvLocation->u.offset = 24;

        struct instr *printfCall = gen(O_CALL, printfName, callParams, rvLocation);
        nextInstr->next = printfCall;
        nextInstr = nextInstr->next;
    }

    // return
    {
        struct instr *ret = gen(O_RET, NULL, NULL, NULL);
        nextInstr->next = ret;
        nextInstr = nextInstr->next;
    }

    return 0;
}