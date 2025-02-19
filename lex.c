#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lex.h"
#include "k0gram.tab.h"

//prints error for unsupported keywords
void unsupportedKeyword()
{
	printf("File: %s Line:%d The keyword %s is not supported by k0\n", filename, rows, yytext);
    exit(1);
}

//prints error for unsupported operators
void unsupportedOperator()
{
	printf("File: %s Line:%d The operator %s is not supported by k0\n", filename, rows, yytext);
    exit(1);
}

//counts the number of newlines in yytext and adds them to rows
//used with multiline comments and strings
void countNewLines()
{
    for(int i = 0; i < strlen(yytext); i++)
    {
        if(yytext[i] == '\n')
        {
            rows++;
        }
    }
}

//allocates a token struct and fills all fields except literal values
int token(int code)
{
    nextToken = (struct token*)malloc(sizeof(struct token));

    if(nextToken == NULL)
    {
        printf("Malloc failed\n");
        exit(1);
    }

    nextToken->category = code;
    nextToken->text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(nextToken->text == NULL)
    {
        printf("Malloc failed\n");
        exit(1);
    }
    strcpy(nextToken->text, yytext);
    nextToken->lineno = rows;
    nextToken->filename = (char*) malloc(sizeof(char) * (strlen(filename) + 1));
    if(nextToken->filename == NULL)
    {
        printf("Malloc failed\n");
        exit(1);
    }
    strcpy(nextToken->filename, filename);
    nextToken->sval = NULL;

    return code;
}

// creates a token then fills the ival field
int intLiteral(int code)
{
    token(code);

    nextToken->ival = atoi(nextToken->text);

    return code;
}

// discards the L at the end of a long then creates an integer token
int longLiteral(int code)
{
    int len = strlen(yytext);
    yytext[len-1] = '\0';
    intLiteral(code);

    return code;
}

// creates a token then fills the ival field
int hexLiteral(int code)
{
    token(code);

    sscanf(nextToken->text, "%x", &(nextToken->ival));

    return code;
}

// discards the f at the end of a float then creates an double token
int floatLiteral(int code)
{
    int len = strlen(yytext);
    yytext[len-1] = '\0';
    doubleLiteral(code);

    return code;
}

// creates a token then fills the dval field
int doubleLiteral(int code)
{
    token(code);

    sscanf(nextToken->text, "%lf", &(nextToken->dval));

    return code;
}

// allocates memory for sval the copies the contents of yytext into it
// removing the start and end quotes and translating escape sequences into their values
int stringLiteral(int code)
{
    token(code);

    nextToken->sval = (char *) malloc(sizeof(char) * (strlen(nextToken->text) + 1));

    if(nextToken->sval == NULL)
    {
        printf("Malloc failed\n");
        exit(1);
    }

    char *nextCharLex = nextToken->text;
    char *nextCharLit = nextToken->sval;

    //skip intial "
    nextCharLex++;

    while(*nextCharLex != '"')
    {
        // if not the start of escape sequence transfer letter
        if(*nextCharLex != '\\')
        {
            *nextCharLit = *nextCharLex;
        }
        // if escape sequence replace with correct value
        else
        {
            nextCharLex++;
            switch (*nextCharLex) 
            {
            case 'n':
                *nextCharLit = '\n';
                break;
            case 't':
                *nextCharLit = '\t';
                break;
            case 'r':
                *nextCharLit = '\r';
                break;
            case 'b':
                *nextCharLit = '\b';
                break;
            case '0':
                *nextCharLit = '\0';
                break;
            case '\\':
                *nextCharLit = '\\';
                break;
            case '\'':
                *nextCharLit = '\'';
                break;
            case '"':
                *nextCharLit = '"';
                break;
            case '$':
                *nextCharLit = '$';
                break;
            default:
                printf("File: %s Line:%d The escape sequence \\%c is not supported by k0\n", nextToken->filename, nextToken->lineno, *nextCharLex);
                exit(1);
                break;
            }
        }
        nextCharLex++;
        nextCharLit++;
    }
    
    //don't forget the null character
    *nextCharLit = '\0';

    return code;
}

// removes the triple quotes at the front and end of the string
// i checked in kotlin and multiline strings don't have escape sequences
// so just copy the rest of the string
int multiLineString(int code)
{
    token(code);
    
    nextToken->sval = (char *) malloc(sizeof(char) * (strlen(nextToken->text) + 1));

    if(nextToken->sval == NULL)
    {
        printf("Malloc failed\n");
        exit(1);
    }

    char *nextCharLex = nextToken->text;
    char *nextCharLit = nextToken->sval;

    int len = strlen(nextToken->text);

    //copy string without the triple quotes
    for(int i = 3; i < len - 3; i++)
    {
        *nextCharLit = nextCharLex[i];
        nextCharLit++;
    }
    //don't forget to close the string
    *nextCharLit = '\0';
    

    return code;
}

void yyerror (char const *s) {
    fprintf (stderr, "%s\n", s);
}


int addSemi(){
    switch(nextToken->category){
        case INTEGER_LITERAL:
        case HEX_LITERAL:
        case REAL_LITERAL:
        case CHARACTER_LITERAL:
        case IDENTIFIER:
        case LINE_STRING:
        case BREAK:
        case CONTINUE:
        case RETURN:
        case MULTILINE_STRING:
        case INCR:
        case DECR:
        case RSQUARE:
        case RPAREN:
        case RCURL: 
            yytext = ";";
            return 1; // True
        default: 
            return 0; // False
    }
}