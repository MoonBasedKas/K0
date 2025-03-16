#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "k0gram.tab.h"
#include "lex.h"
#include "dot.h"
#include "tree.h"
#include "symTab.h"


char *filename;
char temp[100];
//extern int yydebug;
extern struct tree *root;
extern int printTree(struct tree *root, int depth);
extern void freeTree(struct tree *node);
extern int yylex_destroy(void);
void openFile(char *name);

int main(int argc, char *argv[])
{

    int dot = 0; // False
    // TODO: Figure out weird behavior with ./*
    for (int i = 1; i < argc; i++){
        if (!strcmp(argv[i], "-dot")){
            dot = 1; // True!
        } else{
            filename = argv[i];
        }
    }

    if(filename == NULL) // Zero files
    {
        printf("Usage: ./k0 [-dot] {filename1} {filename2} ...\n");
        exit(1);
    }


    //checks that the file name is legal and opens the file
    openFile(filename);

    //yydebug = 1;
    yyparse();
    buildSymTabs(root, 0x0);
    if(dot){ // Dotting away.
        FILE *out = fopen("dotfile.dot", "w");
        print_graph(out, root);
        fclose(out);
        return 0;
    } else {

        printTree(root, 0);
    }
    printTable(rootScope);
    freeTable(rootScope);
    fclose(yyin);
    freeTree(root);

    yylex_destroy();
    return 0;
}

void openFile(char *name)
{
    //saves the file name for storing in tokens
    filename = name;

    //checks for .kt extension
    char *dot = strrchr(filename, '.');
    if(dot == NULL)
    {
        //if no extension adds .kt
        strcpy(temp, filename);
        strcat(temp, ".kt");
        filename = temp;
    }
    else
    {
        //if different extension rejects file
        if(strcmp(dot, ".kt") != 0)
        {
            printf("Input file must be of type .kt\n");
            exit(1);
        }
    }

    yyin = fopen(filename, "r");

    //checks that file opened sucesfully
    if(yyin == NULL)
    {
        printf("File %s cannot be opened.\n", filename);
        exit(1);
    }
}
