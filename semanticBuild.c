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
void assignType(struct tree *n){

    for (int i = 0; i < n->nkids; i++){
        assignType(n->kids[i]);
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
void decTypes(struct tree *node)
{
    switch (node->prodrule)
    {
        //function declarations w/ Types
        case funcDecAll: // @ 4
        case funcDecParamType: // @4
        case funcDecTypeBody: // @4
        case funcDecType: // @4
            node->type = alcFuncType(node->kids[3], node->kids[2], /** WHY? */ NULL);
            break;

        case varDec: // Variable declaration.
            // Two cases normal or array.
            
            break;

        
        

        default:
            node->type = alcType(ANY_TYPE); // NO ASSIGNED TYPE
            for(int i = 0; i < node->nkids; i++)
            {
                decTypes(node->kids[i]);
            }
            break;
    }

}


/**
 * @brief Determines what the type is
 * 
 * @param node 
 * @return int 
 */
int findType(struct tree *node){
    if(node->nkids != 0) return ANY_TYPE;

    if(node->leaf->category != IDENTIFIER) return ANY_TYPE;

    if(!strcmp(node->leaf->text, "Int")) return INT_TYPE;
    if(!strcmp(node->leaf->text, "Float")) return FLOAT_TYPE;
    if(!strcmp(node->leaf->text, "String")) return STRING_TYPE;
    if(!strcmp(node->leaf->text, "Boolean")) return BOOL_TYPE;
    if(!strcmp(node->leaf->text, "Char")) return CHAR_TYPE;
    if(!strcmp(node->leaf->text, "Byte")) return BYTE_TYPE;
    if(!strcmp(node->leaf->text, "Short")) return SHORT_TYPE;
    if(!strcmp(node->leaf->text, "Long")) return LONG_TYPE;
    if(!strcmp(node->leaf->text, "Double")) return DOUBLE_TYPE;
    if(!strcmp(node->leaf->text, "Null")) return NULL_TYPE;
    if(!strcmp(node->leaf->text, "Any")) return ANY_TYPE; // This shouldn't really happen

    return ANY_TYPE;
}