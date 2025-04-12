#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "symTab.h"
#include "tree.h"
#include "lex.h"
#include "k0gram.tab.h"
#include "symNonTerminals.h"

/**
 * @brief supported imports to check against
 * 
 */
static char *supportedImports[] = {
    "String.get",
    "String.equals",
    "String.length",
    "String.toString",
    "String.valueOf",
    "String.substring",
    "java.util.Random.nextInt",
    "java.lang.Math.abs",
    "java.lang.Math.max",
    "java.lang.Math.min",
    "java.lang.Math.pow",
    "java.lang.Math.cos",
    "java.lang.Math.sin",
    "java.lang.Math.tan"
};

static int numSupportedImports = sizeof(supportedImports) / sizeof(supportedImports[0]);

/**
 * @brief check if import is supported
 * 
 * @param fullPath 
 * @return int 
 */
static int isImportSupported(char *fullPath)
{
    for (int i = 0; i < numSupportedImports; i++) {
        if (strcmp(fullPath, supportedImports[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief gather import segments
 * 
 * @param node 
 * @param seg 
 * @param segCount 
 */
static void gatherImportSegments(struct tree *node, char **seg, int *segCount)
{
    if (!node) return;

    switch (node->prodrule)
    {
        case expandingImportID:
            /*
             * kids[0] => IDENTIFIER or another expandingImportID
             * kids[1] => DOT
             * kids[2] => importIdentifier
             */
            gatherImportSegments(node->kids[0], seg, segCount);
            gatherImportSegments(node->kids[2], seg, segCount);
            break;

        case IDENTIFIER:
            if (node->leaf && node->leaf->text) {
                seg[*segCount] = strdup(node->leaf->text);
                (*segCount)++;
            }
            break;

        case MULT:
            // Wildcard?
            break;

        default:
            // Just recurse on children if there's more structure
            for (int i = 0; i < node->nkids; i++) {
                gatherImportSegments(node->kids[i], seg, segCount);
            }
            break;
    }
}

/**
 * @brief build full import path
 * 
 * @param segments 
 * @param segCount 
 */
static char* buildFullImportPath(char **segments, int segCount)
{
    if (segCount == 0) return NULL;

    // Calculate total length needed
    int totalLen = 0;
    for (int i = 0; i < segCount; i++) {
        totalLen += strlen(segments[i]) + 1; // +1 for '.' or null
    }

    char *fullPath = (char*)malloc(totalLen + 1);
    if (!fullPath) {
        fprintf(stderr, "Error: out of memory in buildFullImportPath.\n");
        return NULL;
    }
    fullPath[0] = '\0';

    // Concatenate segments with '.'
    for (int i = 0; i < segCount; i++) {
        strcat(fullPath, segments[i]);
        if (i < segCount - 1) {
            strcat(fullPath, ".");
        }
    }

    return fullPath;
}

/**
 * @brief add segments to symbol table
 * 
 * @param segments 
 * @param segCount 
 * @param rootScope 
 */
static void addSegmentsToSymbolTable(char **segments, int segCount, struct symTab *rootScope)
{
    for (int i = 0; i < segCount; i++) {
        char *seg = segments[i];
        struct typeInfo *funcType = alcType(FUNCTION_TYPE);
        addSymTab(rootScope, seg, FUNCTION);
        assignEntrytype(rootScope, seg, funcType);
    }
}

/**
 * @brief process import identifier
 * 
 * @param importIdNode 
 * @param rootScope 
 */
void processImport(struct tree *importIdNode, struct symTab *rootScope)
{
    if (!importIdNode) return;

    char *seg[128];
    int segCount = 0;
    for (int i = 0; i < 128; i++) {
        seg[i] = NULL;
    }

    gatherImportSegments(importIdNode, seg, &segCount);
    if (segCount == 0) {
        return;
    }

    // Build full path (e.g., "java.util.Random.nextInt")
    char *fullPath = buildFullImportPath(seg, segCount);
    if (!fullPath) {
        // Memory error or empty
        return;
    }

    if (isImportSupported(fullPath)) {
        addSegmentsToSymbolTable(seg, segCount, rootScope);
    }
    else {
        // Possibly ignore or warn about unsupported import
        // fprintf(stderr, "Warning: unsupported import: %s\n", fullPath);
    }

    free(fullPath);
    for (int i = 0; i < segCount; i++) {
        free(seg[i]); // because we used strdup, this might be one of our valgrind leaks
    }
}

/**
 * @brief parse import list
 * 
 * @param importListNode 
 * @param rootScope 
 */
void parseImportList(struct tree* importListNode, struct symTab* rootScope)
{
    if (!importListNode) return;

    /*
     * kids[0] => IMPORT token
     * kids[1] => importIdentifier
     * kids[2] => importList (if present)
     */

    processImport(importListNode->kids[1], rootScope);

    if (importListNode->nkids == 3) {
        parseImportList(importListNode->kids[2], rootScope);
    }
}