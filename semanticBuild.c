/*
This file will have helper functions for the semantic analysis phase.

TODO: WHAT GOES WHERE BRO
*/

#include <stdio.h>
#include "tree.h"
#include "type.h"
#include "symTab.h"
#include "k0gram.tab.h"

/**
 * @brief Assigns a type to a node
 * 
 * @param node 
 */
void assignType(struct tree *n){
    if (n == NULL) return;

    for (int i = 0; i < n->nkids; i++){
        assignType(n->kids[i]);
    }

    switch (n->prodrule){
        case INT:
            n->type = alcType(INT_TYPE);
            break;
        case FLOAT:
            n->type = alcType(FLOAT_TYPE);
            break;
        case STRING:
            n->type = alcType(STRING_TYPE);
            break;
        case BOOL:
            n->type = alcType(BOOL_TYPE);
            break;
        case CHAR:
            n->type = alcType(CHAR_TYPE);
            break;
        case BYTE:
            n->type = alcType(BYTE_TYPE);
            break;
        case SHORT:
            n->type = alcType(SHORT_TYPE);
            break;
        case LONG:
            n->type = alcType(LONG_TYPE);
            break;
        case DOUBLE:
            n->type = alcType(DOUBLE_TYPE);
            break;
        case NULL:
            n->type = alcType(NULL_TYPE);
            break;
        case 
            
    }
}