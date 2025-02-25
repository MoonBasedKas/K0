%{
    #include "lex.h"
    #include "tree.h"
%}


enum{
    TOPLEVELOBJECT = 1000,
    TOPLEVELOBJECTLIST.
    PROPERTYDECLRATION,
    VARIABLE,
    TYPEPARAMETERS,
    FUNCTIONDECLARATION,
    FUNCTIONVALUEPARAMETERS,
    FUNCTIONVALUEPARAMLIST,
    TYPE
};

%union {
    struct tree *treeptr;
}

/* Non-terminals */
%type <treeptr> program
%type <treeptr> topLevelObjectList
%type <treeptr> topLevelObject
%type <treeptr> declaration
%type <treeptr> propertyDeclaration
%type <treeptr> variable
%type <treeptr> functionDeclaration
%type <treeptr> functionValueParameters
%type <treeptr> functionValueParamList
%type <treeptr> functionValueParameter
%type <treeptr> type
%type <treeptr> userType
%type <treeptr> simpleUserType
%type <treeptr> typeArgumentsList
%type <treeptr> typeArgument
%type <treeptr> reciverType
%type <treeptr> typeParameters
%type <treeptr> functionType
%type <treeptr> functionTypeParameters
%type <treeptr> functionTypeParamList
%type <treeptr> functionTypeParameter
%type <treeptr> parenthesizedType_opt
%type <treeptr> quests
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
%type <treeptr> expressionList
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
%type <treeptr> postfixUnaryOperator
%type <treeptr> excl
%type <treeptr> prefixUnaryOperator
%type <treeptr> quest

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
%token <treeptr> RANGE_UNTIL
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


%right ASSIGNMENT ADD_ASSIGNMENT SUB_ASSIGNMENT
%left DISJ
%left CONJ
%nonassoc EQEQ EQEQEQ EXCL_EQ EXCL_EQEQ
%nonassoc LANGLE RANGLE LE GE
%left ADD SUB
%left MULT DIV MOD
%right INCR DECR

%start program

%%

program:
    topLevelObjectList {$$ = $1;}
    ;

topLevelObjectList:
    topLevelObject {$$ = 1;}
    | topLevelObjectList topLevelObject {$$ = alctoken(TOPLEVELOBJECT, "topLevelObject", 2, $1, $2);}
    ;

topLevelObject:
    declaration {$$ = $1;}
    ;

declaration:
    functionDeclaration {$$ = 1;}
    | propertyDeclaration {$$ = 1;}
    ;

propertyDeclaration: // Do we need modifiers here? (modifier->propertyModifier->const)
    variable variableDeclaration SEMICOLON {$$ = $1;}
    | variable variableDeclaration assignment SEMICOLON {$$ = $1;}
    | variable reciverType variableDeclaration SEMICOLON {$$ = $1;}
    | variable reciverType variableDeclaration assignment SEMICOLON {$$ = $1;}
    | variable typeParameters variableDeclaration SEMICOLON {$$ = $1;}
    | variable typeParameters variableDeclaration assignment SEMICOLON {$$ = $1;}
    | variable typeParameters reciverType variableDeclaration SEMICOLON {$$ = $1;}
    | variable typeParameters reciverType variableDeclaration assignment SEMICOLON {$$ = $1;}

variable:
    VAL {$$ = $1;}
    | VAR {$$ = $1;}
    ;

typeParameters:
    LANGLE variableDeclarationList RANGLE {$$ = $1;}
    ;


functionDeclaration:
    FUN IDENTIFIER functionValueParameters COLON type functionBody {$$ = $1;}
    | FUN IDENTIFIER functionValueParameters COLON type {$$ = $1;}
    | FUN IDENTIFIER functionValueParameters functionBody {$$ = $1;}
    ;

functionValueParameters:
    LPAREN functionValueParamList RPAREN {$$ = $1;}
    ;

functionValueParamList:
    functionValueParameter COMMA functionValueParamList {$$ = $1;}
    | functionValueParameter {$$ = $1;}
    ;

functionValueParameter:
    variableDeclaration {$$ = $1;}
    | variableDeclaration ASSIGNMENT expression {$$ = $1;}
    ;

type:
    functionType {$$ = $1;}
    | parenthesizedType_opt {$$ = $1;}
    | userType {$$ = $1;}
    ;

userType:
    simpleUserType {$$ = $1;}
    | userType DOT simpleUserType {$$ = $1;}
    ;

simpleUserType:
    IDENTIFIER {$$ = $1;}
    | IDENTIFIER LANGLE typeArgumentsList RANGLE {$$ = $1;}
    ;

typeArgumentsList:
    typeArgument {$$ = $1;}
    | typeArgument COMMA typeArgumentsList {$$ = $1;}
    ;

typeArgument:
    type {$$ = $1;}
    | MULT {$$ = $1;}
    ;

reciverType:
    parenthesizedType_opt {$$ = $1;}
    ;

functionType:
    reciverType DOT functionTypeParameters ARROW type {$$ = $1;}
    | functionTypeParameters ARROW type {$$ = $1;}
    ;

functionTypeParameters:
    LPAREN functionTypeParamList RPAREN {$$ = $1;}
    | LPAREN RPAREN {$$ = $1;}
    ;

functionTypeParamList:
    functionTypeParameter COMMA functionTypeParamList {$$ = $1;}
    | functionTypeParameter {$$ = $1;}
    ;

functionTypeParameter:
    variableDeclaration {$$ = $1;}
    | type {$$ = $1;}
    ;

parenthesizedType_opt:
    LPAREN type RPAREN quests {$$ = $1;}
    | LPAREN type RPAREN {$$ = $1;}
    ;

quests:
    quest {$$ = $1;}
    | quests quest {$$ = $1;}
    ;

quest:
    QUEST_NO_WS {$$ = $1;}
    | QUEST_WS {$$ = $1;}
    ;

functionBody:
    block {$$ = $1;}
    | ASSIGNMENT expression {$$ = $1;}
    ;

block:
    LCURL RCURL {$$ = $1;}
    | LCURL statements RCURL {$$ = $1;}
    ;

statements:
    statement SEMICOLON {$$ = $1;}
    | statement SEMICOLON {$$ = $1;}
    | statements SEMICOLON statement {$$ = $1;}
    | SEMICOLON {$$ = $1;}
    ;

statement:
    declaration {$$ = $1;}
    | loopStatement {$$ = $1;}
    | expression {$$ = $1;}
    ;

assignment:
    IDENTIFIER ASSIGNMENT expression {$$ = $1;}
    | IDENTIFIER ADD_ASSIGNMENT expression {$$ = $1;}
    | IDENTIFIER SUB_ASSIGNMENT expression {$$ = $1;}
    ;

loopStatement:
    forStatement {$$ = $1;}
    | whileStatement {$$ = $1;}
    | doWhileStatement {$$ = $1;}
    ;

forStatement:
    FOR LPAREN variableDeclarations IN expression RPAREN controlStructureBody {$$ = $1;}
    ;

whileStatement:
    WHILE LPAREN expression RPAREN controlStructureBody {$$ = $1;}
    | WHILE LPAREN expression RPAREN SEMICOLON {$$ = $1;}
    ;

doWhileStatement:
    DO controlStructureBody WHILE LPAREN expression RPAREN {$$ = $1;}
    ;

variableDeclarations:
    variableDeclaration {$$ = $1;}
    | multiVariableDeclaration {$$ = $1;}
    ;

variableDeclaration:
    IDENTIFIER COLON type {$$ = $1;}
    ;

multiVariableDeclaration:
    LPAREN variableDeclarationList RPAREN {$$ = $1;}
    ;
/* STOP HERE */
variableDeclarationList:
    variableDeclaration
    | variableDeclarationList COMMA variableDeclaration
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
    | rangeExpression RANGE_UNTIL additiveExpression
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
    | prefixUnaryExpression
    ;

prefixUnaryExpression:
    postfixUnaryExpression
    | prefixUnaryOperator postfixUnaryExpression
    ;

prefixUnaryOperator:
    INCR
    | DECR
    | SUB
    | ADD
    | excl
    ;

postfixUnaryExpression:
    primaryExpression postfixUnaryOperator
    | primaryExpression
    ;

postfixUnaryOperator:
    INCR
    | DECR
    | EXCL_NO_WS excl
    ;

excl:
    EXCL_NO_WS
    | EXCL_WS
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
    | IDENTIFIER LPAREN expressionList RPAREN
    | IDENTIFIER LPAREN RPAREN
    ;

expressionList:
    expression COMMA expression
    | expression
    ;

parenthesizedExpression:
    LPAREN expression RPAREN
    ;

ifExpression:
    IF LPAREN expression RPAREN SEMICOLON
    | IF LPAREN expression RPAREN controlStructureBody SEMICOLON
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
    | LPAREN VAL variableDeclaration ASSIGNMENT expression RPAREN
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
    | whenCondition;

whenCondition:
    expression
    | IN expression
    ;

controlStructureBody:
    block
    | statement
    ;

jumpExpression: // SEMICOLON added for shift/reduce conflict. Exclude in semantic value?
    RETURN SEMICOLON
    | RETURN expression
    | CONTINUE
    | BREAK
    ;