#ifndef TREE_H
#define TREE_H

struct tree {
    int prodrule;
    char* symbolname;
    int nkids;
    struct tree *kids[10]; // idk  how many kids we need
    struct token *leaf; // if nkids == 0, null for epsilon
};

typedef struct tree *nodeptr;
nodeptr new_tree_node(int prodrule, int nkids, ...);

#endif