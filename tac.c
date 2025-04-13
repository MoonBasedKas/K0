/*
 * Three Address Code - skeleton for CS 423
 */
#include <stdio.h>
#include <stdlib.h>
#include "tac.h"

char *regionnames[] = {"global","loc", "class", "lab", "const", "", "none"};
char *regionname(int i) 
{ 
    return regionnames[i-R_GLOBAL]; 
}
char *opcodenames[] = {
   "ADD","SUB", "MUL", "DIV", "NEG", "ASN", "ADDR", "LCONT", "SCONT", "GOTO",
   "BLT", "BLE", "BGT", "BGE", "BEQ", "BNE", "BIF", "BNIF", "PARM", "CALL",
   "RETURN"
   };
char *opcodename(int i) 
{ 
    return opcodenames[i-O_ADD]; 
}
char *pseudonames[] = {
   "glob","proc", "loc", "lab", "end", "prot"
   };
char *pseudoname(int i) 
{ 
    return pseudonames[i-D_GLOB]; 
}

int labelcounter;

struct addr *genlabel()
{
    struct addr *a = malloc(sizeof(struct addr));
    a->region = R_LABEL;
    a->u.offset = labelcounter++;
    printf("generated a label %d\n", a->u.offset);
    return a;
}

struct instr *gen(int op, struct addr a1, struct addr a2, struct addr a3)
{
    struct instr *rv = malloc(sizeof (struct instr));
    if (rv == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(4);
    }
    rv->opcode = op;
    rv->dest = a1;
    rv->src1 = a2;
    rv->src2 = a3;
    rv->next = NULL;
    return rv;
}

struct instr *copylist(struct instr *l)
{
    if (l == NULL) return NULL;
    struct instr *lcopy = gen(l->opcode, l->dest, l->src1, l->src2);
    lcopy->next = copylist(l->next);
    return lcopy;
}

struct instr *append(struct instr *l1, struct instr *l2)
{
    if (l1 == NULL) return l2;
    struct instr *ltmp = l1;
    while(ltmp->next != NULL) ltmp = ltmp->next;
    ltmp->next = l2;
    return l1;
}

struct instr *concat(struct instr *l1, struct instr *l2)
{
    return append(copylist(l1), l2);
}