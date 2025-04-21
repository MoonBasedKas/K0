/*
 * Three Address Code - skeleton for CS 423
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tac.h"


FILE *iTarget = NULL;

// Handles names
char *regionnames[] = {"global", "loc", "class", "lab", "const", "", "none"};
char *regionName(int i)
{
    return regionnames[i - R_GLOBAL];
}
char *opcodenames[] = {
    "ADD", "SUB", "MUL", "DIV", "MOD", "NEG", "AND", "OR", "XOR", "NOT", "ASN", "ADDR", "LCONT", "SCONT", "GOTO",
    "BLT", "BLE", "BGT", "BGE", "BEQ", "BNE", "BIF", "BNIF", "PARM", "CALL",
    "RETURN"};
char *opCodeName(int i)
{
    return opcodenames[i - O_ADD];
}
char *pseudonames[] = {
    "glob", "proc", "loc", "lab", "end", "prot"};
char *pseudoName(int i)
{
    return pseudonames[i - D_GLOB];
}

int labelCounter;

/**
 * @brief Generates a label
 *
 * @return struct addr*
 */
struct addr *genLabel()
{
    struct addr *a = malloc(sizeof(struct addr));
    memset(a, 0, sizeof(struct addr));
    a->region = R_LABEL;
    a->u.offset = labelCounter++;
    printf("generated a label %d\n", a->u.offset);
    return a;
}

/**
 * @brief Generates a local
 *
 * @param size
 * @param scope
 * @return struct addr*
 */
struct addr *genLocal(int size, struct symTab *scope)
{
    struct addr *a = malloc(sizeof(struct addr));
    memset(a, 0, sizeof(struct addr));
    a->region = R_LOCAL;
    a->u.offset = scope->varSize;
    scope->varSize += size;
    return a;
}

/**
 * @brief Generates a constant
 *
 * @param val
 * @return struct addr*
 */
struct addr *genConst(int val)
{
    struct addr *a = malloc(sizeof(struct addr));
    memset(a, 0, sizeof(struct addr));
    a->region = R_CONST;
    a->u.offset = val;
    return a;
}

/**
 * @brief Generates the instruction
 *
 * @param op opcode
 * @param a1 dest
 * @param a2 source 1
 * @param a3 source 2
 * @return struct instr*
 */
struct instr *genInstr(int op, struct addr *a1, struct addr *a2, struct addr *a3)
{
    struct instr *rv = malloc(sizeof(struct instr));
    memset(rv, 0, sizeof(struct instr));
    if (rv == NULL)
    {
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

/**
 * @brief Copies an instruction list
 *
 * @param l
 * @return struct instr*
 */
struct instr *copyInstrList(struct instr *l)
{
    if (l == NULL)
        return NULL;
    struct instr *lcopy = genInstr(l->opcode, l->dest, l->src1, l->src2);
    lcopy->next = copyInstrList(l->next);
    return lcopy;
}

/**
 * @brief Appends to the instruction list
 *
 * @param l1
 * @param l2
 * @return struct instr*
 */
struct instr *appendInstrList(struct instr *l1, struct instr *l2)
{
    if (l1 == NULL)
        return l2;
    struct instr *ltmp = l1;
    while (ltmp->next != NULL)
        ltmp = ltmp->next;
    ltmp->next = l2;
    return l1;
}

struct instr *concatInstrList(struct instr *l1, struct instr *l2)
{
    return appendInstrList(copyInstrList(l1), l2);
}

/**
 * @brief Print the address in a human-readable format.
 *
 * @param a The address to print.
 */
static void printAddr(struct addr *a)
{
    if (!a)
        return;

    switch (a->region)
    {
    case R_GLOBAL:
        fprintf(iTarget,"global:%d", a->u.offset);
        break;
    case R_LOCAL:
        fprintf(iTarget,"loc:%d", a->u.offset);
        break;
    case R_LABEL:
        fprintf(iTarget,"label:%d", a->u.offset);
        break;
    case R_CONST:
        fprintf(iTarget,"const:%d", a->u.offset);
        break;
    case R_NAME:
        // If .u.name is set, print it; otherwise, fallback to offset.
        if (a->u.name)
        {
            fprintf(iTarget,"%s", a->u.name);
        }
        else
        {
            // "str:0" if offset = 0
            fprintf(iTarget,"str:%d", a->u.offset);
        }
        break;
    default:
        fprintf(iTarget,"unknown:%d", a->u.offset);
        break;
    }
}

/**
 * @brief Print the TAC code out
 *
 * @param code The TAC list to print.
 */
void tacPrint(struct instr *code)
{
    struct instr *p = code;
    while (p)
    {

        // Handle pseudo first (D_GLOB, D_PROC, etc.)
        if (p->opcode >= D_GLOB && p->opcode <= D_PROT)
        {

            /*
             * .string 8 => D_GLOB, dest->region = R_GLOBAL, offset=8
             * "Variable i is %d.\000" => D_GLOB, dest->region=R_NAME, .u.name=...
             */
            if (p->opcode == D_GLOB)
            {
                if (p->dest && p->dest->region == R_GLOBAL && p->dest->u.offset == 8)
                {
                    fprintf(iTarget,".string 8\n");
                }
                else if (p->dest && p->dest->region == R_NAME && p->dest->u.name)
                {
                    // e.g. "Variable i is %d.\000"
                    fprintf(iTarget,"\t%s\n", p->dest->u.name);
                }
                else
                {
                    // Print "glob <addr>"
                    fprintf(iTarget,"%s ", pseudoName(p->opcode));
                    if (p->dest)
                    {
                        printAddr(p->dest);
                    }
                    fprintf(iTarget,"\n");
                }
            }
            else if (p->opcode == D_LABEL)
            {
                // Possibly .code or some other label.
                if (p->dest && p->dest->region == R_NAME && p->dest->u.name && strcmp(p->dest->u.name, ".code") == 0)
                {
                    fprintf(iTarget,".code\n");
                }
                else
                {
                    // "lab <dest>"
                    fprintf(iTarget,"%s ", pseudoName(p->opcode));
                    if (p->dest)
                    {
                        printAddr(p->dest);
                    }
                    fprintf(iTarget,"\n");
                }
            }
            else if (p->opcode == D_PROC)
            {
                fprintf(iTarget,"proc ");
                // Usually dest=R_NAME:"main"
                if (p->dest && p->dest->region == R_NAME && p->dest->u.name)
                {
                    fprintf(iTarget,"%s", p->dest->u.name);
                }
                else if (p->dest)
                {
                    printAddr(p->dest);
                }

                fprintf(iTarget,",");

                // src1 => param count -> R_CONST
                if (p->src1 && p->src1->region == R_CONST)
                {
                    fprintf(iTarget,"%d", p->src1->u.offset);
                }
                else if (p->src1)
                {
                    printAddr(p->src1);
                }

                fprintf(iTarget,",");

                // src2 => local size -> R_CONST
                if (p->src2 && p->src2->region == R_CONST)
                {
                    fprintf(iTarget,"%d", p->src2->u.offset);
                }
                else if (p->src2)
                {
                    printAddr(p->src2);
                }

                fprintf(iTarget,"\n");
            }
            else
            {
                // pseudo-ops print
                fprintf(iTarget,"\t%s ", pseudoName(p->opcode));
                if (p->dest)
                {
                    printAddr(p->dest);
                }
                if (p->src1)
                {
                    fprintf(iTarget,",");
                    printAddr(p->src1);
                }
                if (p->src2)
                {
                    fprintf(iTarget,",");
                    printAddr(p->src2);
                }
            }
        }
        else if (p->opcode >= O_ADD && p->opcode <= O_RET)
        {
            // ops and stuff prints
            if (p->opcode == O_CALL)
            { // STUPID SPECIFIC FORMATING FOR MY OCD BRAIN
                fprintf(iTarget,"\tCALL\t");

                if (p->dest && p->dest->region == R_NAME && p->dest->u.name)
                {
                    fprintf(iTarget,"%s", p->dest->u.name);
                }
                else if (p->dest)
                {
                    printAddr(p->dest); // incase
                }

                fprintf(iTarget,",");

                // src1 => #params, const:2 -> print just "2" !!!!1!one
                if (p->src1 && p->src1->region == R_CONST)
                {
                    fprintf(iTarget,"%d", p->src1->u.offset);
                }
                else if (p->src1)
                {
                    printAddr(p->src1);
                }

                fprintf(iTarget,",");

                // src2 => return storage -> loc:24
                if (p->src2)
                {
                    printAddr(p->src2);
                }

                fprintf(iTarget,"\n");
            }
            else
            {
                fprintf(iTarget,"\t%s\t", opCodeName(p->opcode));
                if (p->dest)
                {
                    printAddr(p->dest);
                }
                if (p->src1)
                {
                    fprintf(iTarget,",");
                    printAddr(p->src1);
                }
                if (p->src2)
                {
                    fprintf(iTarget,",");
                    printAddr(p->src2);
                }
                fprintf(iTarget,"\n");
            }
        }
        else
        {
            fprintf(iTarget,"UNKNOWN(%d)\n", p->opcode);
        }
        p = p->next;
    }
}

/**
 * @brief A node in the FreedNode list.
 *
 * Tracks addresses that have been freed.
 * Thinking about our growing Valgrind issues
 */
struct FreedNode
{
    void *ptr;
    struct FreedNode *next;
};

/**
 * @brief Check if an address has already been freed.
 *
 * @param head The head of the FreedNode list.
 * @param p The pointer to check.
 * @return True if the pointer has been freed, false otherwise.
 */
static bool alreadyFreed(struct FreedNode *head, void *p)
{
    for (struct FreedNode *fn = head; fn != NULL; fn = fn->next)
    {
        if (fn->ptr == p)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Push a new FreedNode for 'p' onto the list. Returns the new head.
 *
 * @param head The head of the FreedNode list.
 * @param p The pointer to mark as freed.
 * @return The new head of the FreedNode list.
 */
static struct FreedNode *markFreed(struct FreedNode *head, void *p)
{
    struct FreedNode *fn = malloc(sizeof(struct FreedNode));
    fn->ptr = p;
    fn->next = head;
    return fn;
}

/**
 * @brief Free the TAC list.
 *
 * @param list da list
 */
void freeInstrList(struct instr *list)
{
    struct FreedNode *freedList = NULL;
    struct instr *cur = list;

    while (cur)
    {
        struct instr *next = cur->next;
        // tired of switchs
        if (cur->dest && !alreadyFreed(freedList, cur->dest))
        {
            freedList = markFreed(freedList, cur->dest);
            free(cur->dest);
        }
        if (cur->src1 && !alreadyFreed(freedList, cur->src1))
        {
            freedList = markFreed(freedList, cur->src1);
            free(cur->src1);
        }
        if (cur->src2 && !alreadyFreed(freedList, cur->src2))
        {
            freedList = markFreed(freedList, cur->src2);
            free(cur->src2);
        }
        free(cur);
        cur = next;
    }

    // Free FreedNode list freefreelistfreedfree
    while (freedList)
    {
        struct FreedNode *tmp = freedList->next;
        free(freedList);
        freedList = tmp;
    }
}