#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

void typeCheck(struct tree *node);
int ifAssigned(struct tree *node);
void prefixExpression(struct tree *node);
struct symEntry *returnType(struct tree *node);
void paramTypeCheck(struct tree *node);
void checkImport(struct tree *import, struct tree *element);
void leafExpression(struct tree *node);
void assignAddExpression(struct tree *node);
void assignSubExpression(struct tree *node);
void forStatement(struct tree * node);
void binaryExpression(struct tree *node);
void multaplicativeExpression(struct tree *node);
void subExpression(struct tree *node);
void addExpression(struct tree *node);
void inExpression(struct tree *node);
void typeError(char *message, struct tree *node);

#endif