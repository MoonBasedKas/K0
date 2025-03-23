/*
This file will have helper functions for the semantic analysis phase.


*/

#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "type.h"
#include "lex.h"
#include "symTab.h"
#include "symNonTerminals.h"
#include "k0gram.tab.h"
#include "semanticBuild.h"

/**
 * @brief Assigns a type to a node. To be used later.
 * 
 * @param node 
 */
void assignType(struct tree *n, struct symTab *scope){

    for (int i = 0; i < n->nkids; i++){
        assignType(n->kids[i], scope);
    }
    // This is a step below.
    switch (n->prodrule){
        case INT_TYPE:
            n->type = alcType(INT_TYPE);
            break;
        case FLOAT_TYPE:
            n->type = alcType(FLOAT_TYPE);
            break;
        case STRING_TYPE:
            n->type = alcType(STRING_TYPE);
            break;
        case BOOL_TYPE:
            n->type = alcType(BOOL_TYPE);
            break;
        case CHAR_TYPE:
            n->type = alcType(CHAR_TYPE);
            break;
        case BYTE_TYPE:
            n->type = alcType(BYTE_TYPE);
            break;
        case SHORT_TYPE:
            n->type = alcType(SHORT_TYPE);
            break;
        case LONG_TYPE:
            n->type = alcType(LONG_TYPE);
            break;
        case DOUBLE_TYPE:
            n->type = alcType(DOUBLE_TYPE);
            break;
        case NULL_TYPE:
            n->type = alcType(NULL_TYPE);
            break;
        default:
            // Replace w/ a user defined.
            n->type = alcType(ANY_TYPE);
            break; 
            
    }

}


/**
 * @brief Wrapper function to learn the types.
 * 
 * @param node 
 */
void decTypes(struct tree *node, struct symTab *scope)
{
    switch (node->prodrule)
    {
        //function declarations w/ Types
        case funcDecAll: // @ 4
        case funcDecParamType: // @4
        case funcDecTypeBody: // @4
        case funcDecType: // @4
            scope = scope->buckets[hash(node->kids[0]->leaf->text)]->scope;
            node->type = alcFuncType(node->kids[3], node->kids[2], scope);
            break;

        case varDec: 
            // Bingo
            node->type = alcType(findType(node->kids[1]));
            break;

        case arrayDec:
        case arrayDecValueless:
        case arrayDecEqual:
        case arrayDecEqualValueless:
            node->type = alcArrayType(NULL, NULL);
            break;

        default:
            node->type = alcType(ANY_TYPE); // NO ASSIGNED TYPE
            for(int i = 0; i < node->nkids; i++)
            {
                decTypes(node->kids[i], scope);
            }
            break;
    }

}


