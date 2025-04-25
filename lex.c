#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "lex.h"
#include "tree.h"
#include "k0gram.tab.h"
#include "errorHandling.h"

int saw_newline = 0;
int inserted = 0;
static int lastToken = 0;
static int savedToken = 0;
static int blightToken = 0; // A blight upon my day.
static YYSTYPE saved_lval, last_lval, magic_lval;

/**
 * @brief Counts the number of newlines in yytext and adds them to rows
 *
 * @return void
 */
void countNewLines()
{
    for (int i = 0; i < strlen(yytext); i++)
    {
        if (yytext[i] == '\n')
        {
            rows++;
        }
    }
}

/**
 * @brief Frees tokens implied by
 *
 * @param targets
 * @param ...
 * @return int
 */
int freeTokens(int targets, ...)
{
    va_list args;
    struct token *temp;
    va_start(args, targets);
    for (int i = 0; i < targets; i++)
    {
        temp = va_arg(args, struct token *);
        free(temp->text);
        free(temp);
    }
    va_end(args);

    return 0;
}

/**
 * @brief Allocates a token struct and fills all fields except literal values
 *
 * @param code
 * @return int
 */
int token(int code)
{
    prevToken = nextToken;
    nextToken = (struct token *)malloc(sizeof(struct token));

    if (nextToken == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }

    nextToken->category = code;
    nextToken->text = (char *)malloc(sizeof(char) * (strlen(yytext) + 1));
    if (nextToken->text == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }
    strcpy(nextToken->text, yytext);
    nextToken->lineno = rows;
    nextToken->filename = (char *)malloc(sizeof(char) * (strlen(filename) + 1));
    if (nextToken->filename == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }
    strcpy(nextToken->filename, filename);
    nextToken->sval = NULL;

    return code;
}

int leaf(int code)
{
    token(code);
    // printf("%s\n", yytext);
    yylval.treeptr = alctoken(code, nextToken->text, 0); // tree.c
    yylval.treeptr->leaf = nextToken;

    return code;
}

void changeSymbolName(char *string)
{
    free(yylval.treeptr->symbolname);
    yylval.treeptr->symbolname = strdup(string);
}

// creates a token then fills the ival field
int intLiteral(int code)
{
    leaf(code);
    changeSymbolName("intLiteral");

    char *temp = removeUnderscores(nextToken->text);

    nextToken->ival = atoi(temp);
    free(temp);

    return code;
}

// discards the L at the end of a long then creates an integer token
int longLiteral(int code)
{
    int len = strlen(yytext);
    yytext[len - 1] = '\0';
    intLiteral(code);
    changeSymbolName("longLiteral");

    return code;
}

// creates a token then fills the ival field
int hexLiteral(int code)
{
    leaf(code);
    changeSymbolName("hexLiteral");

    char *temp = removeUnderscores(nextToken->text);

    sscanf(temp, "%x", &(nextToken->ival));

    free(temp);

    return code;
}

// discards the f at the end of a float then creates an double token
int floatLiteral(int code)
{
    int len = strlen(yytext);
    yytext[len - 1] = '\0';
    doubleLiteral(code);
    changeSymbolName("floatLiteral");

    return code;
}

// creates a token then fills the dval field
int doubleLiteral(int code)
{
    leaf(code);
    changeSymbolName("doubleLiteral");

    char *temp = removeUnderscores(nextToken->text);

    sscanf(temp, "%lf", &(nextToken->dval));

    free(temp);

    return code;
}

// allocates memory for sval the copies the contents of yytext into it
// removing the start and end quotes and translating escape sequences into their values
int stringLiteral(int code)
{
    leaf(code);
    changeSymbolName("stringLiteral");

    nextToken->sval = (char *)malloc(sizeof(char) * (strlen(nextToken->text) + 1));

    if (nextToken->sval == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }

    char *nextCharLex = nextToken->text;
    char *nextCharLit = nextToken->sval;

    // skip intial "
    nextCharLex++;

    while (*nextCharLex != '"')
    {
        // if not the start of escape sequence transfer letter
        if (*nextCharLex != '\\')
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
                fprintf(stderr, "File: %s Line:%d The escape sequence \\%c is not supported by k0\n", nextToken->filename, nextToken->lineno, *nextCharLex);
                exit(1);
                break;
            }
        }
        nextCharLex++;
        nextCharLit++;
    }

    // don't forget the null character
    *nextCharLit = '\0';

    return code;
}

// removes the triple quotes at the front and end of the string
// i checked in kotlin and multiline strings don't have escape sequences
// so just copy the rest of the string
int multiLineString(int code)
{
    leaf(code);
    changeSymbolName("multilineStringLiteral");

    nextToken->sval = (char *)malloc(sizeof(char) * (strlen(nextToken->text) + 1));

    if (nextToken->sval == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }

    char *nextCharLex = nextToken->text;
    char *nextCharLit = nextToken->sval;

    int len = strlen(nextToken->text);

    // copy string without the triple quotes
    for (int i = 3; i < len - 3; i++)
    {
        *nextCharLit = nextCharLex[i];
        nextCharLit++;
    }
    // don't forget to close the string
    *nextCharLit = '\0';

    return code;
}

char *removeUnderscores()
{
    char *nextCharLex = nextToken->text;
    char *nextCharLit = (char *)malloc(sizeof(char) * (strlen(yytext) + 1));
    int i = 0;
    for (int j = 0; nextCharLex[i] != '\0'; i++)
    {
        if (*nextCharLex == '_')
        {
            j++;
        }
        else
        {
            nextCharLit[i] = nextCharLex[j + i];
        }
    }
    nextCharLit[i] = '\0';
    return nextCharLit;
}

int addSemi()
{
    if (!nextToken)
        return 0; // Null
    switch (nextToken->category)
    {
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
        case BYTE:
        case INT:
        case DOUBLE:
        case CHAR:
        case STRING:
        case TRUE:
        case FALSE:
        case NULL_K:
            yytext = ";";
            return 1; // True
        default:
            return 0; // False
    }
}

/**
 * @brief Checks if a token is the end of a statement
 *
 * @param tok
 * @return true
 */
bool isEndOfStmt(int tok) {
    switch (tok) {
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
        case SEMICOLON:
        case NULL_K:
            return true;
        default:
            return false;
    }
}

/**
 * @brief Which tokens should start a fresh statement
 *
 * @param tok
 * @return bool
 */
bool isBeginOfStmt(int tok) {
    switch (tok) {
    case IF:
    case FOR:
    case WHILE:
    case DO:
    case IDENTIFIER:  // e.g. a bare expression statement
    case RETURN:
    case BREAK:
    case CONTINUE:
    case RCURL: // This pushes the problem to var in line 24
    case VAR: //case VAL: This pushes the problem to z on line 70
    case FUN:
    case VAL:
        return true;
      default:
        return false;
    }
}

extern int yylex(void);
//#undef yylex
//#define k0_yylex yylex

int k0_yylex(void) {
    return yylex();
}

/**
 * @brief Wrapper for yylex
 *
 * TODO: Need to have a way to check past and future.
 *
 * @return int
 */
int yylex3(void) {
    int tok = 0;
    if (savedToken) {
        fprintf(stderr,
                "Replay savedToken=%d\n",
                savedToken);
        if (saved_lval.treeptr) {
            fprintf(stderr,
                    "        symbolname=\"%s\"\n",
                    saved_lval.treeptr->symbolname);
        }
        yylval = saved_lval;
        tok = savedToken;
        savedToken = 0;
        lastToken  = tok;
        return tok;
    }

    tok = k0_yylex();
    // fprintf(stderr,
    //        "Got raw tok=%d, lastToken=%d, saw_newline=%d at line %d\n",
    //        tok, lastToken, saw_newline, rows);

    if (saw_newline && isEndOfStmt(lastToken) && isBeginOfStmt(tok)) {
        savedToken = tok;
        saved_lval = yylval;
        // fprintf(stderr,
        //         "Saving lookahead tok=%d\n",
        //         savedToken);
        if (saved_lval.treeptr) {
            // fprintf(stderr,
            //         "        saved symbolname=\"%s\"\n",
            //         saved_lval.treeptr->symbolname);
        }
        tok = SEMICOLON;
        saw_newline = 0;
        fprintf(stderr, "Injecting SEMICOLON instead\n");
        return savedToken;
    }

    
    lastToken  = tok;
    return tok;
}

/**
 * 
 */
int yylex2(void){
    int tok = 0; // Carrier of the semis
    char *temp = NULL;
    // Preloading
    if(lastToken == 0){
        lastToken = k0_yylex();
        last_lval = yylval;
    }

    // Avoid yylex
    if (inserted == 1){
        inserted = 0;
        yylval = last_lval;
        tok = lastToken;
        last_lval = saved_lval;
        lastToken = savedToken;
        printf("%s\n", yylval.treeptr->leaf->text);
        return tok;
    }

    savedToken = k0_yylex(); // Messages from the future
    saved_lval = yylval;


    

    if(saw_newline && isEndOfStmt(lastToken) && isBeginOfStmt(savedToken)){
        temp = yytext;
        yytext = ";";
        tok = leaf(SEMICOLON);
        yytext = temp;
        
        blightToken = lastToken;
        magic_lval = last_lval;
        last_lval = yylval;
        lastToken = tok;
        tok = blightToken;
        yylval = magic_lval;
        saw_newline = 0;
        inserted = 1; // The answer is always more global variables
        printf("%s\n", yylval.treeptr->leaf->text);
        return tok;
    }
    

    // BEGIN THE GREAT SHIFTING
    saw_newline = 0;
    yylval = last_lval;
    tok = lastToken;
    last_lval = saved_lval;
    lastToken = savedToken;
    printf("%s\n", yylval.treeptr->leaf->text);
    return tok;
}