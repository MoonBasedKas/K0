#include "tac.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    struct instr *instrList;
    struct instr *nextInstr;

    //fake symbol table
    struct addr *addrI = genLocal(8);

    //i = 5
    instrList = gen(O_ASN, addrI, NULL, NULL); //2nd arg needs to be a const
    nextInstr = instrList;

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