#ifndef LEX_H
#define LEX_H
//included for definition of FILE
#include <stdio.h>

extern FILE *yyin;
extern int yylex();
extern int yyparse();
void yyerror (char const *s);
extern char *yytext;
extern int rows;
extern struct token *nextToken;
extern struct token *prevToken;
extern char *filename;


void unsupportedKeyword();
void unsupportedOperator();
void countNewLines();

int token(int code);
int leaf(int code);
int intLiteral(int code);
int longLiteral(int code);
int hexLiteral(int code);
int floatLiteral(int code);
int doubleLiteral(int code);
int stringLiteral(int code);
int multiLineString(int code);
int addSemi();
char *removeUnderscores();

struct token {
   int category;   /* the integer code returned by yylex */
   char *text;     /* the actual string (lexeme) matched */
   int lineno;     /* the line number on which the token occurs */
   char *filename; /* the source file in which the token occurs */
   int ival;       /* for integer constants, store binary value here */
   double dval;	   /* for real constants, store binary value here */
   char *sval;     /* for string constants, malloc space, de-escape, store */
                   /*    the string (less quotes and after escapes) here */
};

#endif
