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
char *regionnames[] = {"global", "loc", "class", "lab", "const", "str", "name", "none"};
char *regionName(int i)
{
    return regionnames[i - R_GLOBAL];
}
char *opcodenames[] = {
    "ADD", "SUB", "MUL", "DIV", "MOD", "RNG", "RNU", "IN", "NEG", "AND", "OR", "XOR", "NOT", "ASN", "ADDR", "LCONT", "SCONT", "GOTO",
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

    const int SLOT = 8;
    // Offset-ing maybe?
    scope->varSize = ((scope->varSize + SLOT - 1) / SLOT) * SLOT;

    struct addr *a = malloc(sizeof(struct addr));
    if (!a)
    {
        fprintf(stderr, "out of memory\n");
        exit(4);
    }
    memset(a, 0, sizeof(struct addr));
    a->region = R_LOCAL;
    a->u.offset = scope->varSize;
    scope->varSize += SLOT;
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


struct addr *genConstD(double val){
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
 *  Stuff for grabbing the string list for easier printing
 *  D_GLOB printed where it wanted
 *  fuck D_GLOB
 */

struct StringLit
{
    char *s;
    int len;
    struct StringLit *next;
};

static struct StringLit *stringHead = NULL;

void recordStringLiteral(const char *raw)
{
    struct StringLit *n = malloc(sizeof *n);
    n->len = strlen(raw) + 1;
    n->s = strdup(raw);
    n->next = stringHead;
    stringHead = n;
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
        fprintf(iTarget, "global:%d", a->u.offset);
        break;
    case R_LOCAL:
        fprintf(iTarget, "loc:%d", a->u.offset);
        break;
    case R_LABEL:
        fprintf(iTarget, "label:%d", a->u.offset);
        break;
    case R_CONST:
        fprintf(iTarget, "const:%d", a->u.offset);
        break;
    case R_STRING: // Catch like below
        if (a->u.name)
        {
            fprintf(iTarget, "\"%s\"", a->u.name);
        }
        else
        {
            fprintf(iTarget, "str:%d", a->u.offset);
        }
        break;
    case R_NAME:
        // If .u.name is set, print it; otherwise, fallback to offset.
        if (a->u.name)
        {
            fprintf(iTarget, "%s", a->u.name);
        }
        else
        {
            // "str:0" if offset = 0
            fprintf(iTarget, "str:%d", a->u.offset);
        }
        break;
    default:
        fprintf(iTarget, "unknown:%d", a->u.offset);
        break;
    }
}

/**
 * @brief Drops the string section
 *
 * This was easier than trying to align everything in a cursed IF ELSE IF ELSE IF ELSE IF ELSE IF ELSE IF ELSE IF ELSE
 * @param out
 */
void dumpStringSection(FILE *out)
{
    for (struct StringLit *p = stringHead; p; p = p->next)
    {
        // one .string directive per literal
        fprintf(out, ".string %d\n", p->len);
        fprintf(out, "\t\"%s\"\n", p->s);
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

    // first emit all string literals
    dumpStringSection(iTarget);

    while (p)
    {
        // skip the string length + data pair since we already dumped them
        if (p->opcode == D_GLOB && p->dest && p->dest->region == R_CONST && p->next && p->next->opcode == D_GLOB && p->next->dest && p->next->dest->region == R_STRING)
        {
            p = p->next->next;
            continue;
        }

        // skip any lone string length entry
        if (p->opcode == D_GLOB && p->dest && p->dest->region == R_CONST)
        {
            p = p->next;
            continue;
        }

        if (p->opcode == D_PROC)
        {
            // procedure header
            fprintf(iTarget, "proc ");
            printAddr(p->dest);
            fprintf(iTarget, ",%d,%d\n",
                    p->src1->u.offset,
                    p->src2->u.offset);
        }
        else if (p->opcode == D_LABEL)
        {
            // code section or label
            if (p->dest->region == R_NAME && strcmp(p->dest->u.name, ".code") == 0)
            {
                fprintf(iTarget, ".code\n");
            }
            else
            {
                fprintf(iTarget, "lab ");
                printAddr(p->dest);
                fprintf(iTarget, "\n");
            }
        }
        else if (p->opcode >= D_GLOB && p->opcode <= D_PROT)
        {
            // other pseudo-ops
            fprintf(iTarget, "\t%s ", pseudoName(p->opcode));
            if (p->dest)
                printAddr(p->dest);
            if (p->src1)
            {
                fprintf(iTarget, ",");
                printAddr(p->src1);
            }
            if (p->src2)
            {
                fprintf(iTarget, ",");
                printAddr(p->src2);
            }
            fprintf(iTarget, "\n");
        }
        else if (p->opcode == O_CALL)
        {
            // call instruction
            fprintf(iTarget, "\tCALL\t");
            printAddr(p->dest);
            fprintf(iTarget, ",%d,", p->src1->u.offset);
            printAddr(p->src2);
            fprintf(iTarget, "\n");
        }
        else if (p->opcode >= O_ADD && p->opcode <= O_RET)
        {
            // arithmetic or branch
            fprintf(iTarget, "\t%s\t", opCodeName(p->opcode));
            if (p->dest)
                printAddr(p->dest);
            if (p->src1)
            {
                fprintf(iTarget, ",");
                printAddr(p->src1);
            }
            if (p->src2)
            {
                fprintf(iTarget, ",");
                printAddr(p->src2);
            }
            fprintf(iTarget, "\n");
        }
        else
        {
            // anything else
            fprintf(iTarget, "UNKNOWN(%d)\n", p->opcode);
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