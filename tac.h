/*
 * Three Address Code - skeleton for CSE 423
 */
#ifndef TAC_H
#define TAC_H

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
/* Opcodes, per lecture notes */
#define O_ADD   3001
#define O_SUB   3002
#define O_MUL   3003
#define O_DIV   3004
#define O_NEG   3005
#define O_ASN   3006
#define O_ADDR  3007
#define O_LCONT 3008
#define O_SCONT 3009
#define O_GOTO  3010
#define O_BLT   3011
#define O_BLE   3012
#define O_BGT   3013
#define O_BGE   3014 
#define O_BEQ   3015 //three address instr
#define O_BNE   3016 //three address instr
#define O_BIF   3017 //two address instr -compare hw to 0
#define O_BNIF  3018 //two address instr -compare hw to 0
#define O_PARM  3019
#define O_CALL  3020
#define O_RET   3021
/* declarations/pseudo instructions */
#define D_GLOB  3051
#define D_PROC  3052
#define D_LOCAL 3053
#define D_LABEL 3054
#define D_END   3055
#define D_PROT  3056 /* prototype "declaration" */

struct instr *genInstr(int, struct addr *, struct addr *, struct addr *);
struct instr *copyInstrList(struct instr *l);
struct instr *appendInstrList(struct instr *l1, struct instr *l2);
struct instr *concatInstrList(struct instr *, struct instr *);
char *regionName(int i);
char *opCodeName(int i);
char *pseudoName(int i);
struct addr *genLabel();
struct addr *genLocal(int size, struct symTab *scope);
void tacPrint(struct instr *code);
void freeInstrList(struct instr *list);
#endif