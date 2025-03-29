#ifndef TREE_H
#define TREE_H


struct symTab;

struct tree
{
    int id; // id for dot.
    int prodrule;
    char *symbolname;
    int nkids;
    struct tree *kids[10];
    struct token *leaf;
    struct typeInfo *type;
};

typedef struct tree *nodeptr;
extern struct tree *root;
nodeptr new_tree_node(int prodrule, int nkids, ...);
int printNode(nodeptr t);
int printTree(nodeptr root, int depth);
struct tree *alctoken(int prodrule, char *symbolname, int nkids, ...);
extern int serial;
void buildSymTabs(struct tree *node, struct symTab *scope);
int checkExistance(struct tree *node, struct symTab *scope);
int verifyDeclared(struct tree *node, struct symTab *scope);
int addMathModule();
#endif