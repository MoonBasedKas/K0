%{
    #include "lex.h"
    #include "tree.h"
%}


%union {
    struct tree *treeptr;
}

/* Non-terminals */
%type <treeptr> program
%type <treeptr> topLevelObjectList
%type <treeptr> topLevelObject
%type <treeptr> semis
%type <treeptr> declaration
%type <treeptr> propertyDeclaration
%type <treeptr> functionDeclaration
%type <treeptr> functionValueParameters
%type <treeptr> functionValueParamList
%type <treeptr> functionValueParameter
%type <treeptr> type
%type <treeptr> userType
%type <treeptr> simpleUserType
%type <treeptr> typeArguments
%type <treeptr> typeArgumentsList
%type <treeptr> typeArgument
%type <treeptr> reciverType
%type <treeptr> functionType
%type <treeptr> functionTypeParameters
%type <treeptr> functionTypeParamList
%type <treeptr> functionTypeParameter
%type <treeptr> parenthesizedType
%type <treeptr> nullableType
%type <treeptr> quests
%type <treeptr> quest
%type <treeptr> functionBody
%type <treeptr> block
%type <treeptr> statements
%type <treeptr> statement
%type <treeptr> assignment
%type <treeptr> loopStatement
%type <treeptr> forStatement
%type <treeptr> whileStatement
%type <treeptr> doWhileStatement
%type <treeptr> variableDeclarations
%type <treeptr> variableDeclaration
%type <treeptr> multiVariableDeclaration
%type <treeptr> variableDeclarationList
%type <treeptr> expression
%type <treeptr> disjunction
%type <treeptr> conjunction
%type <treeptr> equality
%type <treeptr> comparison
%type <treeptr> infixOperation
%type <treeptr> elvisExpression
%type <treeptr> infixFunctionCall
%type <treeptr> rangeExpression
%type <treeptr> additiveExpression
%type <treeptr> multiplicativeExpression
%type <treeptr> prefixUnaryExpression
%type <treeptr> postfixUnaryExpression
%type <treeptr> primaryExpression
%type <treeptr> parenthesizedExpression
%type <treeptr> ifExpression
%type <treeptr> whenExpression
%type <treeptr> whenSubject
%type <treeptr> whenEntries
%type <treeptr> whenEntry
%type <treeptr> whenConditionList
%type <treeptr> whenCondition
%type <treeptr> controlStructureBody
%type <treeptr> jumpExpression

/* Terminals */
%token <treeptr> ASSIGNMENT
%token <treeptr> ADD_ASSIGNMENT
%token <treeptr> SUB_ASSIGNMENT
%token <treeptr> ADD
%token <treeptr> SUB
%token <treeptr> MULT
%token <treeptr> DIV
%token <treeptr> MOD
%token <treeptr> INCR
%token <treeptr> DECR
%token <treeptr> EQEQ
%token <treeptr> EXCL_EQ
%token <treeptr> LANGLE
%token <treeptr> RANGLE
%token <treeptr> LE
%token <treeptr> GE
%token <treeptr> EQEQEQ
%token <treeptr> EXCL_EQEQ
%token <treeptr> CONJ
%token <treeptr> DISJ
%token <treeptr> EXCL_NO_WS
%token <treeptr> EXCL_WS
%token <treeptr> QUEST_WS
%token <treeptr> QUEST_NO_WS
%token <treeptr> RANGE

%token <treeptr> DOT
%token <treeptr> COMMA
%token <treeptr> LPAREN
%token <treeptr> RPAREN
%token <treeptr> LSQUARE
%token <treeptr> RSQUARE
%token <treeptr> LCURL
%token <treeptr> RCURL
%token <treeptr> COLON
%token <treeptr> SEMICOLON

%token <treeptr> RESERVED
%token <treeptr> ARROW
%token <treeptr> DOUBLE_ARROW
%token <treeptr> COLONCOLON
%token <treeptr> DOUBLE_SEMICOLON
%token <treeptr> SINGLE_QUOTE

%token <treeptr> BREAK
%token <treeptr> CONTINUE
%token <treeptr> DO
%token <treeptr> ELSE
%token <treeptr> FALSE
%token <treeptr> FOR
%token <treeptr> FUN
%token <treeptr> IF
%token <treeptr> IN
%token <treeptr> NULL_K
%token <treeptr> RETURN
%token <treeptr> TRUE
%token <treeptr> VAL
%token <treeptr> VAR
%token <treeptr> WHEN
%token <treeptr> WHILE

%token <treeptr> IMPORT

%token <treeptr> CONST

%token <treeptr> INTERFACE
%token <treeptr> OVERRIDE

%token <treeptr> INTEGER_LITERAL
%token <treeptr> HEX_LITERAL
%token <treeptr> REAL_LITERAL
%token <treeptr> CHARACTER_LITERAL
%token <treeptr> IDENTIFIER
%token <treeptr> LINE_STRING
%token <treeptr> MULTILINE_STRING

%token <treeptr> EOF_K

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