#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H

#include "type.h"
#include "tree.h"

typePtr lookupType(struct tree *n);
char *typeName(typePtr t);
int compatible(typePtr t1, typePtr t2);
struct param* createParamFromTree(struct tree *paramNode);
typePtr determineReturnType(struct tree *r);
int extractArraySize(struct tree *size);

#endif
