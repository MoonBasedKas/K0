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

static const char *gprNames[NUM_GPR] = {
    "%rax", "%rbx", "%rcx",
    "%rdx", "%rsi", "%rdi",
    "%rbp", "%rsp", "%r8",
    "%r9", "%r10", "%r11",
    "%r12", "%r13"
};

static const char *xmmNames[NUM_XMM] = {
    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
};

// Gonna make an internal buffer for the asm
static char **asmBuf;
static int bufCap = 0;
static int bufLen = 0;

/**
 *  @brief Initialize assembly buffer
 */
static void initAsmBuf(void) {
    bufCap = 128;
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
    char line[256];
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
void initAddrDescrip(void){
    addrMap = NULL;
}

/**
 *  @brief Get address descriptor
 *  @param a Address to get descriptor for
 *  @return Address descriptor
 */
struct addr_descrip *getAddrDesc(struct addr *a) {
    struct addr_descrip *d = malloc(sizeof(*d));
    if (!d){
        fprintf(stderr, "Failed to allocate memory for addr_descrip\n");
        exit(1);
    }
    d->status = 1;
    d->r = NULL;
    d->a = *a;
    d->next = addrMap;
    addrMap = d;
    return d;
}

/**
 *  @brief Emit data section
 *  @param head Head of TAC list
 */
static void emitDataSection(struct instr *head) {
    bool dataStarted = false;
    for (struct instr *p = head; p; p = p->next) {
        if (p->opcode == D_GLOB && p->dest && p->dest->region == R_GLOBAL) {
            if (!dataStarted) {
                emit(".data");
                dataStarted = true;
            }
            // use our lab 11 stuff
            emit(".globl %s", p->dest->u.name);
            emit("%s:", p->dest->u.name);
            emit("\t.long %d", p->src1->u.offset);
        }
    }
}
/**
 *  @brief Get free GPR
 *  @return Index of free GPR
 */
int getGPR(void) {
    for (int i = 0; i < NUM_GPR; i++){
        if (regs[i].status == 0){
            regs[i].status = 1;
            return i;
        }
    }
    fprintf(stderr, "No free GPRs left\n");
    exit(1);
}

/**
 *  @brief Free GPR
 *  @param idx Index of GPR to free
 */
void freeGPR(int idx) {
    if (idx < 0 || idx >= NUM_GPR) return;
    regs[idx].status = 0;
    regs[idx].a = NULL;
}

/**
 *  @brief Get free XMM
 *  @return Index of free XMM
 */
int getXMM(void) {
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
void freeXMM(int idx) {
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
int ensureInGPR(struct addr *a, int *outReg) {
    int r = getGPR();
    regs[r].a = a;
    switch (a->region){
        case R_LOCAL:
            emit("\tmovq\t-%d(%%rbp), %s", a->u.offset, regs[r].name);
            break;
        case R_GLOBAL:
            emit("\tmovq\t%s(%%rip), %s", a->u.name, regs[r].name);
            break;
        case R_CONST:
            emit("\tmovq\t$%d, %s", a->u.offset, regs[r].name);
            break;
        case R_LABEL:
            emit("\tleaq\tL%d(%%rip), %s", a->u.offset, regs[r].name);
            break;
        default:
            emit("\t# unknown region %d in ensureInGPR()", a->region);
            break;
        // Need String and uh name?
    }
    *outReg = r;
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
            emit("\tmovq\t-%d(%%rbp), %s", a->u.offset, xmmRegs[r].name);
            break;
        case R_GLOBAL:
            emit("\tmovq\t%s(%%rip), %s", a->u.name, xmmRegs[r].name);
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
void spillAddr(struct addr *a) {
    for  (struct addr_descrip *d = addrMap; d; d = d->next) {
        if (d->a.region == a->region && d->a.u.offset == a->u.offset && d->r) {
            int idx = (struct regdescrip *)d->r - regs;
            switch (a->region){
                case R_LOCAL:
                    emit("\tmovq\t%s, -%d(%%rbp)", regs[idx].name, a->u.offset);
                    break;
                case R_GLOBAL:
                    emit("\tmovq\t%s, %s(%%rip)", regs[idx].name, a->u.name);
                    break;
                default:
                    emit("\t# unknown region %d in spillAddr()", a->region);
                    break;
            }
            freeGPR(idx);
            d->r = NULL;
            return;
        }
    }
    fprintf(stderr, "Spill failed for %s\n", a->u.name);
    exit(1);
}

/**
 *  @brief Translate IR to x86-64 assembly
 *  @return 1 if translation is successful, 0 otherwise
 */
int translateIcToAsm() {
    extern struct tree *root;
    struct instr *p;

    initRegs();
    initAddrDescrip();
    initAsmBuf();

    emitDataSection(root->icode);
    emit(".text");

    for (p = root->icode; p; p = p->next){
        switch (p->opcode){
            case D_PROC:
                char *fname = p->dest->u.name;
                int locals = p->src1->u.offset;
                emit(".globl %s", fname);
                emit("%s:", fname);
                emit("\tpushq\t%%rbp");
                emit("\tmovq\t%%rsp, %%rbp");
                if (locals > 0){
                    emit("\tsubq\t$%d, %%rsp", locals);
                }
                break;
            case D_END:
                if (p->src1->u.offset > 0)
                    emit("\taddq\t$%d, %%rsp", p->src1->u.offset);
                emit("\tpopq\t%%rbp");
                emit("\tret");
                break;
            case D_LABEL:
                emit("L%d:", p->dest->u.offset);
                break;
            case O_GOTO:
                emit("\tjmp L%d", p->src1->u.offset);
                break;
            case O_PARM:
                int r;
                ensureInGPR(p->dest, &r);
                emit("\tpushq\t%s", regs[r].name);
                freeGPR(r);
                break;
            case O_CALL:
                emit("\tcall\t%s@PLT", p->dest->u.name);
                if (p->src2) {
                    struct addr *ret = p->src2;
                    switch (ret->region) {
                        case R_LOCAL:
                            emit("\tmovq\t%%rax, -%d(%%rbp)", ret->u.offset);
                            break;
                        case R_GLOBAL:
                            emit("\tmovq\t%%rax, %s(%%rip)", ret->u.name);
                            break;
                        default:
                            break;
                    }
                }
                break;
            case O_RET:
                int r;
                ensureInGPR(p->src1, &r);
                switch (p->dest->region) {
                    case R_LOCAL:
                        emit("\tmovq\t%s, -%d(%%rbp)", regs[r].name, p->dest->u.offset);
                        break;
                    case R_GLOBAL:
                        emit("\tmovq\t%s, %s(%%rip)", regs[r].name, p->dest->u.name);
                        break;
                    default:
                        break;
                }
                freeGPR(r);
                break;
            case O_ASN:
                int r;
                ensureInGPR(p->src1, &r);
                switch (p->dest->region) {
                    case R_LOCAL:
                        emit("\tmovq\t%s, -%d(%%rbp)", regs[r].name, p->dest->u.offset);
                        break;
                    case R_GLOBAL:
                        emit("\tmovq\t%s, %s(%%rip)", regs[r].name, p->dest->u.name);
                        break;
                    default:
                        break;
                }
                freeGPR(r);
                break;
            case O_BEQ: case O_BNE: case O_BLT:  case O_BLE:
            case O_BGT: case O_BGE:
                int r1, r2;
                ensureInGPR(p->src1, &r1);
                ensureInGPR(p->src2, &r2);
                emit("\tcmpq %s, %s", regs[r2].name, regs[r1].name);

                const char *mn =
                    (p->opcode == O_BEQ) ? "je"  :
                    (p->opcode == O_BNE) ? "jne" :
                    (p->opcode == O_BLT) ? "jl"  :
                    (p->opcode == O_BLE) ? "jle" :
                    (p->opcode == O_BGT) ? "jg"  : "jge";
                emit("\t%s\tL%d", mn, p->dest->u.offset);
                freeGPR(r1);
                freeGPR(r2);
                break;
            case O_ADD: case O_SUB: case O_MUL:
                int rd, r1, r2;
                ensureInGPR(p->dest, &rd);
                ensureInGPR(p->src1, &r1);
                ensureInGPR(p->src2, &r2);
                const char *op =
                    (p->opcode == O_ADD) ? "addq" :
                    (p->opcode == O_SUB) ? "subq" : "imulq";
                emit("\tmovq\t%s, %s", regs[r1].name, regs[rd].name);
                emit("\t%s\t%s, %s", op, regs[r2].name, regs[rd].name);
                spillAddr(p->dest);
                break;
            default:
                fprintf(stderr, "Unknown opcode %d in translateIcToAsm()\n", p->opcode);
                break;
        }
    }
    return 1;
}

/**
 *  @brief Write x86-64 assembly to file
 */
int writeAsm(FILE *fp){
    if (!fp) return 0;
    for (int i = 0; i < bufLen; i++){
        fprintf(fp, "%s\n", asmBuf[i]);
        free(asmBuf[i]);
    }
    free(asmBuf);
    return 1;
}
