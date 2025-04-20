#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

void typeCheck(struct tree *node);
int ifAssigned(struct tree *node);
void prefixExpression(struct tree *node);
struct symEntry *returnType(struct tree *node);
void paramTypeCheck(struct tree *id, struct tree *exprList);
void checkImport(struct tree *import, struct tree *element, struct symTab *scope);
void leafExpression(struct tree *node);
void assignAddExpression(struct tree *node);
void assignSubExpression(struct tree *node);
void forStatement(struct tree * node);
void binaryExpression(struct tree *node);
void multaplicativeExpression(struct tree *node);
void subExpression(struct tree *node);
void addExpression(struct tree *node);
void inExpression(struct tree *node);
void returnCheck(struct tree *node, struct typeInfo *type);
void arrayDeclaration(struct tree *ident, struct tree *exprList);
void typeError(char *message, struct tree *node);
int typeMagicAssign(struct tree *left, struct tree *right);

#endif