#ifndef X86_64GEN_H
#define X86_64GEN_H
#include "tac.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define NUM_GPR 14
#define NUM_XMM 8

#define MAX_GPR_ARGS 6
extern const char *argRegs[MAX_GPR_ARGS];
extern const int argRegsIdx[MAX_GPR_ARGS];

struct regdescrip {
    char name[12];
    int status;    // 0=empty, 1=loaded(clean), 2=dirty
    struct addr *a;
};

extern struct regdescrip regs[NUM_GPR];
extern struct regdescrip xmmRegs[NUM_XMM];

struct addr_descrip {
    int status;       // 0=empty, 1=mem, 2=reg(clean), 3=mem&reg(synced), 4=reg(dirty)
    struct regdescrip *r;
    struct addr tac_addr;
    struct addr_descrip *next;
};

extern struct addr_descrip *addrMap;

// Initialization
void initCodeGen(struct tree *root);
void initRegs(void);
void initAddrDescrip(void);
void initStringLiterals(struct tree *root);

// Address Handling
struct addr_descrip *getAddrDesc(struct addr *a);
int isAddrEqual(struct addr *a1, struct addr *a2);
// Register Allocation
int getGPR(struct addr *for_addr);
void freeGPR(int idx);
int getXMM(void);
void freeXMM(int idx);

// Data Movement & Preparation
int ensureInGPR(struct addr *a, int *outReg, int assign_to_reg);
int ensureInXMM(struct addr *a, int *outReg);

// Register State Management
void spillAddr(struct addr *a);      // Spills based on TAC address
void spillReg(struct regdescrip *reg_d); // Spills a specific register
void markRegDirty(int regIdx);          // Marks a register as dirty

// Translation and Output
int translateIcToAsm(struct tree *root);
int writeAsm(const char *base_filename);

#endif