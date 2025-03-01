#include "tree.h"
#include "lex.h"
#include "dot.h"

// TODO: add in yyname(int)

int print_graph(FILE *f, struct tree *r){
    fprintf(f, "digraph {\n"); // Write initalizer

    traverseGraph(f, r);

    fprintf(f, "}"); // Write finalizer
    return 0;
}


int writeLeaf(FILE *f, struct tree *root){
    
}

int traverseGraph(FILE *f, struct tree *root){

    if (root == NULL) return 0;

    fprintf(f, "N%d[shape=box];\n", root->id);

    if(root->nkids == 0) return 0; // child node

    for (int i = 0; i < root->nkids; i++){
        fprintf(f, "N%d -> N%d\n;\n", root->id, root->kids[i]->id);
    }

    for (int i = 0; i < root->nkids; i++){
        traverseGraph(f, root->kids[i]);
    }

    return 0;
}