#include "tree.h"
#include "lex.h"
#include "dot.h"
#include "k0gram.tab.h"

extern const char *yyname(int sym);
char *escape(const char *s);
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
 * @brief Writes a leaf's information. Change label for leaf name
 *
 * @param f
 * @param root
 * @param label
 * @return int
 */
int writeLeaf(FILE *f, struct tree *root){
    char *escapedText = escape(root->leaf->text);

    const char *tokenName = yyname(root->leaf->category);

    fprintf(f, "N%d[shape=box style=dotted label=\"%s\\n text=%s\\n lineno=%d\"];\n",
            root->id,
            tokenName,
            escapedText,
            root->leaf->lineno);
    free(escapedText);
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
    fprintf(f, "N%d[shape=box label=\"%s\"];\n", root->id, root->symbolname );

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
        fprintf(f, "N%d -> N%d;\n", root->id, root->kids[i]->id);
    }

    for (int i = 0; i < root->nkids; i++){
        traverseGraph(f, root->kids[i]);
    }

    return 0;
}

/**
 * @brief Escapes the text for the dot file.
 *
 * @param s
 * @return char*
 */
char *escape(const char *s) {
    if (s == NULL)
        return "";
    int len = strlen(s);
    // worst case: every character needs an escape (plus null terminator)
    char *result = malloc(len * 2 + 1);
    if (!result) {
        fprintf(stderr, "malloc failed in escape()\n");
        exit(1);
    }
    char *d = result;
    while (*s) {
        // escape quotes, curly braces, etc. (not sure what else is needed)
        if (*s == '"' || *s == '{' || *s == '}') {
            *d++ = '\\';
        }
        *d++ = *s++;
    }
    *d = '\0';
    return result;
}