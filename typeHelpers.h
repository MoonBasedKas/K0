#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H

#include "type.h"
#include "tree.h"


typePtr lookupType(struct tree *n);
char *typeName(typePtr t);
struct param* createParamFromTree(struct tree *paramNode);
typePtr determineReturnType(struct tree *r);
int extractArraySize(struct tree *size);

int typeEquals(typePtr type1, typePtr type2);
typePtr copyType(typePtr type);
void deleteType(typePtr type);
void typeCheckExpression(struct tree *node);
void leafExpression(struct tree *node);
void binaryExpression(struct tree *node);

#endif
