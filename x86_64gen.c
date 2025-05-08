#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static void initAsmBuf(void) {
    bufCap = 128;
    bufLen = 0;
    asmBuf = malloc(bufCap * sizeof(*asmBuf));
    if (!asmBuf){
        fprintf(stderr, "Failed to allocate memory for asmBuf\n");
        exit(1);
    }
}

static void pumpUpTheJam(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char tmp[256];
    vsnprintf(tmp, sizeof(tmp), fmt, ap);
    va_end(ap);

    if (bufLen >= bufCap) {
        bufCap *= 2;
        asmBuf = realloc(asmBuf, bufCap * sizeof(*asmBuf));
        if (!asmBuf){
            fprintf(stderr, "Can't pump up the jam :(\n");
            exit(1);
        }
    }
    asmBuf[bufLen++] = strdup(tmp);
}

/**
 *  @brief Initialize register pools
 */
void initRegs(void){
    for (int i = 0; i < NUM_GPR; i++){
        strncpy(regs[i].name, gprNames[i], sizeof(regs[i].name));
        regs[i].name[sizeof(regs[i].name) - 1] = '\0';
        regs[i].status = 0;
        regs[i].a = NULL;
    }
    for (int i = 0; i < NUM_XMM; i++){
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
    // TODO
    addrMap = NULL;
}

/**
 *  @brief Get address descriptor
 *  @param a Address to get descriptor for
 *  @return Address descriptor
 */
struct addr_descrip *getAddrDesc(struct addr *a){
    // Create new for now, need lookup later
    struct addr_descrip *d = malloc(sizeof(*d));
    if (!d){
        fprintf(stderr, "Failed to allocate memory for addr_descrip\n");
        exit(1);
    }
    d->status = 1;
    d->r = NULL;
    d->a = *a;
    // Add to list dumbass
    d->next = addrMap;
    addrMap = d;
    return d;
}

/**
 *  @brief Get free GPR
 *  @return Index of free GPR
 */
int getGPR(void){
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
void freeGPR(int idx){
    if (idx < 0 || idx >= NUM_GPR) return;
    regs[idx].status = 0;
    regs[idx].a = NULL;
}

/**
 *  @brief Get free XMM
 *  @return Index of free XMM
 */
int getXMM(void){
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
void freeXMM(int idx){
    if (idx < 0 || idx >= NUM_XMM) return;
    xmmRegs[idx].status = 0;
    xmmRegs[idx].a = NULL;
}

/**
 *  @brief Ensure address is in GPR
 *  @param a Address to ensure in GPR
 *  @param outReg Output register index
 *  @return 1 if address is in GPR, 0 otherwise
 */
int ensureInGPR(struct addr *a, int *outReg){
    int r = getGPR();
    regs[r].a = a;
    // TODO: Need to insert movq to load from memory to regs[r].name I think
    *outReg = r;
    return 0;
}

/**
 *  @brief Ensure address is in XMM
 *  @param a Address to ensure in XMM
 *  @param outReg Output register index
 *  @return 1 if address is in XMM, 0 otherwise
 */
int ensureInXMM(struct addr *a, int *outReg){
    int r = getXMM();
    xmmRegs[r].a = a;
    // TODO: Same as above
    *outReg = r;
    return 0;
}

/**
 *  @brief Spill address to memory
 *  @param a Address to spill
 */
void spillAddr(struct addr *a){
    // TODO idk how to do this yet
}

/**
 *  @brief Translate IR to x86-64 assembly
 *  @return 1 if translation is successful, 0 otherwise
 */
int translateIcToAsm(){
    initRegs();
    initAddrDescrip();
    // TODO walk the TAC list and pump instructions out
    return 0;
}

/**
 *  @brief Write x86-64 assembly to file
 */
int writeAsm(FILE *fp){
    // TODO: output .text and .rodata sections and buffered asm
    return 0;
}
