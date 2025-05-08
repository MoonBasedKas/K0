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

void initAddrDescrip(void){
    // TODO
    addrMap = NULL;
}

struct addr_descrip *getAddrDesc(struct addr *a){
    // Create new for now, need lookup later
    return d;
}

int getGPR(void){

}

void freeGPR(int idx){

}

int getXMM(void){

}

void freeXMM(int idx){

}

int ensureInGPR(struct addr *a, int *outReg){

}

int ensureInXMM(struct addr *a, int *outReg){

}

void spillAddr(struct addr *a){

}

int translateIcToAsm(){

}

int writeAsm(FILE *fp){

}
