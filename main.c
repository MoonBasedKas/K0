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
#include "type.h"
#include "typeDeclaration.h"
#include "typeCheck.h"
#include "symTabHelper.h"
#include "errorHandling.h"
#include "tac.h"
#include "icode.h"
#include "x86_64gen.h"

char *filename;
char temp[100];
extern int yydebug;
extern struct tree *root;
extern int printTree(struct tree *root, int depth);
extern void freeTree(struct tree *node);
extern int yylex_destroy(void);
extern FILE *iTarget;
void openFile(char *name);
FILE *openGenFile(char *name, char *ext);
void populateTypes();
void populateStdlib();
char *getFileName(char *f);
char *addExt(char *f, char *ext);
void populateLibraries();
int symError = 0;

int main(int argc, char *argv[])
{
    char *dio = malloc(4096);
    char *important = NULL;

    int dot = 0; // False
    int tree = 0;
    int symtab = 0;
    int fileCount = 0;
    int debug = 0;
    int c = 0;
    //int ic = 0;
    int s = 0;
    char **fileNames = malloc(sizeof(char *) * argc);
    if (fileNames == NULL)
    {
        fprintf(stderr, "Memory allocation error.\n");
        exit(1);
    }

    // The great cmd argument obtainer.
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-dot"))
        {
            dot = 1;
        }
        else if (!strcmp(argv[i], "-tree"))
        {
            tree = 1;
        }
        else if (!strcmp(argv[i], "-symtab"))
        {
            symtab = 1;
        }
        else if (!strcmp(argv[i], "-debug"))
        {
            debug = 1;
        } /*
        else if (!strcmp(argv[i], "-ic")){
            ic = 1;
        }*/
        else if (!strcmp(argv[i], "-s"))
        {
            s = 1;
        }
        else if (!strcmp(argv[i], "-c"))
        {
            c = 1;
        }
        else if (!strcmp(argv[i], "-help"))
        {
            printf("Usage ./k0 [option]|[file] ...\n");
            printf("Options:\n");
            printf("-help: Prints out this command interface.\n");
            printf("-dot: Generates a dot file, needs to be compiled.\n");
            printf("-tree: View the syntax tree of the program.\n");
            printf("-symtab: View the symbol tables.\n");
            printf("-debug: One does not reveal what their debug command does.\n");
        }
        else
        {
            // Treat non-flag arguments as file names.
            fileNames[fileCount++] = argv[i];
        }
    }

    if (fileCount == 0)
    {
        fprintf(stderr, "Usage: ./k0 [-dot] [-tree] [-symtab] {filename1} {filename2} ...\n");
        free(fileNames);
        exit(1);
    }

    for (int i = 0; i < fileCount; i++)
    {

        root = NULL;
        rootScope = createTable(NULL, "global", PACKAGE);
        populateTypes();
        populateStdlib(); // symTabHelper.c

        // checks that the file name is legal and opens the file
        openFile(fileNames[i]);

        // yydebug = 1;
        yyparse();

        // Semantic analysis
        buildSymTabs(root, rootScope); // symTabHelper.c
        giveTables(root);
        if (debug == 1)
            findNullTables(root);
        verifyDeclared(root, rootScope); // symTabHelper.c
        struct symEntry *x = NULL;
        if ((x = contains(rootScope, "nextInt")) != NULL)
            x->type = alcType(INT_TYPE);
        if (symError != 0 && debug == 0)
            return 3;
        assignMutability(root);
        assignType(root, rootScope); // typeDeclaration.c
        // The bottom two could probably be one function, but more tree traversals is better!
        varTypeTheft(root);
        typeTheft(root);
        returnTheft(root);
        typeCheck(root);
        checkNullability(root);
        checkMutability(root);
        verifyDeclared(root, rootScope); // symTabHelper.c
        buildICode(root);

        if (symError != 0 && debug == 0)
            return 3; // If something is undeclared.

        if (dot)
        {
            char dotFilename[120];
            snprintf(dotFilename, sizeof(dotFilename), "dotfile_%d.dot", i);
            FILE *out = fopen(dotFilename, "w");
            if (out)
            {
                print_graph(out, root);
                fclose(out);
                printf("Dot file written: %s\n", dotFilename);
            }
            else
            {
                fprintf(stderr, "Error writing dot file for %s\n", fileNames[i]);
            }
        }
        if (tree)
        {
            printTree(root, 0); // tree.c
        }

        if (symtab)
        {
            printTable(rootScope); // symTab.c
        }
        else if (symError == 0)
        {
            printf("No errors in file: %s\n\n", fileNames[i]);
        }

        iTarget = openGenFile(fileNames[i], "ic");
        tacPrint(root->icode);
        if (iTarget != NULL)
            fclose(iTarget);

        translateIcToAsm(root);
        writeAsm(fileNames[i]);

        // The great file generator.
        important = getFileName(fileNames[i]);
        if (s)
        { // Generates .s and stop
            char asm_filename_temp[256];
            char base_filename_temp[254];

            // strncpy and snprintf hate each other
            strncpy(base_filename_temp, fileNames[i], sizeof(base_filename_temp) - 1);
            base_filename_temp[sizeof(base_filename_temp) - 1] = '\0';

            char *dot_ptr_temp = strrchr(base_filename_temp, '.');
            if (dot_ptr_temp) *dot_ptr_temp = '\0';

            // strlen(base_filename_temp) is at most 253.
            // So base_filename_temp + ".s" + '\0' will be at most 253 + 2 + 1 = 256 bytes
            // fuck off
            snprintf(asm_filename_temp, sizeof(asm_filename_temp), "%s.s", base_filename_temp);
            printf("Assembly file %s generated. Stopping as per -s option.\n", asm_filename_temp);

            fclose(yyin);
            freeTree(root);
            freeTable(rootScope);
            yylex_destroy();
            continue;
        }
        else if (c)
        { // Generates .o
            memset(dio,0,4096);
            strcpy(dio, "as --gstabs+ -o ");
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".o "));
            free(important);
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".s"));
            free(important);
            system(dio);

            char base_filename_msg_c[256];
            strncpy(base_filename_msg_c, fileNames[i], sizeof(base_filename_msg_c) - 1);
            base_filename_msg_c[sizeof(base_filename_msg_c) - 1] = '\0';
            char *dot_ptr_base_msg_c = strrchr(base_filename_msg_c, '.');
            if (dot_ptr_base_msg_c) *dot_ptr_base_msg_c = '\0';


            memset(dio,0,4096);
            strcpy(dio, "rm ");
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".s"));
            free(important);
            system(dio);
        }
        else
        { // Generates executable.
            printf("Assembling and Linking (to executable)...\n");
            memset(dio,0,4096);
            strcpy(dio, "as --gstabs+ -o ");
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".o "));
            free(important);
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".s"));
            free(important);
            printf("Executing: %s\n", dio);
            system(dio);

            memset(dio,0,4096);
            strcpy(dio, "gcc ");
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".o"));
            strcat(dio, " -o ");
            free(important);
            important = getFileName(fileNames[i]);
            strcat(dio, important);
            free(important);
            printf("Executing: %s\n", dio);
            system(dio);

            memset(dio,0,4096);
            strcpy(dio, "rm ");
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".s"));
            free(important);
            system(dio);

            memset(dio,0,4096);
            strcpy(dio, "rm ");
            important = getFileName(fileNames[i]);
            strcat(dio, addExt(important, ".o"));
            free(important);
            system(dio);

            char base_filename_msg_exe[256];
            strncpy(base_filename_msg_exe, fileNames[i], sizeof(base_filename_msg_exe) - 1);
            base_filename_msg_exe[sizeof(base_filename_msg_exe) - 1] = '\0';
            char *dot_ptr_base_msg_exe = strrchr(base_filename_msg_exe, '.');
            if (dot_ptr_base_msg_exe) *dot_ptr_base_msg_exe = '\0';
            printf("Executable generation process complete for %s\n", base_filename_msg_exe);
        }

        freeTable(rootScope); // symTab.c
        fclose(yyin);
        freeTree(root); // tree.c
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
    if (dot == NULL)
    {
        // No extension: create a new filename with ".kt" appended.
        strcpy(temp, filename);
        strcat(temp, ".kt");

        // If the original file exists, rename it to the new name.
        if (access(filename, F_OK) == 0)
        {
            if (rename(filename, temp) != 0)
            {
                fprintf(stderr, "Error renaming file");
                exit(1);
            }
        }
        // Use the new filename.
        filename = temp;
    }
    else
    {
        // If an extension exists, allow only ".kt".
        if (strcmp(dot, ".kt") != 0)
        {
            fprintf(stderr, "Input file must be of type .kt\n");
            exit(1);
        }
    }
    // Write out the name of the file to standard out.
    printf("Opening file: %s\n", filename);
    yyin = fopen(filename, "r");

    // Check that the file opened successfully.
    if (yyin == NULL)
    {
        fprintf(stderr, "File %s cannot be opened.\n", filename);
        exit(1);
    }
}

/**
 * @brief A more general way of writing to files
 *
 * @param name
 * @param ext
 * @param f
 */
FILE *openGenFile(char *name, char *ext)
{
    /* build output filename */
    char *n = malloc(strlen(name) + strlen(ext) + 2);
    if (!n)
    {
        perror("malloc");
        exit(1);
    }
    strcpy(n, name);
    char *dot = strrchr(n, '.');
    if (dot)
        *dot = '\0';
    strcat(n, ".");
    strcat(n, ext);

    FILE *f = fopen(n, "w");
    if (!f)
    {
        perror("opening generated file");
        exit(1);
    }
    free(n);
    return f;
}

/**
 * @brief Generates a file name with a given extension.
 *
 * @param f
 * @param ext
 * @return char*
 */
char *getFileName(char *f)
{
    char *fName = strdup(f);
    char *violation = NULL;
    char *result = malloc(4096);
    violation = strrchr(fName, '.');
    if(violation)
        *(violation) = '\0';
    strcpy(result, fName);
    return result;
}

char *addExt(char *f, char *ext){
    return strcat(f, ext);
}