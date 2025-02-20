#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "k0gram.tab.h"
#include "lex.h"

char *filename;
char temp[100];

void openFile(char *name);

int main(int argc, char *argv[])
{
    //checks for the correct number of command line arguments
    if(argc < 2)
    {
        printf("Usage: ./k0 {filename1} {filename2} ...\n");
        exit(1);
    }

    //checks that the file name is legal and opens the file
    openFile(argv[1]);

    yyparse();

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