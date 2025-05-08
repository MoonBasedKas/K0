#ifndef x86_64gen_H
#define x86_64gen_H
#include "tac.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_GPR 14      // quads only dab
#define NUM_XMM 8       // percision regs

struct regdescrip {
    char name[12]; // name to use in codegen, e.g. "%rbx"
    int status;    // 0=empty, 1=loaded, 2=dirty, 4=live, ...
    struct addr addr;
};

// Register pools bros
extern struct regdescrip regs[NUM_GPR];
extern struct regdescrip xmmRegs[NUM_XMM];

// Address map/descriptor
struct addr_descrip {
    int status;    // 0=empty, 1=in memory, 2=in register, 3=both
    struct reg_descrip *r; // point at an elem in reg array. could use index.
    struct addr a;
};

// Address map/descriptor
extern struct addr_descrip *addrMap;

// init
void initRegs(void);
void initAddrDescrip(void);

// Addr lookup/create
struct addr_descrip *getAddrDesc(struct addr *a);

// Allocs
int getGPR(void);
void freeReg(int idx);
int getXMM(void);
void freeXMM(int idx);

/*
Make sure IR address is loaded, return register index
*/
int ensureInGPR(struct addr *a, int *outReg);
int ensureInXMM(struct addr *a, int *outReg);

// Register → memory
void spillAddr(struct addr *a);

// Translate and Write
int translateIcToAsm(void);
int writeAsm(FILE *fp);

#endif