#ifndef TYPE_DECLARATION_H
#define TYPE_DECLARATION_H

void assignType(struct tree *n, struct symTab *scope);
void decTypes(struct tree *node, struct symTab *scope);
int checkNullability(struct tree *root);
int checkMutability(struct tree *root);
int hermitPurple(struct tree *node);
extern int symError;

#endif
