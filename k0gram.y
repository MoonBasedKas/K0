%{
    #include "lex.h"
    #include "tree.h"
%}


/* %union {
    struct tree *treeptr;
} */

%token ASSIGNMENT
%token ADD_ASSIGNMENT
%token SUB_ASSIGNMENT
%token ADD
%token SUB
%token MULT
%token DIV
%token MOD
%token INCR
%token DECR
%token EQEQ
%token EXCL_EQ
%token LANGLE
%token RANGLE
%token LE
%token GE
%token EQEQEQ
%token EXCL_EQEQ
%token CONJ
%token DISJ
%token EXCL_NO_WS
%token EXCL_WS
%token QUEST_WS 
%token QUEST_NO_WS
%token RANGE

%token DOT
%token COMMA
%token LPAREN
%token RPAREN
%token LSQUARE
%token RSQUARE
%token LCURL
%token RCURL
%token COLON
%token SEMICOLON

%token RESERVED
%token ARROW
%token DOUBLE_ARROW
%token COLONCOLON
%token DOUBLE_SEMICOLON
%token SINGLE_QUOTE

%token BREAK
%token CONTINUE
%token DO
%token ELSE
%token FALSE
%token FOR
%token FUN
%token IF
%token IN
%token NULL_K
%token RETURN
%token TRUE
%token VAL
%token VAR
%token WHEN
%token WHILE

%token IMPORT

%token CONST

%token INTERFACE
%token OVERRIDE

%token INTEGER_LITERAL
%token HEX_LITERAL
%token REAL_LITERAL
%token CHARACTER_LITERAL
%token IDENTIFIER
%token LINE_STRING
%token MULTILINE_STRING

%token EOF_K

%start program

%%

program:
    topLevelObjectList
    ;

topLevelObjectList:
    topLevelObject
    | topLevelObjectList topLevelObject
    ;

topLevelObject: 
    declaration semis 
    ;

semis:
    semis SEMICOLON
    | SEMICOLON
    ;

declaration:
    functionDeclaration
    | propertyDeclaration
    ;

propertyDeclaration:
    variable variableDeclaration semis
    | variable variableDeclaration assignment semis
    | variable reciverType variableDeclaration semis
    | variable reciverType variableDeclaration assignment semis
    | variable typeParameters variableDeclaration semis
    | variable typeParameters variableDeclaration assignment semis
    | variable typeParameters reciverType variableDeclaration semis
    | variable typeParameters reciverType variableDeclaration assignment semis

variable:
    VAL
    | VAR
    ;

typeParameters:
    LANGLE variableDeclarationList RANGLE
    ;


functionDeclaration: 
    FUN IDENTIFIER functionValueParameters COLON type functionBody 
    | FUN IDENTIFIER functionValueParameters COLON type 
    ;

functionValueParameters:
    LPAREN functionValueParamList RPAREN
    ;

functionValueParamList:
    functionValueParameter COMMA functionValueParamList
    | functionValueParameter
    ;

functionValueParameter:
    variableDeclaration
    | variableDeclaration ASSIGNMENT expression
    ;

type: 
    functionType 
    | parenthesizedType 
    | nullableType 
    | userType
    ;

userType:
    simpleUserType
    | userType DOT simpleUserType
    ;

simpleUserType:
    IDENTIFIER
    | IDENTIFIER typeArguments
    ;

typeArguments:
    LANGLE typeArgumentsList RANGLE
    ;

typeArgumentsList:
    typeArgument
    | typeArgument COMMA typeArgumentsList
    ;

typeArgument:
    type
    | MULT
    ;

reciverType:
    parenthesizedType
    | nullableType
    ;

functionType:
    reciverType DOT functionTypeParameters ARROW type
    | functionTypeParameters ARROW type
    ;

functionTypeParameters:
    LPAREN functionTypeParamList RPAREN
    | LPAREN RPAREN
    ;

functionTypeParamList:
    functionTypeParameter COMMA functionTypeParamList
    | functionTypeParameter
    ;

functionTypeParameter:
    variableDeclaration
    | type
    ;

parenthesizedType:
    LPAREN type RPAREN
    ;

nullableType:
    parenthesizedType quests
    ;

quests:
    quest quests
    | quest
    ;

quest:
    QUEST_NO_WS
    | QUEST_WS
    ;

functionBody: 
    block 
    | ASSIGNMENT expression 
    ;

block: 
    LCURL RCURL
    LCURL statements RCURL 
    ;

statements:
    | statement
    | statement semis
    | statements semis statement
    ;

statement:
    declaration
    | assignment
    | loopStatement
    | expression
    ;

assignment:
    IDENTIFIER ASSIGNMENT expression
    | IDENTIFIER ADD_ASSIGNMENT expression
    | IDENTIFIER SUB_ASSIGNMENT expression
    ;

loopStatement:
    forStatement
    | whileStatement
    | doWhileStatement
    ;

forStatement:
    FOR LPAREN variableDeclarations IN expression RPAREN controlStructureBody
    | FOR LPAREN variableDeclarations IN expression RPAREN 
    ;

whileStatement:
    WHILE LPAREN expression RPAREN controlStructureBody
    | WHILE LPAREN expression RPAREN SEMICOLON

doWhileStatement:
    DO controlStructureBody WHILE LPAREN expression RPAREN
    | DO WHILE LPAREN expression RPAREN
    ;

variableDeclarations:
    variableDeclaration
    | multiVariableDeclaration
    ;

variableDeclaration:
    IDENTIFIER COLON type
    ;

multiVariableDeclaration:
    LPAREN variableDeclarationList RPAREN
    ; 

variableDeclarationList:
    variableDeclarationList COMMA variableDeclaration
    ;

expression:
    disjunction
    ;

disjunction:
    disjunction DISJ conjunction
    | conjunction
    ;

conjunction:
    conjunction CONJ equality
    | equality
    ;

equality: 
    equality EQEQ comparison
    | equality EXCL_EQ comparison
    | equality EQEQEQ comparison
    | equality EXCL_EQEQ comparison
    | comparison
    ;

comparison:
    comparison LANGLE infixOperation
    | comparison RANGE infixOperation
    | comparison GE infixOperation
    | comparison LE infixOperation
    | infixOperation
    ;

infixOperation:
    infixOperation IN elvisExpression
    | elvisExpression
    ;

elvisExpression:
    elvisExpression QUEST_NO_WS COLON infixFunctionCall
    | infixFunctionCall
    ;

infixFunctionCall:
    infixFunctionCall IDENTIFIER rangeExpression
    | rangeExpression
    ;

rangeExpression:
    rangeExpression RANGE additiveExpression
    | rangeExpression RANGE LANGLE additiveExpression
    | additiveExpression
    ;

additiveExpression:
    additiveExpression ADD multiplicativeExpression
    | additiveExpression SUB multiplicativeExpression
    | multiplicativeExpression
    ;

multiplicativeExpression:
    multiplicativeExpression MULT prefixUnaryExpression
    | multiplicativeExpression DIV prefixUnaryExpression
    | multiplicativeExpression MOD prefixUnaryExpression
    ;

prefixUnaryExpression:
    INCR postfixUnaryExpression
    | DECR postfixUnaryExpression
    | ADD postfixUnaryExpression
    | SUB postfixUnaryExpression
    | EXCL_WS postfixUnaryExpression
    | EXCL_NO_WS postfixUnaryExpression
    | postfixUnaryExpression
    ;

postfixUnaryExpression:
    primaryExpression INCR
    | primaryExpression DECR
    | primaryExpression EXCL_NO_WS EXCL_NO_WS
    | primaryExpression EXCL_NO_WS EXCL_WS
    | primaryExpression
    ;

primaryExpression:
    parenthesizedExpression
    | IDENTIFIER
    | INTEGER_LITERAL
    | HEX_LITERAL
    | CHARACTER_LITERAL
    | REAL_LITERAL
    | TRUE
    | FALSE
    | NULL_K
    | LINE_STRING
    | MULTILINE_STRING
    | ifExpression
    | whenExpression
    | jumpExpression
    ;

parenthesizedExpression:
    LPAREN expression RPAREN
    ;

ifExpression:
    IF LPAREN expression RPAREN SEMICOLON 
    | IF LPAREN expression RPAREN controlStructureBody semis
    | IF LPAREN expression RPAREN controlStructureBody SEMICOLON ELSE controlStructureBody
    ;

whenExpression:
    WHEN LCURL RCURL
    | WHEN LCURL whenEntries RCURL
    | WHEN whenSubject LCURL RCURL
    | WHEN whenSubject LCURL whenEntries RCURL
    ;

whenSubject:
    LPAREN expression RPAREN
    LPAREN VAL variableDeclaration ASSIGNMENT expression RPAREN
    ;

whenEntries:
    whenEntries whenEntry
    | whenEntry
    ;

whenEntry:
    whenConditionList ARROW controlStructureBody SEMICOLON
    | ELSE ARROW controlStructureBody SEMICOLON
    ;

whenConditionList:
    whenConditionList COMMA whenCondition

whenCondition:
    expression
    | IN expression
    ;

controlStructureBody:
    block
    | statement
    ;

jumpExpression:
    RETURN expression
    | RETURN
    | CONTINUE
    | BREAK
    ;