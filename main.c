/*
    I don't trust putting comments in my kotlex.l file so I'm going to put my 
    thoughs here

    I organized my tokens based on the lists given in the k0 specification

    I print an error and exit at all tokens stated as illegal in the specifications and created 
    token structures for the ones I was unsure about

    I did not have time to do thorough testing of the non-trivial expressions 
    but will do that this weekend for lab 2 - I am sure some of them need some work
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "k0gram.tab.h"
#include "lex.h"

char *filename;
char temp[100];

void printList(struct tokenlist *head);
void deallocate(struct tokenlist *head);
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