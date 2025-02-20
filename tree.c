#include "tree.h"

int printNode(nodeptr t){
    printf("%s, %d: %d\n", t->symbolname, t->prodrule, t->nkids);
    return 0;
}

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

int humanReadable(nodeptr t){

}