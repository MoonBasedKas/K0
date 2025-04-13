#include "tree.h"
#include "symTab.h"
#include "typeHelpers.h"
#include "typeCheck.h"
#include "lex.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>

extern int symError;

/**
 * @brief Prints type error message and sets symError to 1
 *
 * @param node
 */
void typeError(char *message, struct tree *node)
{
    while(node->nkids != 0)
    {
        node = node->kids[0];
    }

    fprintf(stderr, "Name: %s Line %d, Type Error: %s\n", node->leaf->text, node->leaf->lineno, message);
    symError = 1;
}

void yyerror (char const *s) {

    fprintf (stderr, "File: %s Line:%d %s - At token %s\n", nextToken->filename, nextToken->lineno, s, nextToken->text);
    exit(2);
}