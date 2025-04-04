#ifndef SEMANTICBUILD_H
#define SEMANTICBUILD_H



void assignType(struct tree *n, struct symTab *scope);
void decTypes(struct tree *node, struct symTab *scope);
int checkNullability(struct tree *root);
int checkMutability(struct tree *root);
extern int symError;

#endif
 