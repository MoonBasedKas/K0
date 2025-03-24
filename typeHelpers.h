#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H

#include "type.h"
#include "tree.h"

// Lookup the type of a node
typePtr lookupType(struct tree *n);

// Get the name of a type
char *typeName(typePtr t);

// Check if two types are compatible
int compatible(typePtr t1, typePtr t2);

#endif
