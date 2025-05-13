/*
 * Three Address Code - skeleton for CS 423
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tac.h"

#define MAX_STRINGS 256
char *g_string_literals[MAX_STRINGS];
int g_string_literal_count = 0;

FILE *iTarget = NULL;

// Handles names
char *regionnames[] = {"global", "loc", "class", "lab", "const", "str", "name", "none"};
char *regionName(int i)
{
    if (i >= R_GLOBAL && i <= R_NONE) return regionnames[i - R_GLOBAL];
    return "unknown_region";
}
char *opcodenames[] = {
    "ADD", "SUB", "MUL", "DIV", "MOD", "RNG", "RNU", "IN", "NEG", "AND", "OR", "XOR", "NOT", "ASN", "ADDR", "LCONT", "SCONT", "GOTO",
    "BLT", "BLE", "BGT", "BGE", "BEQ", "BNE", "BIF", "BNIF", "PARM", "CALL",
    "RETURN"};
char *opCodeName(int i)
{
    if (i >= O_ADD && i <= O_RET) return opcodenames[i - O_ADD];
    return NULL;
}
char *pseudonames[] = {
    "glob", "proc", "loc", "lab", "end", "prot"};
char *pseudoName(int i)
{
    if (i >= D_GLOB && i <= D_PROT) return pseudonames[i - D_GLOB];
    return "UNKNOWN_PSEUDO";
}

int labelCounter = 1;

/**
 * @brief Generates a label
 *
 * @return struct addr*
 */
struct addr *genLabel()
{
    struct addr *a = malloc(sizeof(struct addr));
    if (!a) { perror("malloc genLabel"); exit(EXIT_FAILURE); }
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
    scope->varSize += SLOT;
    a->u.offset = scope->varSize;
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
    if (!a) { perror("malloc genConst"); exit(EXIT_FAILURE); }
    memset(a, 0, sizeof(struct addr));
    a->region = R_CONST;
    a->u.offset = val;
    return a;
}


struct addr *genConstD(double val){
    struct addr *a = malloc(sizeof(struct addr));
    if (!a) { perror("malloc genConstD"); exit(EXIT_FAILURE); }
    memset(a, 0, sizeof(struct addr));
    a->region = R_CONST;
    if (sizeof(a->u.offset) >= sizeof(double)) {
        memcpy(&(a->u.offset), &val, sizeof(double));
    } else {
        a->u.offset = (int)val; // Truncation
    }
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
    if (rv == NULL)
    {
        fprintf(stderr, "out of memory\n");
        exit(4);
    }
    memset(rv, 0, sizeof(struct instr));
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
 * Stuff for grabbing the string list for easier printing
 * D_GLOB printed where it wanted
 * */

int recordStringLiteral(const char *raw)
{
    for (int i = 0; i < g_string_literal_count; i++) {
        if (g_string_literals[i] && strcmp(g_string_literals[i], raw) == 0) {
            return i;
        }
    }
    if (g_string_literal_count >= MAX_STRINGS) {
        fprintf(stderr, "Error: String table overflow (max %d strings). Increase MAX_STRINGS.\n", MAX_STRINGS);
        exit(4);
    }

    g_string_literals[g_string_literal_count] = strdup(raw);
    if (!g_string_literals[g_string_literal_count]) {
        fprintf(stderr, "strdup failed in recordStringLiteral\n");
        exit(4);
    }
    return g_string_literal_count++;
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
        if (a->u.name) fprintf(iTarget, "%s", a->u.name);
        else fprintf(iTarget, "global_val:%d", a->u.offset);
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
    case R_STRING:
        fprintf(iTarget, "str:%d", a->u.offset);
        break;
    case R_NAME:
        if (a->u.name)
        {
            fprintf(iTarget, "%s", a->u.name);
        }
        else
        {
            fprintf(iTarget, "name_val:%d", a->u.offset);
        }
        break;
    case R_NONE:
        fprintf(iTarget, "_");
        break;
    default:
        fprintf(iTarget, "unknown_reg:%d", a->u.offset);
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
    if (!out) return;
    for (int i = 0; i < g_string_literal_count; i++)
    {
        if (g_string_literals[i])
        {
            int len_with_null = strlen(g_string_literals[i]) + 1;
            fprintf(out, ".string %d\t\t; str:%d\n", len_with_null, i);
            fprintf(out, "\t\"%s\\000\"\n", g_string_literals[i]);
        }
    }
}

/**
 * @brief Print the TAC code out
 *
 * @param code The TAC list to print.
 */
void tacPrint(struct instr *code)
{
    if (!iTarget) {
        iTarget = stdout;
    }
    struct instr *p = code;

    dumpStringSection(iTarget);

    while (p)
    {
        /*
        if (p->opcode == D_GLOB && p->dest && p->dest->region == R_CONST && p->next && p->next->opcode == D_GLOB && p->next->dest && p->next->dest->region == R_STRING)
        {
            p = p->next->next;
            continue;
        }
        if (p->opcode == D_GLOB && p->dest && p->dest->region == R_CONST)
        {
            p = p->next;
            continue;
        }
        */

        if (p->opcode == D_PROC)
        {
            fprintf(iTarget, "proc ");
            printAddr(p->dest);
            fprintf(iTarget, ",%d,%d\n",
                    p->src1 ? p->src1->u.offset : 0,
                    p->src2 ? p->src2->u.offset : 0);
        }
        else if (p->opcode == D_LABEL)
        {
            if (p->dest && p->dest->region == R_NAME && p->dest->u.name && strcmp(p->dest->u.name, ".code") == 0)
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
            const char* ps_name = pseudoName(p->opcode);
            fprintf(iTarget, "\t%s ", ps_name ? ps_name : "UNKNOWN_PSEUDO");
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
            fprintf(iTarget, "\tCALL\t");
            printAddr(p->dest);
            fprintf(iTarget, ",%d,", p->src1 ? p->src1->u.offset : 0);
            printAddr(p->src2);
            fprintf(iTarget, "\n");
        }
        else if (p->opcode >= O_ADD && p->opcode <= O_RET)
        {
            const char* op_name = opCodeName(p->opcode);
            fprintf(iTarget, "\t%s\t", op_name ? op_name : "UNKNOWN_OP");
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
            fprintf(iTarget, "UNKNOWN_OPCODE_VAL(%d)\n", p->opcode);
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
    if (!fn) { perror("malloc markFreed"); exit(EXIT_FAILURE); }
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
    struct FreedNode *freedAddrList = NULL;
    struct instr *cur = list;
    struct instr *next_instr;

    while (cur)
    {
        next_instr = cur->next;
        // tired of switchs
        if (cur->dest && !alreadyFreed(freedAddrList, cur->dest))
        {
            // if ((cur->dest->region == R_GLOBAL || cur->dest->region == R_NAME) && cur->dest->u.name) {
            //     free(cur->dest->u.name);
            // }
            freedAddrList = markFreed(freedAddrList, cur->dest);
            free(cur->dest);
        }
        if (cur->src1 && !alreadyFreed(freedAddrList, cur->src1))
        {
            // if ((cur->src1->region == R_GLOBAL || cur->src1->region == R_NAME) && cur->src1->u.name) {
            //     free(cur->src1->u.name);
            // }
            freedAddrList = markFreed(freedAddrList, cur->src1);
            free(cur->src1);
        }
        if (cur->src2 && !alreadyFreed(freedAddrList, cur->src2))
        {
            // if ((cur->src2->region == R_GLOBAL || cur->src2->region == R_NAME) && cur->src2->u.name) {
            //     free(cur->src2->u.name);
            // }
            freedAddrList = markFreed(freedAddrList, cur->src2);
            free(cur->src2);
        }
        free(cur);
        cur = next_instr;
    }

    // Free FreedNode list freefreelistfreedfree
    struct FreedNode *tmp_freed_node;
    while (freedAddrList)
    {
        tmp_freed_node = freedAddrList->next;
        free(freedAddrList);
        freedAddrList = tmp_freed_node;
    }

    for (int i = 0; i < g_string_literal_count; i++) {
        if (g_string_literals[i]) {
            free(g_string_literals[i]);
            g_string_literals[i] = NULL;
        }
    }
    g_string_literal_count = 0;
}
