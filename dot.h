
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int traverseGraph(FILE *f, struct tree *root);
int print_graph(FILE *f, struct tree *r);
int writeNode(FILE *f, struct tree *root);
int writeLeaf(FILE *f, struct tree *root);
