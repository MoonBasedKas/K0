#include "tree.h"
#include "lex.h"
#include "dot.h"

// TODO: add in yyname(int)

/**
 * @brief Writes a dot file from our generated graph
 * 
 * @param f 
 * @param r 
 * @return int 
 */
int print_graph(FILE *f, struct tree *r){
    fprintf(f, "digraph {\n"); 

    traverseGraph(f, r);

    fprintf(f, "}");
    return 0;
}


/**
 * @brief Writes a leaf's information.
 * 
 * @param f 
 * @param root 
 * @return int 
 */
int writeLeaf(FILE *f, struct tree *root){
    fprintf(f, "N%d[shape=box style=dotted label=\"%s\n text=%s\n lineno=%d\"];\n", root->id, "UpdateMe",
        root->leaf->text, root->leaf->lineno);

    return 0;
}

/**
 * @brief Writes the internal nodes information to properly display
 * 
 * @param f 
 * @param root 
 * @return int 
 */
int writeNode(FILE *f, struct tree *root){
    fprintf(f, "N%d[shape=box label=%s];\n", root->id, root->symbolname );

    return 0;
}

/**
 * @brief Traverses the graph and generates the dot transitions.
 * 
 * @param f 
 * @param root 
 * @return int 
 */
int traverseGraph(FILE *f, struct tree *root){

    if (root == NULL) return 0;

    fprintf(f, "N%d[shape=box];\n", root->id);

    if(root->nkids == 0) {
        writeLeaf(f, root);
        return 0; // child node
    } 

    writeNode(f, root);

    for (int i = 0; i < root->nkids; i++){
        fprintf(f, "N%d -> N%d\n;\n", root->id, root->kids[i]->id);
    }

    for (int i = 0; i < root->nkids; i++){
        traverseGraph(f, root->kids[i]);
    }

    return 0;
}