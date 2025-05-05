#include "tree.h"
#include "lex.h"
#include "dot.h"
#include "k0gram.tab.h"
#include "type.h"
#include "tac.h"
#include "typeHelpers.h"
#include "errorHandling.h"

extern const char *yyname(int sym);
char *escape(const char *s);
// TODO: add in yyname(int)

/**
 * @brief Prints the icode address in the dot file
 * 
 * @param f
 * @param addr
 */
static void printAddrDot(FILE *f, struct addr *addr) {
    if (!addr) return;
    switch (addr->region) {
        case R_GLOBAL: fprintf(f, "global:%d", addr->u.offset); break;
        case R_LOCAL: fprintf(f, "local:%d", addr->u.offset); break;
        case R_CLASS: fprintf(f, "class:%d", addr->u.offset); break;
        case R_LABEL: fprintf(f, "label:%s", addr->u.name); break;
        case R_CONST: fprintf(f, "const:%d", addr->u.offset); break;
        case R_STRING:
            if (addr->u.name) {
                fprintf(f, "\"%s\"", addr->u.name);
            } else {
                fprintf(f, "str:%d", addr->u.offset);
            }
            break;
        case R_NAME:
            if (addr->u.name) {
                fprintf(f, "%s", addr->u.name);
            } else {
                fprintf(f, "name:%d", addr->u.offset);
            }
            break;
        case R_NONE: fprintf(f, "none"); break;
        default: fprintf(f, "unknown:%d", addr->u.offset); break;
    }
}

/**
 * @brief Appends the address to the dot file if it exists
 * 
 * @param f
 * @param a
 */
static void appendAddrDotLabel(FILE *f, struct addr *a) {
    if (!a) return;
    // inserting address hopefully
    fprintf(f, "\\n addr=");
    printAddrDot(f, a);
}

/**
 * @brief Color code our shit
 * 
 * @param t
 * @return const char*
 */
static const char *typeColor(typePtr t) {
    if (!t) return "lightgray";
    switch (t->basicType) {
      case INT_TYPE:      return "lightblue";
      case DOUBLE_TYPE:   return "lightcyan";
      case BOOL_TYPE:     return "lightgreen";
      case CHAR_TYPE:     return "pink";
      case STRING_TYPE:   return "lime";
      case ARRAY_TYPE:    return "goldenrod1";
      case FUNCTION_TYPE: return "crimson";
      case UNIT_TYPE:     return "darkkhaki";
      case RANGE_TYPE:    return "orange";
      default:            return "gray";
    }
}

/**
 * @brief Writes a dot file from our generated graph
 *
 * @param f
 * @param r
 * @return int
 */
int print_graph(FILE *f, struct tree *r){
    fprintf(f, "digraph G{\n");
    fprintf(f, "compound=true;\n");
    fprintf(f, "graph [nodesep=0.6 ranksep=0.8];\n");

    traverseGraph(f, r);

    fprintf(f, "}\n");
    return 0;
}

/**
 * @brief Writes the IR instructions to the dot file
 * 
 * @param f
 * @param head
 * @param parentId
 */
static void writeIR(FILE *f, struct instr *head, int parentId) {
    for (struct instr *p = head; p; p = p->next) {
        fprintf(f,
          "I%p [ shape=folder, color=blue, label=\"%s ",
          (void*)p,
          opCodeName(p->opcode)
        );
        if (p->dest)  { printAddrDot(f, p->dest); }
        if (p->src1)  { fprintf(f, ","); printAddrDot(f, p->src1); }
        if (p->src2)  { fprintf(f, ","); printAddrDot(f, p->src2); }
        fprintf(f, "\" ];\n");

        if (p->next) {
            fprintf(f,
              "I%p -> I%p [ style=bold ];\n",
              (void*)p, (void*)p->next
            );
        }
    }
    // link AST → first IR instr
    fprintf(f,
      "N%d -> I%p [ style=dashed, color=gray, constraint=false ];\n",
      parentId,
      (void*)head
    );
}

/**
 * @brief Writes the place node for the address
 * 
 * @param f
 * @param n
 */
static void writePlace(FILE *f, struct tree *n) {
    if (!n->addr) return;
    fprintf(f,
      "N%d_place [ shape=plaintext, color=blue, label=\"",
      n->id);
    printAddrDot(f, n->addr);
    fprintf(f,
      "\" ];\n"
      "N%d -> N%d_place [ style=dotted, color=blue ];\n",
      n->id, n->id);
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
    const char *typeStr = typeName(root->type);
    const char *fillColor = typeColor(root->type);
    fprintf(f,
            "N%d [\n"
            "shape=box,\n"
            "style=\"filled,dotted\",\n"
            "fillcolor=\"%s\",\n"
            "label=\"%s\\n"
            "text=%s\\n"
            "lineno=%d\\n"
            "type=%s",
            root->id,
            fillColor,
            tokenName,
            escapedText,
            root->leaf->lineno,
            typeStr);

    appendAddrDotLabel(f, root->addr);
    fprintf(f, "\"\n];\n");

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
    const char *typeStr = typeName(root->type);
    const char *fillColor = typeColor(root->type);
    fprintf(f,
            "N%d [\n"
            "shape=box,\n"
            "style=filled,\n"
            "fillcolor=\"%s\",\n"
            "label=\"%s\\n"
            "type=%s",
            root->id,
            fillColor,
            root->symbolname,
            typeStr);

    appendAddrDotLabel(f, root->addr);
    fprintf(f, "\"\n];\n");

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

    // What little icode we have, we print here... kinda
    writePlace(f, root);
    if (root->parent == NULL && root->icode) {
        writeIR(f, root->icode, root->id);
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