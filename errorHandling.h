#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

void unsupportedKeyword();
void unsupportedOperator();
void typeError(char *message, struct tree *node);
void yyerror (char const *s);

// icode stuff
void debugICode(char *string, struct tree *node);

#endif