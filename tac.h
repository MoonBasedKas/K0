
#ifndef TAC_H
#define TAC_H

#include "symTab.h"

extern FILE *iTarget;
struct addr {
  int region;
  union {
  int offset;
  char *name;
  } u;
};

/* Regions: */
#define R_GLOBAL 2001 /* can assemble as relative to the pc */
#define R_LOCAL  2002 /* can assemble as relative to the ebp */
#define R_CLASS  2003 /* can assemble as relative to the 'this' register */
#define R_LABEL  2004 /* pseudo-region for labels in the code region */
#define R_CONST  2005 /* pseudo-region for immediate mode constants */
#define R_NAME   2006 /* pseudo-region for source names */
#define R_NONE   2007 /* pseudo-region for unused addresses */

struct instr {
   int opcode;
   struct addr *dest, *src1, *src2;
   struct instr *next;
};

enum {
  /* Opcodes, per lecture notes */
  O_ADD = 3001,
  O_SUB,
  O_MUL,
  O_DIV,
  O_MOD,
  O_NEG,
  O_AND,
  O_OR,
  O_XOR,
  O_NOT,
  O_ASN,
  O_ADDR,
  O_LCONT,
  O_SCONT,
  O_GOTO,
  O_BLT,
  O_BLE,
  O_BGT,
  O_BGE,
  O_BEQ,   //three address instr
  O_BNE,   //three address instr
  O_BIF,   //two address instr -compare hw to 0
  O_BNIF,  //two address instr -compare hw to 0
  O_PARM,
  O_CALL,
  O_RET,
  /* declarations/pseudo instructions */
  D_GLOB,
  D_PROC,
  D_LOCAL,
  D_LABEL,
  D_END,
  D_PROT  /* prototype "declaration" */
};

struct instr *genInstr(int, struct addr *, struct addr *, struct addr *);
struct instr *copyInstrList(struct instr *l);
struct instr *appendInstrList(struct instr *l1, struct instr *l2);
struct instr *concatInstrList(struct instr *, struct instr *);
char *regionName(int i);
char *opCodeName(int i);
char *pseudoName(int i);
struct addr *genLabel();
struct addr *genLocal(int size, struct symTab *scope);
struct addr *genConst(int val);
void tacPrint(struct instr *code);
void freeInstrList(struct instr *list);
#endif