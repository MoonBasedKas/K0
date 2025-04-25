#include "tree.h"
#include "symTab.h"
#include "typeHelpers.h"
#include "typeCheck.h"
#include "lex.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>

extern int symError;

// prints error for unsupported keywords
void unsupportedKeyword()
{
    fprintf(stderr, "File: %s Line:%d The keyword %s is not supported by k0\n", filename, rows, yytext);
    exit(1);
}

// prints error for unsupported operators
void unsupportedOperator()
{
    fprintf(stderr, "File: %s Line:%d The operator %s is not supported by k0\n", filename, rows, yytext);
    exit(1);
}

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

// icode stuff
void debugICode(char *string, struct tree *node)
{
    while (node->nkids != 0)
    {
        node = node->kids[0];
    }

    printf("Name: %s Line %d, Debug Message: %s\n", node->leaf->text, node->leaf->lineno, string);
}