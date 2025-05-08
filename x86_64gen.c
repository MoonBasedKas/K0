#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "x86_64gen.h"
#include "tac.h"
#include "tree.h"

/*
We can make it
Everything will be fine
*/

// Register pools
struct regdescrip regs[NUM_GPR];
struct regdescrip xmmRegs[NUM_XMM];
// Address map
struct addr_descrip *addrMap = NULL;

const char *argRegs[MAX_GPR_ARGS] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
};
const int argRegsIdx[MAX_GPR_ARGS] = {
    5, 4, 3, 2, 6, 7
};

static const char *gprNames[NUM_GPR] = {
    "%rax", "%rbx", "%rcx",
    "%rdx", "%rsi", "%rdi",
    "%r8", "%r9", "%r10",
    "%r11", "%r12", "%r13",
    "%r14", "%r15"
};

static const char *xmmNames[NUM_XMM] = {
    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
};

// Gonna make an internal buffer for the asm
static char **asmBuf;
static int bufCap = 0;
static int bufLen = 0;

extern *g_string_literals[];
extern int g_string_literal_count;
char **g_string_labels = NULL;

static int current_gpr_arg_idx = 0;
static int current_stack_arg_offset = 0;
static int current_func_local_size = 0;

/**
 *  @brief Initialize assembly buffer
 */
static void initAsmBuf(void) {
    if (asmBuf) { for (int i = 0; i < bufLen; i++) free(asmBuf[i]); free(asmBuf); }
    bufCap = 256;
    bufLen = 0;
    asmBuf = malloc(bufCap * sizeof(*asmBuf));
    if (!asmBuf) {
        fprintf(stderr, "Failed to allocate memory for asmBuf()\n");
        exit(1);
    }
}

/**
 *  @brief Emit assembly
 *  @param fmt Format string
 *  @param ... Arguments va_list goated
 */
static void emit(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char line[512];
    vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);

    if (bufLen >= bufCap) {
        bufCap *= 2;
        asmBuf = realloc(asmBuf, bufCap * sizeof(*asmBuf));
        if (!asmBuf){
            fprintf(stderr, "Out of memory in emit()\n");
            exit(1);
        }
    }
    asmBuf[bufLen++] = strdup(line);
    if (!asmBuf[bufLen-1]) {
        fprintf(stderr, "Error: Out of memory in emit strdup\n");
        exit(1);
    }
}

/**
 *  @brief Initialize register pools
 */
void initRegs(void) {
    for (int i = 0; i < NUM_GPR; i++) {
        strncpy(regs[i].name, gprNames[i], sizeof(regs[i].name));
        regs[i].name[sizeof(regs[i].name) - 1] = '\0';
        regs[i].status = 0;
        regs[i].a = NULL;
    }
    for (int i = 0; i < NUM_XMM; i++) {
        strncpy(xmmRegs[i].name, xmmNames[i], sizeof(xmmRegs[i].name));
        xmmRegs[i].name[sizeof(xmmRegs[i].name) - 1] = '\0';
        xmmRegs[i].status = 0;
        xmmRegs[i].a = NULL;
    }
}

/**
 *  @brief Initialize address map
 */
void initAddrDescrip(void){ /* Clear and initialize address descriptor map */
    struct addr_descrip *curr = addrMap, *next_d;
    while (curr) {
        next_d = curr->next;
        if ((curr->tac_addr.region == R_GLOBAL
            || curr->tac_addr.region == R_NAME
            || curr->tac_addr.region == R_STRING)
            && curr->tac_addr.u.name) {
            free(curr->tac_addr.u.name);
        }
        free(curr); curr = next_d;
    }
    addrMap = NULL;
}

/**
 *  @brief Compare two struct addr instances
 *  @param a1 First address
 *  @param a2 Second address
 *  @return 1 if addresses are equal, 0 otherwise
 */
int isAddrEqual(struct addr *a1, struct addr *a2) {
    if (!a1 || !a2) return 0;
    if (a1->region != a2->region) return 0;
    switch (a1->region)
    {
        case R_GLOBAL:
        case R_STRING:
        case R_NAME:
            return (a1->u.name && a2->u.name && strcmp(a1->u.name, a2->u.name) == 0);
        case R_LOCAL:
        case R_LABEL:
        case R_CONST:
            return (a1->u.offset == a2->u.offset);
        default:
            return 0;
    }
}

/**
 *  @brief Get/create address descriptor
 *  @param a Address to get descriptor for
 *  @return Address descriptor
 */
struct addr_descrip *getAddrDesc(struct addr *a)
{
    if (!a) return NULL;
    for (struct addr_descrip *d = addrMap; d; d = d->next)
    {
        if (isAddrEqual(&d->tac_addr, a))
        {
            return d;
        }
    }
    // Create new descriptor
    struct addr_descrip *d = malloc(sizeof(*d));
    if (!d) { fprintf(stderr, "Error: Failed to allocate addr_descrip\n"); exit(1); }
    d->status = 1; // Default: in memory only
    d->r = NULL;
    d->tac_addr = *a; // Store a copy
    // If it's a name, duplicate it to avoid issues with temp TAC names
    if ((a->region == R_GLOBAL || a->region == R_NAME || a->region == R_STRING) && a->u.name)
    {
        d->tac_addr.u.name = strdup(a->u.name);
        if (!d->tac_addr.u.name) { fprintf(stderr, "Error: strdup failed in getAddrDesc\n"); exit(1); }
    }
    d->next = addrMap;
    addrMap = d;
    return d;
}

/**
 *  @brief Initialize string literals
 *  @param root Root of AST
 */
void initStringLiterals(struct tree *root) {
    if (g_string_labels) {
        for (int i = 0; i < g_string_literal_count; i++)
        {
            if (g_string_labels[i]) free(g_string_labels[i]);
        }
        free(g_string_labels);
        g_string_labels = NULL; // Reset
    }

    if (g_string_literal_count > 0) {
        g_string_labels = malloc(g_string_literal_count * sizeof(char*));
        if (!g_string_labels) {
            fprintf(stderr, "Error: Failed to allocate string labels\n");
            exit(1);
        }
        for (int i = 0; i < g_string_literal_count; i++) {
            char label_buf[16];
            snprintf(label_buf, sizeof(label_buf), ".LC%d", i);
            g_string_labels[i] = strdup(label_buf);
            if (!g_string_labels[i]) {
                fprintf(stderr, "Error: strdup failed for string label .LC%d\n", i);
                exit(1);
            }
        }
    }
}

/**
 *  @brief Emit data section
 *  @param head Head of TAC list
 */
static void emitDataSections(struct instr *head_icode) /* Emit .data and .rodata */
{
    bool data_emitted = false, rodata_emitted = false;
    // Emit global variables (all as quad words)
    for (struct instr *p = head_icode; p; p = p->next)
    {
        if (p->opcode == D_GLOB && p->dest && p->dest->region == R_GLOBAL)
        {
            if (!data_emitted) { emit(".data"); data_emitted = true; }
            emit(".globl %s", p->dest->u.name);
            emit(".align 8");
            emit("%s:", p->dest->u.name);
            // global integers initialized as quad words.
            emit("\t.quad %lld", (p->src1 && p->src1->region == R_CONST) ? (long long)p->src1->u.offset : 0LL);
        }
        // TODO: Do we need global doubles?
    }
    // Emit string literals
    if (g_string_literal_count > 0 && g_string_labels)
    {
        if (!rodata_emitted) { emit(".section .rodata"); rodata_emitted = true; }
        for (int i = 0; i < g_string_literal_count; i++)
        {
            emit(".align 8");
            emit("%s:", g_string_labels[i]);
            // TODO: g_string_literals[i] is properly escaped for .string directive
            emit("\t.string \"%s\"", g_string_literals[i]);
        }
    }
    // TODO: Emit actual .double constants here for R_CONST_DOUBLE TACs
    // emit(".LC_DBL_0: .double 1.0");
}

/**
 *  @brief Mark a GPR as dirty
 *  @param regIdx Index of GPR to mark
 */
void markRegDirty(int regIdx)
{
    if (regIdx < 0 || regIdx >= NUM_GPR || regs[regIdx].status == 0) return;
    regs[regIdx].status = 2; // 2 = dirty
    if (regs[regIdx].a)
    {
        struct addr_descrip *d = getAddrDesc(regs[regIdx].a);
        if (d) d->status = 4; // 4 = in register (dirty)
    }
}

/**
 *  @brief Spill a register to memory
 *  @param reg_d Register descriptor
 */
void spillReg(struct regdescrip *reg_d)
{
    if (!reg_d || reg_d->status != 2 || !reg_d->a) return; // Not dirty or no TAC addr
    struct addr_descrip *addr_d = getAddrDesc(reg_d->a);
    if (!addr_d) return; // Shouldn't happen if reg_d->a is valid

    // emit("# Spilling dirty reg %s for TAC addr in region %d", reg_d->name, reg_d->a->region);
    switch (reg_d->a->region)
    { // All memory ops are quad words dumbass
        case R_LOCAL:
            emit("\tmovq\t%s, -%d(%%rbp)", reg_d->name, reg_d->a->u.offset);
            break;
        case R_GLOBAL:
            emit("\tmovq\t%s, %s(%%rip)", reg_d->name, reg_d->a->u.name);
            break;
        default: // Constants, labels, strings should not be "spilled" to.
            emit("\t# WARNING: spillReg called for unspillable region %d with reg %s", reg_d->a->region, reg_d->name);
            break;
    }
    reg_d->status = 1; // Register clean (synced with memory)
    addr_d->status = 3; // Address is in memory and register (synced)
}

/**
 *  @brief Get free GPR
 *  @return Index of free GPR
 */
int getGPR(struct addr *for_addr) {
    for (int i = 0; i < NUM_GPR; i++)
    {
        // Simple check: is it empty?
        // TODO: More sophisticated: avoid ABI regs if setting up params, or %rax/%rdx during DIV
        // Im not 100% if we need to check for ABI AT ALL, but wiki says its important
        if (regs[i].status == 0)
        { // Found empty
            regs[i].status = 1; // Mark as loaded/clean
            regs[i].a = for_addr;
            if (for_addr)
            {
                struct addr_descrip *d = getAddrDesc(for_addr);
                d->r = &regs[i];
                d->status = 2; // In register only (clean)
            }
            return i;
        }
    }
    // need to spill → pick %rbx or similar non-critical
    // LRU FRAMES YOU BASTARDS
    int spill_candidate_idx = 1; // %rbx as a default spill candidate
    // emit("# getGPR: No free GPRs, spilling %s", regs[spill_candidate_idx].name);
    if (regs[spill_candidate_idx].a)
    {
         struct addr_descrip *old_addr_d = getAddrDesc(regs[spill_candidate_idx].a);
         spillReg(&regs[spill_candidate_idx]); // Spill it (if it was dirty)
         if (old_addr_d)
         {
            old_addr_d->r = NULL; // No longer in this specific register
            old_addr_d->status = 1; // Back to memory only
         }
    }
    // Reassign the spilled register
    regs[spill_candidate_idx].status = 1;
    regs[spill_candidate_idx].a = for_addr;
    if (for_addr)
    {
        struct addr_descrip *d = getAddrDesc(for_addr);
        d->r = &regs[spill_candidate_idx];
        d->status = 2; // In register only (clean)
    }
    return spill_candidate_idx;
}

/**
 *  @brief Free GPR
 *  @param idx Index of GPR to free
 */
void freeGPR(int idx)
{
    if (idx < 0 || idx >= NUM_GPR || regs[idx].status == 0) return; // Already free or invalid
    spillReg(&regs[idx]); // Spill if value was modified and not yet written back
    if (regs[idx].a)
    {
        struct addr_descrip *d = getAddrDesc(regs[idx].a);
        // If this register was the location for 'a', update 'a's descriptor
        if (d && d->r == &regs[idx])
        {
            d->r = NULL;
            if (d->status == 2 || d->status == 3 || d->status == 4) d->status = 1;
        }
    }
    regs[idx].status = 0; // Mark as empty
}

/**
 *  @brief Get free XMM
 *  @return Index of free XMM
 */
int getXMM(void) { // todo
    for (int i = 0; i < NUM_XMM; i++){
        if (xmmRegs[i].status == 0){
            xmmRegs[i].status = 1;
            return i;
        }
    }
    fprintf(stderr, "No free XMMs left\n");
    exit(1);
}

/**
 *  @brief Free XMM
 *  @param idx Index of XMM to free
 */
void freeXMM(int idx) { // todo
    if (idx < 0 || idx >= NUM_XMM) return;
    xmmRegs[idx].status = 0;
    xmmRegs[idx].a = NULL;
}

/**
 *  @brief Ensure IR address is in GPR
 *  @param a Address to ensure in GPR (IR ptr)
 *  @param outReg Output register index
 *  @return 0 success
 */
int ensureInGPR(struct addr *a, int *outReg, int assign_to_reg_definitely)
{
    if (!a) { fprintf(stderr, "Error: ensureInGPR called with NULL addr\n"); exit(1); }
    struct addr_descrip *d = getAddrDesc(a);

    // Check if 'a' is already in a GPR and its value is current
    if (d->r != NULL && (d->status >= 2 /*in reg clean/dirty/synced*/))
    {
        int current_reg_idx = d->r - regs; // Calculate index from regdescrip pointer
        // Ensure the register actually holds 'a' and is not stale
        if (current_reg_idx >= 0 && current_reg_idx < NUM_GPR &&
            regs[current_reg_idx].a && isAddrEqual(regs[current_reg_idx].a, a))
        {
            *outReg = current_reg_idx;
            // emit("# ensureInGPR: %s already in %s", a->u.name ? a->u.name : "addr", regs[current_reg_idx].name);
            return 0;
        }
        else
        { // Descriptor out of sync, or register was re-used
            d->r = NULL;
            if (d->status != 1) d->status = 1; // Revert to "in memory only"
        }
    }

    // Not in a suitable register, or stale. Load it.
    int r = getGPR(a); // Get a GPR. 'a' is now associated with regs[r].
    *outReg = r;

    // emit("# ensureInGPR: Loading %s into %s", a->u.name ? a->u.name : "addr", regs[r].name);
    switch (a->region)
    {
        case R_LOCAL:
            emit("\tmovq\t-%d(%%rbp), %s", a->u.offset, regs[r].name);
            break;
        case R_GLOBAL:
            emit("\tmovq\t%s(%%rip), %s", a->u.name, regs[r].name);
            break;
        case R_CONST:
            emit("\tmovq\t$%lld, %s", (long long)a->u.offset, regs[r].name);
            break;
        case R_LABEL:
            emit("\tleaq\tL%d(%%rip), %s", a->u.offset, regs[r].name);
            break;
        case R_STRING:
            if (a->u.offset >= 0 && a->u.offset < g_string_literal_count && g_string_labels)
            {
                emit("\tleaq\t%s(%%rip), %s", g_string_labels[a->u.offset], regs[r].name);
            }
            else
            {
                fprintf(stderr, "Error: Invalid string literal index %d or labels not init\n", a->u.offset);
                exit(1);
            }
            break;
        case R_NAME: // Address of a named symbol (e.g., function name for indirect call)
            emit("\tleaq\t%s(%%rip), %s", a->u.name, regs[r].name);
            break;
        default:
            emit("\t# Error: ensureInGPR unhandled region %d", a->region);
            return 1; // Error
    }
    regs[r].status = 1; // Loaded, so it's clean
    // d->r was set by getGPR
    // If loaded from memory, it's synced. If it's a const/address, it's "in register, clean".
    d->status = (a->region == R_CONST  ||
                 a->region == R_LABEL  ||
                 a->region == R_STRING ||
                 a->region == R_NAME) ? 2 : 3;
    return 0;
}

/**
 *  @brief Ensure address is in XMM
 *  @param a Address to ensure in XMM
 *  @param outReg Output register index
 *  @return 1 if address is in XMM, 0 otherwise
 */
int ensureInXMM(struct addr *a, int *outReg) {
    int r = getXMM();
    xmmRegs[r].a = a;
    switch (a->region){
        case R_LOCAL:
            emit("\tmovsd\t-%d(%%rbp), %s", a->u.offset, xmmRegs[r].name);
            break;
        case R_GLOBAL:
            emit("\tmovsd\t%s(%%rip), %s", a->u.name, xmmRegs[r].name);
            break;
        default:
            emit("\t# unknown region %d in ensureInXMM()", a->region);
            break;
    }
    *outReg = r;
    return 0;
}

/**
 *  @brief Spill a GPR-backed IR addr back to memory
 *  I'm pretty sure this is just a movq to the stack
 *  based on the region and then we dump the register
 *  @param a IR Address ptr to spill
 */
void spillAddr(struct addr *a)
{
    struct addr_descrip *d = getAddrDesc(a);
    // Spill only if 'a' is in a register AND that register is dirty
    if (d && d->r && d->status == 4 /* in register (dirty) */)
    {
        // d->r points to the struct regdescrip
        spillReg(d->r);
    }
}

/**
 *  @brief Initialize code generation
 *  @param ast_root Root of AST
 */
void initCodeGen(struct tree *ast_root)
{
    initRegs();
    initAddrDescrip();
    initAsmBuf();
    initStringLiterals(ast_root); // Depends on g_string_literals being populated
}

/**
 *  @brief Translate IR
 *  @return
 */
int translateIcToAsm(struct tree *root)
{
    if (!root || !root->icode)
    {
        fprintf(stderr, "Error: No ICODE found for translation.\n");
        return 0;
    }
    struct instr *p;

    initCodeGen(root);

    emitDataSections(root->icode);
    emit(".text");

    for (p = root->icode; p; p = p->next)
    {
        switch (p->opcode)
        {
            case D_PROC:
            {
                char *fname = p->dest->u.name;
                int locals_size = (p->src2 && p->src2->region == R_CONST) ? p->src2->u.offset : 0;
                current_func_local_size = locals_size; // Save for D_END

                emit(".globl %s", fname);
                emit("%s:", fname);
                emit("\tpushq\t%%rbp");
                emit("\tmovq\t%%rsp, %%rbp");
                if (locals_size > 0)
                {
                    emit("\tsubq\t$%d, %%rsp", locals_size);
                }

                current_gpr_arg_idx = 0;
                current_stack_arg_offset = 0;
                break;
            }
            case D_END:
            {
                // Use current_func_local_size captured at D_PROC
                if (current_func_local_size > 0)
                {
                    emit("\taddq\t$%d, %%rsp", current_func_local_size); // Deallocate locals
                }
                emit("\tpopq\t%%rbp");
                emit("\tret");
                current_func_local_size = 0;
                break;
            }
            case D_LABEL:
            {
                emit("L%d:", p->dest->u.offset);
                break;
            }
            case O_GOTO:
            {
                emit("\tjmp\tL%d", p->dest->u.offset);
                break;
            }
            case O_PARM:
            {
                int r_src;
                ensureInGPR(p->src1, &r_src, 0);

                if (current_gpr_arg_idx < MAX_GPR_ARGS)
                { // Use ABI GPR
                    emit("\tmovq\t%s, %s", regs[r_src].name, argRegs[current_gpr_arg_idx++]);
                }
                else
                {
                    emit("\tpushq\t%s", regs[r_src].name);
                    current_stack_arg_offset += 8; // Quad word
                }
                freeReg(r_src); // Value is now in ABI reg or on stack
                break;
            }
            case O_CALL:
            {
                // p->dest is func name (R_NAME) or func addr
                // p->src1 is arg count (R_CONST) - currently not used by this codegen for stack cleanup
                // p->src2 is where to store result (e.g. R_LOCAL for a temp)

                if (p->dest->region == R_NAME)
                {
                     emit("\tcall\t%s@PLT", p->dest->u.name); // Assuming external or needs PLT
                }
                else // Indirect call via register
                {
                    int r_func_addr;
                    ensureInGPR(p->dest, &r_func_addr, 0); // Load function address into a register
                    emit("\tcall\t*%s", regs[r_func_addr].name);
                    freeReg(r_func_addr);
                }

                // Clean up stack arguments IF any were pushed by O_PARM
                if (current_stack_arg_offset > 0)
                {
                    emit("\taddq\t$%d, %%rsp", current_stack_arg_offset);
                }

                // Store return value (usually in %rax for int/ptr/bool, %xmm0 for float/double)
                if (p->src2 && p->src2->region != R_NONE) // If there's a destination for the return value
                {
                    // TODO: Check type of p->src2; if double, use %xmm0 and movsd
                    struct addr_descrip *res_d = getAddrDesc(p->src2);
                    if (res_d->r) { freeReg(res_d->r - regs); res_d->r = NULL; }

                    if (p->src2->region == R_LOCAL) emit("\tmovq\t%%rax, -%d(%%rbp)", p->src2->u.offset);
                    else if (p->src2->region == R_GLOBAL) emit("\tmovq\t%%rax, %s(%%rip)", p->src2->u.name);
                    else { emit("\t# O_CALL: unhandled dest region %d for return val", p->src2->region); }
                    res_d->status = 1; // Mark as in memory
                }
                // Reset arg counters for next call sequence
                current_gpr_arg_idx = 0;
                current_stack_arg_offset = 0;
                break;
            }
            case O_RET:
            {
                if (p->src1 && p->src1->region != R_NONE)
                {
                    int r_ret_val;
                    ensureInGPR(p->src1, &r_ret_val, 0); // Load return value into a register
                    emit("\tmovq\t%s, %%rax", regs[r_ret_val].name); // Move to %rax
                    freeReg(r_ret_val);
                }
                // Else: void return, %rax content not explicitly set by this TAC.
                // Actual 'ret' instruction is part of D_END
                break;
            }
            case O_ASN:
            {
                int r_src;
                ensureInGPR(p->src1, &r_src, 0); // Load src1. Pass 0 for assign_to_reg.
                struct addr_descrip *dest_d = getAddrDesc(p->dest);
                if (dest_d->r) { freeReg(dest_d->r - regs);
                dest_d->r = NULL; }

                if (p->dest->region == R_LOCAL) emit("\tmovq\t%s, -%d(%%rbp)", regs[r_src].name, p->dest->u.offset);
                else if (p->dest->region == R_GLOBAL) emit("\tmovq\t%s, %s(%%rip)", regs[r_src].name, p->dest->u.name);
                else { emit("\t# O_ASN: unhandled dest region %d", p->dest->region); }
                dest_d->status = 1;
                freeReg(r_src);
                break;
            }
            case O_ADD: case O_SUB: case O_MUL:
            {
                int r_s1, r_s2, r_dst_val;
                ensureInGPR(p->src1, &r_s1, 0);
                ensureInGPR(p->src2, &r_s2, 0);
                r_dst_val = getGPR(p->dest);

                emit("\tmovq\t%s, %s", regs[r_s1].name, regs[r_dst_val].name); // r_dst_val = src1
                const char *op_str = (p->opcode == O_ADD) ? "addq" :
                                     (p->opcode == O_SUB) ? "subq" : "imulq"; // imulq for signed mul
                emit("\t%s\t%s, %s", op_str, regs[r_s2].name, regs[r_dst_val].name); // r_dst_val op= src2

                markRegDirty(r_dst_val);    // Value in r_dst_val is new
                spillReg(&regs[r_dst_val]); // Store result from r_dst_val to p->dest's memory

                freeReg(r_s1);
                freeReg(r_s2);
                freeReg(r_dst_val); // Result is now in memory for p->dest
                break;
            }
            case O_DIV: case O_MOD:
            {
                int r_s1_dividend, r_s2_divisor;
                ensureInGPR(p->src1, &r_s1_dividend, 0);
                ensureInGPR(p->src2, &r_s2_divisor, 0);

                emit("\tmovq\t%s, %%rax", regs[r_s1_dividend].name); // Dividend to %rax
                emit("\tcqto");                                     // Sign-extend %rax into %rdx:%rax for idivq
                emit("\tidivq\t%s", regs[r_s2_divisor].name);      // Divide %rdx:%rax by divisor reg
                // holy fuck

                freeReg(r_s1_dividend);
                freeReg(r_s2_divisor);

                char *result_source_reg = (p->opcode == O_DIV) ? "%rax" : "%rdx"; // Quotient or Remainder
                struct addr_descrip *dest_d = getAddrDesc(p->dest);
                if (dest_d->r) { freeReg(dest_d->r - regs); dest_d->r = NULL; }

                if (p->dest->region == R_LOCAL) emit("\tmovq\t%s, -%d(%%rbp)", result_source_reg, p->dest->u.offset);
                else if (p->dest->region == R_GLOBAL) emit("\tmovq\t%s, %s(%%rip)", result_source_reg, p->dest->u.name);
                else { emit("\t# O_DIV/MOD: unhandled dest region %d", p->dest->region); }
                dest_d->status = 1;
                // TODO: Restore %rax, %rdx if they were saved
                break;
            }
            case O_BEQ: case O_BNE: case O_BLT: case O_BLE: case O_BGT: case O_BGE:
            {
                int r1, r2;
                ensureInGPR(p->src1, &r1, 0);
                ensureInGPR(p->src2, &r2, 0);
                emit("\tcmpq\t%s, %s", regs[r2].name, regs[r1].name);

                const char *jmp_instr =
                    (p->opcode == O_BEQ) ? "je"  : (p->opcode == O_BNE) ? "jne" :
                    (p->opcode == O_BLT) ? "jl"  : (p->opcode == O_BLE) ? "jle" :
                    (p->opcode == O_BGT) ? "jg"  : "jge";
                emit("\t%s\tL%d", jmp_instr, p->dest->u.offset);
                freeReg(r1);
                freeReg(r2);
                break;
            }
            // Need NEG and BIF
            default:
                emit("\t# Error: UNKNOWN OPCODE %d (%s)", p->opcode, opCodeName(p->opcode) ? opCodeName(p->opcode) : "unknown_name");
                // fprintf(stderr, "Unknown opcode %d in translateIcToAsm()\n", p->opcode); // Keep for debugging
                break;
        }
    }
    return 1;
}

/**
 *  @brief Write x86-64 assembly to file and cleanup
 *  @param fp File pointer
 *  @return 1 if successful, 0 otherwise
 */
int writeAsm(FILE *fp)
{
    if (!fp || !asmBuf) return 0;
    for (int i = 0; i < bufLen; i++)
    {
        fprintf(fp, "%s\n", asmBuf[i]);
        free(asmBuf[i]); // Free the duplicated line
    }
    free(asmBuf);
    asmBuf = NULL; // NO DOUBLE FREE ERIK
    bufLen = 0;
    bufCap = 0;
    return 1;
}

// Dummy g_string_literals
#ifndef G_STRING_LITERALS_DEFINED
#define G_STRING_LITERALS_DEFINED
char *g_string_literals[] = {"Default k0 test string."};
int g_string_literal_count = 1;
#endif
