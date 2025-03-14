%{
    #include <stdio.h>
    #include "lex.h"
    #include "tree.h"

    #define YYDEBUG 1
%}
//%debug - deprecated
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
%type <treeptr> excl
%type <treeptr> prefixUnaryOperator
%type <treeptr> quest
%type <treeptr> constantVal
%type <treeptr> importList
%type <treeptr> importIdentifier
%type <treeptr> postfixExpression
%type <treeptr> elseIfSequence
%type <treeptr> elseIfStmt
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


%right ASSIGNMENT ADD_ASSIGNMENT SUB_ASSIGNMENT
%left DISJ
%left CONJ
%nonassoc EQEQ EQEQEQ EXCL_EQ EXCL_EQEQ
%nonassoc LANGLE RANGLE LE GE
%left ADD SUB
%left MULT DIV MOD

%start program

%%

program:
    topLevelObjectList {root = $$ = alctoken(1000, "program", 1, $1);}
    ;

topLevelObjectList:
    topLevelObject {$$ = $1;}
    | topLevelObject SEMICOLON {$$ = $1;}
    | topLevelObjectList topLevelObject {$$ = alctoken(1001, "topLevelObject", 2, $1, $2);}
    | topLevelObjectList topLevelObject SEMICOLON {$$ = alctoken(1002, "topLevelObjectSemi", 2, $1, $2);}
    ;

topLevelObject:
    declaration {$$ = $1;}
    | importList {$$ = $1;}
    ;

importList:
    IMPORT importIdentifier {$$ = alctoken(1003, "collapsedImport", 2, $1, $2);}
    | IMPORT importIdentifier importList {$$ = alctoken(1004, "expandingImprt", 3, $1, $2, $3);}
    ;

importIdentifier:
    IDENTIFIER DOT importIdentifier {$$ = alctoken(1005, "expandingImportID", 3, $1, $2, $3);}
    | IDENTIFIER {$$ = $1;}
    | MULT {$$ = $1;}
    ;

declaration:
    functionDeclaration {$$ = $1;}
    | propertyDeclaration {$$ = $1;}
    | assignment {$$ = $1;}
    ;

propertyDeclaration:
    variable variableDeclaration  {$$ = alctoken(1006, "propDecEmpty", 2, $1, $2);}
    | variable IDENTIFIER ASSIGNMENT expression {$$ = alctoken(1007, "propDecTypeless", 3, $1, $2, $4);}
    | variable variableDeclaration ASSIGNMENT expression  {$$ = alctoken(1008, "propDecAssign", 3, $1, $2, $4);}
    | variable reciverType variableDeclaration  {$$ = alctoken(1009, "propDecReceiver", 3, $1, $2, $3);}
    | variable reciverType variableDeclaration ASSIGNMENT expression  {$$ = alctoken(1010, "propDecReceiverAssign", 4, $1, $2, $3, $5);}
    | variable typeParameters variableDeclaration  {$$ = alctoken(1011, "propDecTypeParams", 4, $1, $2, $3);}
    | variable typeParameters variableDeclaration ASSIGNMENT expression  {$$ = alctoken(1012, "propDecTypeParamsAssign", 4, $1, $2, $3, $5);}
    | variable typeParameters reciverType variableDeclaration  {$$ = alctoken(1013, "propDecTypeParamsReceiver", 4, $1, $2, $3, $4);}
    | variable typeParameters reciverType variableDeclaration ASSIGNMENT expression  {$$ = alctoken(1014, "propDecAll", 5, $1, $2, $3, $4, $6);}

variable:
    constantVal {$$ = $1;}
    | VAL {$$ = $1;}
    | VAR {$$ = $1;}
    ;

constantVal:
    CONST VAL {$$ = alctoken(1015, "constantVal", 2, $1, $2);}
    ;

typeParameters:
    LANGLE variableDeclarationList RANGLE {$$ = alctoken(1016, "typeParameters", 1, $2);}
    ;

functionDeclaration:
    FUN IDENTIFIER functionValueParameters COLON type functionBody {$$ = alctoken(1017, "funcDecAll", 5, $1, $2, $3, $5, $6);}
    | FUN IDENTIFIER functionValueParameters COLON type {$$ = alctoken(1018, "funcDecParamType", 4, $1, $2, $3, $5);}
    | FUN IDENTIFIER functionValueParameters functionBody {$$ = alctoken(1019, "funcDecParamBody", 4, $1, $2, $3, $4);}
    | FUN IDENTIFIER LPAREN RPAREN COLON type functionBody {$$ = alctoken(1020, "funcDecTypeBody", 4, $1, $2, $6, $7);}
    | FUN IDENTIFIER LPAREN RPAREN COLON type {$$ = alctoken(1021, "funcDecType", 3, $1, $2, $6);}
    | FUN IDENTIFIER LPAREN RPAREN functionBody {$$ = alctoken(1022, "funcDecBody", 3, $1, $2, $5);}
    ;

functionValueParameters:
    LPAREN functionValueParamList RPAREN {$$ = alctoken(1023, "funcValParams", 1, $2);}
    ;

functionValueParamList:
    functionValueParameter COMMA functionValueParamList {$$ = alctoken(1024, "funcValParamList", 2, $1, $3);}
    | functionValueParameter {$$ = $1;}
    ;

functionValueParameter:
    variableDeclaration {$$ = $1;}
    | variableDeclaration ASSIGNMENT expression {$$ = alctoken(1025, "funcValParamAssign", 2, $1, $3);}
    ;

type:
    functionType {$$ = $1;}
    | userType {$$ = $1;}
    ;

userType:
    simpleUserType {$$ = $1;}
    | userType DOT simpleUserType {$$ = alctoken(1026, "userType", 3, $1, $2, $3);}
    ;

simpleUserType:
    IDENTIFIER {$$ = $1;}
    | IDENTIFIER LANGLE typeArgumentsList RANGLE {$$ = alctoken(1027, "simpleUserType", 2, $1, $3);}
    ;

typeArgumentsList:
    typeArgument {$$ = $1;}
    | typeArgument COMMA typeArgumentsList {$$ = alctoken(1028, "typeArgumentsList", 2, $1, $3);}
    ;

typeArgument:
    type {$$ = $1;}
    | MULT {$$ = $1;}
    ;

reciverType:
    parenthesizedType_opt {$$ = $1;}
    ;

functionType:
    reciverType DOT functionTypeParameters ARROW type {$$ = alctoken(1029, "functionTypeDot", 5, $1, $2, $3, $4, $5);}
    | functionTypeParameters ARROW type {$$ = alctoken(1030, "functionType", 3, $1, $2, $3);}
    ;

functionTypeParameters:
    LPAREN functionTypeParamList RPAREN {$$ = alctoken(1031, "functionTypeParameters", 3, $1, $2, $3);}
    | LPAREN RPAREN {$$ = alctoken(1032, "functionTypeParamsEmpty", 2, $1, $2);}
    ;

functionTypeParamList:
    functionTypeParameter COMMA functionTypeParamList {$$ = alctoken(1033, "functionTypeParamList", 2, $1, $3);}
    | functionTypeParameter {$$ = $1;}
    ;

functionTypeParameter:
    variableDeclaration {$$ = $1;}
    | type {$$ = $1;}
    ;

parenthesizedType_opt:
    LPAREN type RPAREN quests {$$ = alctoken(1034, "parenTypeQuests", 2, $2, $4);}
    | LPAREN type RPAREN {$$ = alctoken(1035, "parenType", 1, $2);}
    ;

quests:
    quest {$$ = $1;}
    | quest quests {$$ = alctoken(1036, "quests", 2, $1, $2);}
    ;

quest:
    QUEST_NO_WS {$$ = $1;}
    | QUEST_WS {$$ = $1;}
    ;

functionBody:
    block {$$ = $1;}
    | ASSIGNMENT expression {$$ = alctoken(1037, "funcBody", 2, $1, $2);}
    ;

block:
    LCURL RCURL {$$ = alctoken(1038, "blockEmpty", 2, $1, $2);}
    | LCURL statements RCURL {$$ = alctoken(1039, "blockStmnts", 3, $1, $2, $3);}
    ;

statements:
    statement SEMICOLON {$$ = alctoken(1040, "statement", 1, $1);}
    | statement SEMICOLON statements {$$ = alctoken(1041, "statementsMultiple", 2, $1, $3);}
    | SEMICOLON {$$ = $1;}
    ;

statement:
    declaration {$$ = $1;}
    | loopStatement {$$ = $1;}
    | expression {$$ = $1;}
    ;

assignment:
    IDENTIFIER ASSIGNMENT expression {$$ = alctoken(1042, "assignment", 2, $1, $3);}
    | IDENTIFIER ADD_ASSIGNMENT expression {$$ = alctoken(1043, "assignAdd", 2, $1, $3);}
    | IDENTIFIER SUB_ASSIGNMENT expression {$$ = alctoken(1044, "assignSub", 2, $1, $3);}
    ;


loopStatement:
    forStatement {$$ = $1;}
    | whileStatement {$$ = $1;}
    | doWhileStatement {$$ = $1;}
    ;

forStatement:
    FOR LPAREN variableDeclarations IN expression RPAREN controlStructureBody {$$ = alctoken(1045, "forStmntWithVars", 4, $1, $3, $5, $7);}
    | FOR LPAREN IDENTIFIER IN expression RPAREN controlStructureBody {$$ = alctoken(1046, "forStmnt", 4, $1, $3, $5, $7);}
    ;

whileStatement:
    WHILE LPAREN expression RPAREN controlStructureBody {$$ = alctoken(1047, "whileStmntCtrlBody", 3, $1, $3, $5);}
    | WHILE LPAREN expression RPAREN SEMICOLON {$$ = alctoken(1048, "whileStmnt", 2, $1, $3);}
    ;

doWhileStatement:
    DO controlStructureBody WHILE LPAREN expression RPAREN {$$ = alctoken(1049, "doWhileStmnt", 4, $1, $2, $3, $5);}
    ;

variableDeclarations:
    variableDeclaration {$$ = $1;}
    | multiVariableDeclaration {$$ = $1;}
    ;

variableDeclaration:
    IDENTIFIER COLON type {$$ = alctoken(1050, "varDec", 2, $1, $3);}
    | IDENTIFIER COLON type quests {$$ = alctoken(1051, "varDecQuests", 3, $1, $3, $4);}
    ;

multiVariableDeclaration:
    LPAREN variableDeclarationList RPAREN {$$ = $2;}
    ;

variableDeclarationList:
    variableDeclaration {$$ = $1;}
    | variableDeclarationList COMMA variableDeclaration {$$ = alctoken(1053, "varDecList", 2, $1, $3);}
    ;

expression:
    disjunction                         {$$ = $1;}
    ;

disjunction:
    disjunction DISJ conjunction        {$$ = alctoken(1054, "disj", 2, $1, $3);}
    | conjunction                       {$$ = $1;}
    ;

conjunction:
    conjunction CONJ equality           {$$ = alctoken(1055, "conj", 2, $1, $3);}
    | equality                          {$$ = $1;}
    ;

equality:
    equality EQEQ comparison            {$$ = alctoken(1056, "equal", 2, $1, $3);}
    | equality EXCL_EQ comparison       {$$ = alctoken(1057, "notEqual", 2, $1, $3);}
    | equality EQEQEQ comparison        {$$ = alctoken(1058, "eqeqeq", 2, $1, $3);}
    | equality EXCL_EQEQ comparison     {$$ = alctoken(1059, "notEqeqeq", 2, $1, $3);}
    | comparison                        {$$ = $1;}
    ;

comparison:
    comparison LANGLE infixOperation    {$$ = alctoken(1060, "less", 2, $1, $3);}
    | comparison RANGLE infixOperation   {$$ = alctoken(1061, "greater", 2, $1, $3);}
    | comparison GE infixOperation      {$$ = alctoken(1062, "lessEqual", 2, $1, $3);}
    | comparison LE infixOperation      {$$ = alctoken(1063, "greaterEqual", 2, $1, $3);}
    | infixOperation                    {$$ = $1;}
    ;

infixOperation:
    infixOperation IN elvisExpression   {$$ = alctoken(1064, "in", 2, $1, $3);}
    | elvisExpression                   {$$ = $1;}
    ;

elvisExpression:
    elvisExpression QUEST_NO_WS COLON infixFunctionCall     {$$ = alctoken(1065, "elvis", 2, $1, $4);}
    | infixFunctionCall                                     {$$ = $1;}
    ;

infixFunctionCall:
    infixFunctionCall IDENTIFIER rangeExpression            {$$ = alctoken(1066, "infixFunction", 3, $1, $2, $3);}
    | rangeExpression                                       {$$ = $1;}
    ;

rangeExpression:
    additiveExpression RANGE rangeExpression                {$$ = alctoken(1067, "range", 2, $1, $3);}
    | additiveExpression RANGE_UNTIL rangeExpression        {$$ = alctoken(1068, "rangeUntil", 2, $1, $3);}
    | additiveExpression                                    {$$ = $1;}
    ;

additiveExpression:
    additiveExpression ADD multiplicativeExpression         {$$ = alctoken(1069, "add", 2, $1, $3);}
    | additiveExpression SUB multiplicativeExpression       {$$ = alctoken(1070, "sub", 2, $1, $3);}
    | multiplicativeExpression                              {$$ = $1;}
    ;

multiplicativeExpression:
    multiplicativeExpression MULT prefixUnaryExpression     {$$ = alctoken(1071, "mult", 2, $1, $3);}
    | multiplicativeExpression DIV prefixUnaryExpression    {$$ = alctoken(1072, "div", 2, $1, $3);}
    | multiplicativeExpression MOD prefixUnaryExpression    {$$ = alctoken(1073, "mod", 2, $1, $3);}
    | prefixUnaryExpression                                 {$$ = $1;}
    ;

prefixUnaryExpression:
    prefixUnaryOperator prefixUnaryExpression            {$$ = alctoken(1074, "prefix", 2, $1, $2);}
    | postfixExpression                                   {$$ = $1;}
    ;

prefixUnaryOperator:
    INCR                {$$ = $1;}
    | DECR              {$$ = $1;}
    | SUB               {$$ = $1;}
    | ADD               {$$ = $1;}
    | excl              {$$ = $1;}
    ;

excl:
    EXCL_NO_WS                                  {$$ = $1;}
    | EXCL_WS                                   {$$ = $1;}
    ;

primaryExpression:
    parenthesizedExpression                     {$$ = $1;}
    | IDENTIFIER                                {$$ = $1;}
    | INTEGER_LITERAL                           {$$ = $1;}
    | HEX_LITERAL                               {$$ = $1;}
    | CHARACTER_LITERAL                         {$$ = $1;}
    | REAL_LITERAL                              {$$ = $1;}
    | TRUE                                      {$$ = $1;}
    | FALSE                                     {$$ = $1;}
    | NULL_K                                    {$$ = $1;}
    | LINE_STRING                               {$$ = $1;}
    | MULTILINE_STRING                          {$$ = $1;}
    | ifExpression                                {$$ = $1;}
    | whenExpression                            {$$ = $1;}
    | jumpExpression                            {$$ = $1;}
    | IDENTIFIER LSQUARE expression RSQUARE     {$$ = alctoken(1075, "arrayAccess", 2, $1, $3);}
    ;

postfixExpression:
    primaryExpression {$$ = $1;}
    | postfixExpression LPAREN expressionList RPAREN {$$ = alctoken(1076, "postfixExpr", 2, $1, $3);}
    | postfixExpression LPAREN RPAREN {$$ = alctoken(1077, "postfixNoExpr", 1, $1);}
    | postfixExpression DOT IDENTIFIER {$$ = alctoken(1078, "postfixDotID", 3, $1, $2, $3);}
    | postfixExpression DOT IDENTIFIER LPAREN expressionList RPAREN {$$ = alctoken(1079, "postfixDotIDExpr", 4, $1, $2, $3, $5);}
    | postfixExpression DOT IDENTIFIER LPAREN RPAREN {$$ = alctoken(1080, "postfixDotIDNoExpr", 3, $1, $2, $3);}
    | postfixExpression QUEST_NO_WS DOT IDENTIFIER {$$ = alctoken(1081, "postfixSafeDotID", 4, $1, $2, $3, $4);}
    | postfixExpression QUEST_NO_WS DOT IDENTIFIER LPAREN expressionList RPAREN {$$ = alctoken(1082, "postfixSafeDotIDExpr", 5, $1, $2, $3, $4, $6);}
    | postfixExpression QUEST_NO_WS DOT IDENTIFIER LPAREN RPAREN {$$ = alctoken(1083, "postfixSafeDotIDNoExpr", 4, $1, $2, $3, $4);}
    | postfixExpression LSQUARE expression RSQUARE {$$ = alctoken(1084, "postfixArrayAccess", 2, $1, $3);}
    | postfixExpression INCR {$$ = alctoken(1085, "postfixIncr", 1, $1);}
    | postfixExpression DECR {$$ = alctoken(1086, "postfixDecr", 1, $1);}
    ;

expressionList:
    expression COMMA expressionList             {$$ = alctoken(1087, "expressionList", 2, $1, $3);}
    | expression                                {$$ = $1;}
    ;

parenthesizedExpression:
    LPAREN expression RPAREN                    {$$ = $2;}
    ;

ifExpression:
      IF LPAREN expression RPAREN controlStructureBody
         { $$ = alctoken(2010, "IfThenStmt", 2, $3, $5); }
    | IF LPAREN expression RPAREN controlStructureBody ELSE controlStructureBody
         { $$ = alctoken(2020, "IfThenElseStmt", 3, $3, $5, $7); }
    | IF LPAREN expression RPAREN controlStructureBody ELSE elseIfSequence
         { $$ = alctoken(2030, "IfThenElseIfStmt", 3, $3, $5, $6); }
    | IF LPAREN expression RPAREN controlStructureBody elseIfSequence ELSE controlStructureBody
         { $$ = alctoken(2040, "IfThenElseIfStmt", 4, $3, $5, $6, $8); }
    ;

/* The else-if chain is handled by ElseIfSequence and ElseIfStmt */
elseIfSequence:
      elseIfStmt
    | elseIfSequence elseIfStmt
         { $$ = alctoken(2050, "ElseIfSequence", 2, $1, $2); }
    ;

elseIfStmt:
      ELSE ifExpression
         { $$ = alctoken(2060, "ElseIfStmt", 2, $2); }
    ;
whenExpression:
    WHEN LCURL RCURL                                                                    {$$ = alctoken(1092, "whenNoSubNoEnt", 1, $1);}
    | WHEN LCURL whenEntries RCURL                                                      {$$ = alctoken(1093, "whenEnt", 2, $1, $3);}
    | WHEN whenSubject LCURL RCURL                                                      {$$ = alctoken(1094, "whenSub", 1, $1, $2);}
    | WHEN whenSubject LCURL whenEntries RCURL                                          {$$ = alctoken(1095, "whenSubEnt", 3, $1, $2, $3);}
    ;

whenSubject:
    LPAREN expression RPAREN                                                            {$$ = alctoken(1096, "whenSubExp", 1, $2);}
    | LPAREN VAL variableDeclaration ASSIGNMENT expression RPAREN                       {$$ = alctoken(1097, "whenSubVar", 4, $2, $3, $4, $5);}
    ;

whenEntries:
    whenEntry whenEntries                                                              {$$ = alctoken(1098, "whenEntries", 2, $1, $2);}
    | whenEntry                                                                         {$$ = $1;}
    ;

whenEntry:
    whenConditionList ARROW controlStructureBody SEMICOLON                              {$$ = alctoken(1099, "whenEntryConds", 3, $1, $2, $3);}
    | ELSE ARROW controlStructureBody SEMICOLON                                         {$$ = alctoken(1100, "whenEntryElse", 3, $1, $2, $3);}
    ;

whenConditionList:
    whenCondition COMMA whenConditionList                                        {$$ = alctoken(1101, "whenConds", 2, $1, $3);}
    | whenCondition                                                                     {$$ = $1;}
    ;

whenCondition:
    expression                                                                          {$$ = $1;}
    | IN expression                                                                     {$$ = alctoken(1102, "whenCondsIn", 2, $1, $2);}
    ;

controlStructureBody:
    block                                                                               {$$ = $1;}
    | statement                                                                         {$$ = $1;}
    ;

jumpExpression: // SEMICOLON added for shift/reduce conflict. Exclude in semantic value?
    RETURN SEMICOLON                                                                    {$$ = $1;}
    | RETURN expression                                                                 {$$ = alctoken(1103, "returnVal", 2, $1, $2);}
    | CONTINUE                                                                          {$$ = $1;}
    | BREAK                                                                             {$$ = $1;}
    ;

%%
const char *yyname(int sym)
{
   return yytname[sym - YYerror + 1];
}