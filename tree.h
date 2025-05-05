#ifndef TREE_H
#define TREE_H

struct symTab;
struct typeInfo;  // Forward declaration

struct tree
{
    int id; // id for dot.
    int prodrule;
    char *symbolname;
    int nkids;
    struct tree *parent;
    struct tree *kids[10];
    struct token *leaf;
    struct typeInfo *type;
    struct addr *addr;
    struct instr *icode;
    struct addr *first;
    struct addr *follow;
    struct addr *onTrue;
    struct addr *onFalse;
    int icodeDone;
    struct symTab *table; // Scope that the node exists within.
};

typedef struct tree *nodeptr;
extern struct tree *root;
nodeptr new_tree_node(int prodrule, int nkids, ...);
int printNode(nodeptr t);
int printTree(nodeptr root, int depth);
struct tree *alctoken(int prodrule, char *symbolname, int nkids, ...);
int assignMutability(struct tree *root);
int typeTheft(struct tree *root);
int varTypeTheft(struct tree *root);
void freeTree(nodeptr node);
void freeToken(struct token *tok);
int returnTheft(struct tree *node);
extern int serial;


#endif