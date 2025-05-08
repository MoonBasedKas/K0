#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "x86_64gen.h"

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

/**
 *  @brief Initialize register pools
 */
void initRegs(void){
    for (int i = 0; i < NUM_GPR; i++){
        strncpy(regs[i].name, gprNames[i], sizeof(regs[i].name));
        regs[i].name[sizeof(regs[i].name) - 1] = '\0';
        regs[i].status = 0;
        regs[i].addr = NULL;
    }
    for (int i = 0; i < NUM_XMM; i++){
        strncpy(xmmRegs[i].name, xmmNames[i], sizeof(xmmRegs[i].name));
        xmmRegs[i].name[sizeof(xmmRegs[i].name) - 1] = '\0';
        xmmRegs[i].status = 0;
        xmmRegs[i].addr = NULL;
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
    return d;
}

/**
 *  @brief Get free GPR
 *  @return Index of free GPR
 */
int getGPR(void){

}

/**
 *  @brief Free GPR
 *  @param idx Index of GPR to free
 */
void freeGPR(int idx){

}

/**
 *  @brief Get free XMM
 *  @return Index of free XMM
 */
int getXMM(void){

}

/**
 *  @brief Free XMM
 *  @param idx Index of XMM to free
 */
void freeXMM(int idx){

}

/**
 *  @brief Ensure address is in GPR
 *  @param a Address to ensure in GPR
 *  @param outReg Output register index
 *  @return 1 if address is in GPR, 0 otherwise
 */
int ensureInGPR(struct addr *a, int *outReg){

}

/**
 *  @brief Ensure address is in XMM
 *  @param a Address to ensure in XMM
 *  @param outReg Output register index
 *  @return 1 if address is in XMM, 0 otherwise
 */
int ensureInXMM(struct addr *a, int *outReg){

}

/**
 *  @brief Spill address to memory
 *  @param a Address to spill
 */
void spillAddr(struct addr *a){

}

/**
 *  @brief Translate IR to x86-64 assembly
 *  @return 1 if translation is successful, 0 otherwise
 */
int translateIcToAsm(){

}

/**
 *  @brief Write x86-64 assembly to file
 */
int writeAsm(FILE *fp){

}
