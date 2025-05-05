#ifndef x86_64gen_H
#define x86_64gen_H
#include "tac.h"

struct regdescrip {
    char name[12]; // name to use in codegen, e.g. "%rbx"
    int status;    // 0=empty, 1=loaded, 2=dirty, 4=live, ...
    struct addr addr;
};
struct addr_descrip {
    int status;    // 0=empty, 1=in memory, 2=in register, 3=both
    struct reg_descrip *r; // point at an elem in reg array. could use index.
    struct addr a;
};


int getreg();
int instructionTranslate();




#endif