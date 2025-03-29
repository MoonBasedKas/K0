#ifndef SYMTABHELPER_H
#define SYMTABHELPER_H

int addMathModule();
void populateTypes();
void populateStdlib();
void populateLibraries();
void buildSymTabs(struct tree *node, struct symTab *scope);
char *getTableType(int type);
int verifyDeclared(struct tree *node, struct symTab *scope);
int checkExistance(struct tree *node, struct symTab *scope);


#endif
