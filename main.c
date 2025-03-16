#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "k0gram.tab.h"
#include "lex.h"
#include "dot.h"
#include "tree.h"
#include "symTab.h"


char *filename;
char temp[100];
extern int yydebug;
extern struct tree *root;
extern int printTree(struct tree *root, int depth);
extern void freeTree(struct tree *node);
extern int yylex_destroy(void);
void openFile(char *name);
void populateTypes();
void populateStdlib();
void populateLibraries();
extern int symError;

int main(int argc, char *argv[])
{

    int dot = 0; // False
    int tree = 0;
    int symtab = 0;
    int fileCount = 0;

    char **fileNames = malloc(sizeof(char *) * argc);
    if(fileNames == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++){
        if (!strcmp(argv[i], "-dot")){
            dot = 1;
        } else if (!strcmp(argv[i], "-tree")){
            tree = 1;
        } else if (!strcmp(argv[i], "-symtab")){
            symtab = 1;
        } else {
            // Treat non-flag arguments as file names.
            fileNames[fileCount++] = argv[i];
        }
    }

    if(fileCount == 0){
        printf("Usage: ./k0 [-dot] [-tree] [-symtab] {filename1} {filename2} ...\n");
        free(fileNames);
        exit(1);
    }

    for(int i = 0; i < fileCount; i++){

        root = NULL;
        rootScope = createTable(NULL, "global");
        populateTypes();
        populateStdlib();

        //checks that the file name is legal and opens the file
        openFile(fileNames[i]);

        //yydebug = 1;
        yyparse();
        buildSymTabs(root, rootScope);
        verifyDeclared(root, rootScope);

        if(dot){
            char dotFilename[120];
            snprintf(dotFilename, sizeof(dotFilename), "dotfile_%d.dot", i);
            FILE *out = fopen(dotFilename, "w");
            if(out){
                print_graph(out, root);
                fclose(out);
                printf("Dot file written: %s\n", dotFilename);
            } else {
                printf("Error writing dot file for %s\n", fileNames[i]);
            }
        }
        if (tree) {
            printTree(root, 0);
        }

        if(symtab){
            printTable(rootScope);
        }

        freeTable(rootScope);
        fclose(yyin);
        freeTree(root);
        yylex_destroy();
    }

    free(fileNames);
    return symError;
}

void openFile(char *name)
{
    // Save the original file name.
    filename = name;
    char *slash = strrchr(filename, '/');
    char *base = (slash == NULL) ? filename : slash + 1;

    // Check for an extension.
    char *dot = strrchr(base, '.');
    if(dot == NULL)
    {
        // No extension: create a new filename with ".kt" appended.
        strcpy(temp, filename);
        strcat(temp, ".kt");

        // If the original file exists, rename it to the new name.
        if(access(filename, F_OK) == 0)
        {
            if(rename(filename, temp) != 0)
            {
                perror("Error renaming file");
                exit(1);
            }
        }
        // Use the new filename.
        filename = temp;
    }
    else
    {
        // If an extension exists, allow only ".kt".
        if(strcmp(dot, ".kt") != 0)
        {
            printf("Input file must be of type .kt\n");
            exit(1);
        }
    }

    yyin = fopen(filename, "r");

    // Check that the file opened successfully.
    if(yyin == NULL)
    {
        printf("File %s cannot be opened.\n", filename);
        exit(1);
    }
}

/**
 * @brief Preloads the standard library.
 *
 */
void populateTypes(){
    // Types
    addSymTab(rootScope, "Int", NULL, VARIABLE);
    addSymTab(rootScope, "String", NULL, VARIABLE);
    addSymTab(rootScope, "Byte", NULL, VARIABLE);
    addSymTab(rootScope, "Short", NULL, VARIABLE);
    addSymTab(rootScope, "Long", NULL, VARIABLE);
    addSymTab(rootScope, "Float", NULL, VARIABLE);
    addSymTab(rootScope, "Boolean", NULL, VARIABLE);
    addSymTab(rootScope, "Double", NULL, VARIABLE);
    addSymTab(rootScope, "Array", NULL, VARIABLE);
}

void populateStdlib(){
    // Functions
    addSymTab(rootScope, "print", NULL, VARIABLE);
    addSymTab(rootScope, "println", NULL, VARIABLE);
    addSymTab(rootScope, "get", NULL, VARIABLE);
    addSymTab(rootScope, "equals", NULL, VARIABLE);
    addSymTab(rootScope, "length", NULL, VARIABLE);
    addSymTab(rootScope, "toString", NULL, VARIABLE);
    addSymTab(rootScope, "valueOf", NULL, VARIABLE);
    addSymTab(rootScope, "substring", NULL, VARIABLE);
    addSymTab(rootScope, "readln", NULL, VARIABLE);
}


/**
 * @brief Pre adds all libraries in k0
 *
 */
void populateLibraries(){
    //Predefined libraries
    addSymTab(rootScope, "java", NULL, VARIABLE);
    addSymTab(rootScope, "util", NULL, VARIABLE);
    addSymTab(rootScope, "lang", NULL, VARIABLE);
    addSymTab(rootScope, "math", NULL, VARIABLE);
    addSymTab(rootScope, "Random", NULL, VARIABLE);

    // Functions within predfined libraries.
    addSymTab(rootScope, "nextInt", NULL, VARIABLE);
    addSymTab(rootScope, "abs", NULL, VARIABLE);
    addSymTab(rootScope, "max", NULL, VARIABLE);
    addSymTab(rootScope, "min", NULL, VARIABLE);
    addSymTab(rootScope, "pow", NULL, VARIABLE);
    addSymTab(rootScope, "cos", NULL, VARIABLE);
    addSymTab(rootScope, "sin", NULL, VARIABLE);
    addSymTab(rootScope, "tan", NULL, VARIABLE);
}