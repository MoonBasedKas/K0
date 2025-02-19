%{
    #include "lex.h"
%}

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

%token NL
%token EOF_K

%start program

%%

// Basic separator rules
semis:
    semis semi
    | semi
    ;

semi:
    SEMICOLON optNewLine
    | newLine optNewLine
    ;

optNewLine:
    %empty
    | newLine
    | optNewLine newLine
    ;

newLine:
    NL
    | newLine NL
    ;

//program structure
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

declaration:
    functionDeclaration
    | propertyDeclaration
    ;

propertyDeclaration:
    VAL optNewLine optTypeParameters optNewLine optReciverType optNewLine variableDeclarations optNewLine optAssign semis
    | VAR optNewLine optTypeParameters optNewLine optReciverType optNewLine variableDeclarations optNewLine optAssign semis

optTypeParameters:
    typeParameters
    | %empty
    ;

typeParameters:
    LANGLE optNewLine typeParameterList optNewLine RANGLE
    ;

typeParameterList:
    typeParameterList optNewLine COMMA typeParameter
    ;

typeParameter:
    IDENTIFIER COLON type
    ;

optReciverType:
    %empty
    | reciverType
    ;

optAssign:
    %empty
    | ASSIGNMENT optNewLine expression
    ;

functionDeclaration: 
    FUN optNewLine IDENTIFIER optNewLine functionValueParameters optNewLine optType optNewLine optFunctionBody 
    ;

optType:
    COLON optNewLine type
    | %empty
    ;

optFunctionBody:
    functionBody
    | %empty
    ;

functionValueParameters:
    LPAREN optNewLine functionValueParamList optNewLine RPAREN
    ;

functionValueParamList:
    functionValueParameter optNewLine COMMA optNewLine functionValueParamList
    | functionValueParameter
    ;

functionValueParameter:
    parameter
    | parameter optNewLine ASSIGNMENT optNewLine expression
    ;

parameter:
    IDENTIFIER optNewLine COLON optNewLine type
    ;

type: 
    functionType 
    | parenthesizedType 
    | nullableType 
    | userType
    ;

userType:
    simpleUserType
    | userType optNewLine DOT optNewLine simpleUserType
    ;

simpleUserType:
    IDENTIFIER
    | IDENTIFIER optNewLine typeArguments
    ;

typeArguments:
    LANGLE optNewLine typeArgumentsList optNewLine RANGLE
    | LANGLE optNewLine typeArgumentsList optNewLine RANGLE

typeArgumentsList:
    typeArgument
    | typeArgument optNewLine COMMA optNewLine typeArgumentsList

typeArgument:
    type
    | MULT
    ;

reciverType:
    parenthesizedType
    | nullableType
    ;

functionType:
    reciverType optNewLine DOT optNewLine functionTypeParameters optNewLine ARROW optNewLine type
    | functionTypeParameters ARROW type
    ;

functionTypeParameters:
    LPAREN optNewLine functionTypeParamList optNewLine RPAREN
    ;

functionTypeParamList:
    functionTypeParameter optNewLine COMMA optNewLine functionTypeParamList
    | functionTypeParameter
    ;

functionTypeParameter:
    parameter
    | type
    | %empty
    ;

parenthesizedType:
    LPAREN optNewLine type optNewLine RPAREN
    ;

nullableType:
    parenthesizedType optNewLine quests
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
    | ASSIGNMENT optNewLine expression 
    ;

block: 
    LCURL optNewLine statements optNewLine RCURL 
    ;

statements:
    %empty
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
    IDENTIFIER ASSIGNMENT optNewLine expression
    | IDENTIFIER ADD_ASSIGNMENT optNewLine expression
    | IDENTIFIER SUB_ASSIGNMENT optNewLine expression
    ;

loopStatement:
    forStatement
    | whileStatement
    | doWhileStatement
    ;

forStatement:
    FOR optNewLine LPAREN variableDeclarations IN expression RPAREN optNewLine controlStructureBody
    | FOR optNewLine LPAREN variableDeclarations IN expression RPAREN optNewLine 
    ;

whileStatement:
    WHILE optNewLine LPAREN expression RPAREN optNewLine controlStructureBody
    | WHILE optNewLine LPAREN expression RPAREN optNewLine SEMICOLON

doWhileStatement:
    DO optNewLine controlStructureBody optNewLine WHILE optNewLine LPAREN expression RPAREN
    | DO optNewLine WHILE optNewLine LPAREN expression RPAREN
    ;

variableDeclarations:
    variableDeclaration
    | multiVariableDeclaration
    ;

variableDeclaration:
    optNewLine IDENTIFIER optNewLine COLON optNewLine type
    ;

multiVariableDeclaration:
    LPAREN optNewLine variableDeclarationList optNewLine RPAREN
    ; 

variableDeclarationList:
    variableDeclarationList optNewLine COMMA optNewLine variableDeclaration
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
    LPAREN optNewLine expression optNewLine RPAREN
    ;

ifExpression:
    IF optNewLine LPAREN optNewLine expression RPAREN optNewLine SEMICOLON 
    | IF optNewLine LPAREN optNewLine expression RPAREN optNewLine controlStructureBody semis
    | IF optNewLine LPAREN optNewLine expression RPAREN optNewLine controlStructureBody semi ELSE controlStructureBody
    ;

whenExpression:
    WHEN optNewLine optWhenSubject optNewLine LCURL optNewLine optWhenEntries optNewLine RCURL
    ;

optWhenSubject:
    %empty
    | whenSubject
    ;

whenSubject:
    LPAREN expression RPAREN
    LPAREN optNewLine VAL optNewLine variableDeclaration optNewLine ASSIGNMENT optNewLine expression RPAREN
    ;

optWhenEntries:
    optWhenEntries whenEntry
    | %empty
    ;

whenEntry:
    whenConditionList optNewLine ARROW optNewLine controlStructureBody semi
    | ELSE optNewLine ARROW optNewLine controlStructureBody semi
    ;

whenConditionList:
    whenConditionList optNewLine COMMA optNewLine whenCondition

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