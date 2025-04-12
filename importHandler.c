#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "symTab.h"
#include "tree.h"
#include "lex.h"
#include "symNonTerminals.h"

typedef enum {
    LIB_UNKNOWN,
    LIB_
} ImportType;

/**
 * @brief Processes an import statement.
 * 
 * @param n 
 * @param rootScope 
 */
void processImport(struct tree *node, struct symTab *rootScope) {
    switch(node->prodrule) {
        case collapsedImport:
            // collapsedImport: IMPORT importIdentifier
            // n->kids[1] is the importIdentifier node.
            addImportIdentifier(node->kids[1], rootScope);
            break;
        case expandingImport:
            // expandingImport: IMPORT importIdentifier importList
            // n->kids[1] is the current importIdentifier.
            // n->kids[2] is the rest of the import list.
            addImportIdentifier(node->kids[1], rootScope);
            processImport(node->kids[2], rootScope);
            break;
        default:
            fprintf(stderr, "processImport: Unexpected production rule %d\n", n->prodrule);
            break;
    }
}