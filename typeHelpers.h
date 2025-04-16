#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H

#include "type.h"
#include "tree.h"
#include <stdarg.h>


typePtr lookupType(struct tree *n);
char *typeName(typePtr t);
int typeSize(struct typeInfo *type);
struct param* createParamFromTree(struct tree *paramNode);
struct param *buildfuncParams(int params, ...);
struct param *alcParams(char *paramName, int type);
typePtr determineReturnType(struct tree *r);
int extractArraySize(struct tree *size);

int typeEquals(typePtr type1, typePtr type2);
typePtr copyType(typePtr type);
void deleteType(typePtr type);

int countExprList(struct tree *exprList);
int getImportParamCount(const char *fnName);
#endif
