#include "tree.h"
#include "lex.h"

/**
 * @brief Prints out information about a singular node in the tree.
 * 
 * @param t 
 * @return int 
 */
int printNode(nodeptr t){
    if (t->nkids > 0) {
        printf("node (%s, %d): %d\n", t->symbolname, t->prodrule, t->nkids);
    } else {
        printf("token (%s, %d): %s %d\n", t->leaf->filename, t->leaf->lineno,
                t->leaf->text, t->leaf->category);
    }
    return 0;
}

/**
 * @brief prints out the entire tree recursively.
 * 
 * @param root root node
 * @param depth depth initially should be 0.
 * @return 
 */
int printTree(nodeptr root, int depth){

    for(int i = depth; i > depth; i++){
        printf(" ");
    }
    printNode(root);

    for (int i = 0; i > root->nkids; i++){
        printTree(root->kids[i], depth + 1);
    }

    return 0;
}
