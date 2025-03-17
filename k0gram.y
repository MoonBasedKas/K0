%{
    #include <stdio.h>
    #include "lex.h"
    #include "tree.h"
    #include "symNonTerminals.h"

    #define YYDEBUG 1
%}
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
%type <treeptr> arraySize
%type <treeptr> arrayValues
%type <treeptr> arrayDeclaration
%type <treeptr> arrayIndex
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
    topLevelObjectList      {root = $$ = alctoken(program, "program", 1, $1);}
    ;

topLevelObjectList:
    topLevelObject                                  {$$ = $1;}
    | topLevelObject SEMICOLON                      {$$ = $1;}
    | topLevelObjectList topLevelObject             {$$ = alctoken(topLevelObject, "topLevelObject", 2, $1, $2);}
    | topLevelObjectList topLevelObject SEMICOLON   {$$ = alctoken(topLevelObjectSemi, "topLevelObjectSemi", 2, $1, $2);}
    ;

topLevelObject:
    declaration     {$$ = $1;}
    | importList    {$$ = $1;}
    ;

importList:
    IMPORT importIdentifier                 {$$ = alctoken(collapsedImport, "collapsedImport", 2, $1, $2);}
    | IMPORT importIdentifier importList    {$$ = alctoken(expandingImport, "expandingImport", 3, $1, $2, $3);}
    ;

importIdentifier:
    IDENTIFIER DOT importIdentifier     {$$ = alctoken(expandingImportID, "expandingImportID", 3, $1, $2, $3);}
    | IDENTIFIER                        {$$ = $1;}
    | MULT                              {$$ = $1;}
    ;

declaration:
    functionDeclaration     {$$ = $1;}
    | propertyDeclaration   {$$ = $1;}
    | assignment            {$$ = $1;}
    ;

propertyDeclaration:
    variable variableDeclaration                                                        {$$ = alctoken(propDecEmpty, "propDecEmpty", 2, $1, $2);}
    | variable IDENTIFIER ASSIGNMENT expression                                         {$$ = alctoken(propDecTypeless, "propDecTypeless", 3, $1, $2, $4);}
    | variable variableDeclaration ASSIGNMENT expression                                {$$ = alctoken(propDecAssign, "propDecAssign", 3, $1, $2, $4);}
    | variable reciverType variableDeclaration                                          {$$ = alctoken(propDecReceiver, "propDecReceiver", 3, $1, $2, $3);}
    | variable reciverType variableDeclaration ASSIGNMENT expression                    {$$ = alctoken(propDecReceiverAssign, "propDecReceiverAssign", 4, $1, $2, $3, $5);}
    | variable typeParameters variableDeclaration                                       {$$ = alctoken(propDecTypeParams, "propDecTypeParams", 4, $1, $2, $3);}
    | variable typeParameters variableDeclaration ASSIGNMENT expression                 {$$ = alctoken(propDecTypeParamsAssign, "propDecTypeParamsAssign", 4, $1, $2, $3, $5);}
    | variable typeParameters reciverType variableDeclaration                           {$$ = alctoken(propDecTypeParamsReceiver, "propDecTypeParamsReceiver", 4, $1, $2, $3, $4);}
    | variable typeParameters reciverType variableDeclaration ASSIGNMENT expression     {$$ = alctoken(propDecAll, "propDecAll", 5, $1, $2, $3, $4, $6);}
    | arrayDeclaration                                                                  {$$ = $1;}
    ;

arrayDeclaration:
    variable variableDeclaration arraySize arrayValues                                                  {$$ = alctoken(arrayDec, "arrayDec", 4, $1, $2, $3, $4);}
    | variable variableDeclaration arraySize                                                            {$$ = alctoken(arrayDecValueless, "arrayDecValueless", 3, $1, $2, $3);}
    | variable variableDeclaration ASSIGNMENT IDENTIFIER LANGLE IDENTIFIER RANGLE arraySize arrayValues {$$ = alctoken(arrayDecEqual, "arrayDecEqual", 6, $1, $2, $4, $6, $8, $9);}
    | variable variableDeclaration ASSIGNMENT IDENTIFIER LANGLE IDENTIFIER RANGLE arraySize             {$$ = alctoken(arrayDecEqualValueless, "arrayDecEqualValueless", 5, $1, $2, $4, $6, $8);}

arraySize:
    LPAREN INTEGER_LITERAL RPAREN   {$$ = alctoken(arraySizeInt, "arraySizeInt", 3, $1, $2, $3);}
    | LPAREN IDENTIFIER RPAREN      {$$ = alctoken(arraySizeIdent, "arraySizeIdent", 3, $1, $2, $3);}
    ;


arrayValues:
    LCURL expressionList RCURL      {$$ = alctoken(arrayValues, "arrayValues", 3, $1, $2, $3);}
    ;


variable:
    constantVal {$$ = $1;}
    | VAL       {$$ = $1;}
    | VAR       {$$ = $1;}
    ;

constantVal:
    CONST VAL   {$$ = alctoken(constantVal, "constantVal", 2, $1, $2);}
    ;

typeParameters:
    LANGLE variableDeclarationList RANGLE   {$$ = alctoken(typeParameters, "typeParameters", 1, $2);}
    ;

functionDeclaration:
    FUN IDENTIFIER functionValueParameters COLON type functionBody  {$$ = alctoken(funcDecAll, "funcDecAll", 5, $1, $2, $3, $5, $6);}
    | FUN IDENTIFIER functionValueParameters COLON type             {$$ = alctoken(funcDecParamType, "funcDecParamType", 4, $1, $2, $3, $5);}
    | FUN IDENTIFIER functionValueParameters functionBody           {$$ = alctoken(funcDecParamBody, "funcDecParamBody", 4, $1, $2, $3, $4);}
    | FUN IDENTIFIER LPAREN RPAREN COLON type functionBody          {$$ = alctoken(funcDecTypeBody, "funcDecTypeBody", 4, $1, $2, $6, $7);}
    | FUN IDENTIFIER LPAREN RPAREN COLON type                       {$$ = alctoken(funcDecType, "funcDecType", 3, $1, $2, $6);}
    | FUN IDENTIFIER LPAREN RPAREN functionBody                     {$$ = alctoken(funcDecBody, "funcDecBody", 3, $1, $2, $5);}
    ;

functionValueParameters:
    LPAREN functionValueParamList RPAREN    {$$ = alctoken(funcValParams, "funcValParams", 1, $2);}
    ;

functionValueParamList:
    functionValueParameter COMMA functionValueParamList     {$$ = alctoken(funcValParamList, "funcValParamList", 2, $1, $3);}
    | functionValueParameter                                {$$ = $1;}
    ;

functionValueParameter:
    variableDeclaration                             {$$ = $1;}
    | variableDeclaration ASSIGNMENT expression     {$$ = alctoken(funcValParamAssign, "funcValParamAssign", 2, $1, $3);}
    ;

type:
    functionType    {$$ = $1;}
    | userType      {$$ = $1;}
    ;

userType:
    simpleUserType                  {$$ = $1;}
    | userType DOT simpleUserType   {$$ = alctoken(userType, "userType", 3, $1, $2, $3);}
    ;

simpleUserType:
    IDENTIFIER LANGLE typeArgumentsList RANGLE  {$$ = alctoken(simpleUserType, "simpleUserType", 2, $1, $3);}
    | IDENTIFIER                                {$$ = $1;}
    ;

typeArgumentsList:
    typeArgument                            {$$ = $1;}
    | typeArgument COMMA typeArgumentsList  {$$ = alctoken(typeArgumentsList, "typeArgumentsList", 2, $1, $3);}
    ;

typeArgument:
    type    {$$ = $1;}
    | MULT  {$$ = $1;}
    ;

reciverType:
    parenthesizedType_opt   {$$ = $1;}
    ;

functionType:
    reciverType DOT functionTypeParameters ARROW type   {$$ = alctoken(functionTypeDot, "functionTypeDot", 5, $1, $2, $3, $4, $5);}
    | functionTypeParameters ARROW type                 {$$ = alctoken(functionType, "functionType", 3, $1, $2, $3);}
    ;

functionTypeParameters:
    LPAREN functionTypeParamList RPAREN     {$$ = alctoken(functionTypeParameters, "functionTypeParameters", 3, $1, $2, $3);}
    | LPAREN RPAREN                         {$$ = alctoken(functionTypeParamsEmpty, "functionTypeParamsEmpty", 2, $1, $2);}
    ;

functionTypeParamList:
    functionTypeParameter COMMA functionTypeParamList   {$$ = alctoken(functionTypeParamList, "functionTypeParamList", 2, $1, $3);}
    | functionTypeParameter                             {$$ = $1;}
    ;

functionTypeParameter:
    variableDeclaration {$$ = $1;}
    | type              {$$ = $1;}
    ;

parenthesizedType_opt:
    LPAREN type RPAREN quests   {$$ = alctoken(parenTypeQuests, "parenTypeQuests", 2, $2, $4);}
    | LPAREN type RPAREN        {$$ = alctoken(parenType, "parenType", 1, $2);}
    ;

quests:
    quest           {$$ = $1;}
    | quest quests  {$$ = alctoken(quests, "quests", 2, $1, $2);}
    ;

quest:
    QUEST_NO_WS     {$$ = $1;}
    | QUEST_WS      {$$ = $1;}
    ;

functionBody:
    block                       {$$ = $1;}
    | ASSIGNMENT expression     {$$ = alctoken(funcBody, "funcBody", 2, $1, $2);}
    ;

block:
    LCURL RCURL                 {$$ = alctoken(blockEmpty, "blockEmpty", 2, $1, $2);}
    | LCURL statements RCURL    {$$ = alctoken(blockStmnts, "blockStmnts", 3, $1, $2, $3);}
    ;

statements:
    statement SEMICOLON                 {$$ = alctoken(statement, "statement", 1, $1);}
    | statement SEMICOLON statements    {$$ = alctoken(statementsMultiple, "statementsMultiple", 2, $1, $3);}
    | SEMICOLON                         {$$ = $1;}
    ;

statement:
    declaration         {$$ = $1;}
    | loopStatement     {$$ = $1;}
    | expression        {$$ = $1;}
    ;

assignment:
    IDENTIFIER ASSIGNMENT expression        {$$ = alctoken(assignment, "assignment", 2, $1, $3);}
    | IDENTIFIER ADD_ASSIGNMENT expression  {$$ = alctoken(assignAdd, "assignAdd", 2, $1, $3);}
    | IDENTIFIER SUB_ASSIGNMENT expression  {$$ = alctoken(assignSub, "assignSub", 2, $1, $3);}
    | arrayIndex ASSIGNMENT expression        {$$ = alctoken(assignment, "assignment", 2, $1, $3);}
    | arrayIndex ADD_ASSIGNMENT expression  {$$ = alctoken(assignAdd, "assignAdd", 2, $1, $3);}
    | arrayIndex SUB_ASSIGNMENT expression  {$$ = alctoken(assignSub, "assignSub", 2, $1, $3);}
    ;

arrayIndex:
    IDENTIFIER LSQUARE INTEGER_LITERAL RSQUARE {$$ = alctoken(assignSub, "assignSub", 2, $1, $3);}
    ;


loopStatement:
    forStatement        {$$ = $1;}
    | whileStatement    {$$ = $1;}
    | doWhileStatement  {$$ = $1;}
    ;

forStatement:
    FOR LPAREN variableDeclarations IN expression RPAREN controlStructureBody   {$$ = alctoken(forStmntWithVars, "forStmntWithVars", 4, $1, $3, $5, $7);}
    | FOR LPAREN IDENTIFIER IN expression RPAREN controlStructureBody           {$$ = alctoken(forStmnt, "forStmnt", 4, $1, $3, $5, $7);}
    ;

whileStatement:
    WHILE LPAREN expression RPAREN controlStructureBody     {$$ = alctoken(whileStmntCtrlBody, "whileStmntCtrlBody", 3, $1, $3, $5);}
    | WHILE LPAREN expression RPAREN SEMICOLON              {$$ = alctoken(whileStmnt, "whileStmnt", 2, $1, $3);}
    ;

doWhileStatement:
    DO controlStructureBody WHILE LPAREN expression RPAREN  {$$ = alctoken(doWhileStmnt, "doWhileStmnt", 4, $1, $2, $3, $5);}
    ;

variableDeclarations:
    variableDeclaration         {$$ = $1;}
    | multiVariableDeclaration  {$$ = $1;}
    ;

variableDeclaration:
    IDENTIFIER COLON type           {$$ = alctoken(varDec, "varDec", 2, $1, $3);}
    | IDENTIFIER COLON type quests  {$$ = alctoken(varDecQuests, "varDecQuests", 3, $1, $3, $4);}
    ;

multiVariableDeclaration:
    LPAREN variableDeclarationList RPAREN   {$$ = $2;}
    ;

variableDeclarationList:
    variableDeclaration                                     {$$ = $1;}
    | variableDeclarationList COMMA variableDeclaration     {$$ = alctoken(varDecList, "varDecList", 2, $1, $3);}
    ;

expression:
    disjunction                         {$$ = $1;}
    ;

disjunction:
    disjunction DISJ conjunction        {$$ = alctoken(disj, "disj", 2, $1, $3);}
    | conjunction                       {$$ = $1;}
    ;

conjunction:
    conjunction CONJ equality           {$$ = alctoken(conj, "conj", 2, $1, $3);}
    | equality                          {$$ = $1;}
    ;

equality:
    equality EQEQ comparison            {$$ = alctoken(equal, "equal", 2, $1, $3);}
    | equality EXCL_EQ comparison       {$$ = alctoken(notEqual, "notEqual", 2, $1, $3);}
    | equality EQEQEQ comparison        {$$ = alctoken(eqeqeq, "eqeqeq", 2, $1, $3);}
    | equality EXCL_EQEQ comparison     {$$ = alctoken(notEqeqeq, "notEqeqeq", 2, $1, $3);}
    | comparison                        {$$ = $1;}
    ;

comparison:
    comparison LANGLE infixOperation    {$$ = alctoken(less, "less", 2, $1, $3);}
    | comparison RANGLE infixOperation  {$$ = alctoken(greater, "greater", 2, $1, $3);}
    | comparison GE infixOperation      {$$ = alctoken(lessEqual, "lessEqual", 2, $1, $3);}
    | comparison LE infixOperation      {$$ = alctoken(greaterEqual, "greaterEqual", 2, $1, $3);}
    | infixOperation                    {$$ = $1;}
    ;

infixOperation:
    infixOperation IN elvisExpression   {$$ = alctoken(in, "in", 2, $1, $3);}
    | elvisExpression                   {$$ = $1;}
    ;

elvisExpression:
    elvisExpression QUEST_NO_WS COLON infixFunctionCall     {$$ = alctoken(elvis, "elvis", 2, $1, $4);}
    | infixFunctionCall                                     {$$ = $1;}
    ;

infixFunctionCall:
    infixFunctionCall IDENTIFIER rangeExpression            {$$ = alctoken(infixFunction, "infixFunction", 3, $1, $2, $3);}
    | rangeExpression                                       {$$ = $1;}
    ;

rangeExpression:
    additiveExpression RANGE rangeExpression                {$$ = alctoken(range, "range", 2, $1, $3);}
    | additiveExpression RANGE_UNTIL rangeExpression        {$$ = alctoken(rangeUntil, "rangeUntil", 2, $1, $3);}
    | additiveExpression                                    {$$ = $1;}
    ;

additiveExpression:
    additiveExpression ADD multiplicativeExpression         {$$ = alctoken(add, "add", 2, $1, $3);}
    | additiveExpression SUB multiplicativeExpression       {$$ = alctoken(sub, "sub", 2, $1, $3);}
    | multiplicativeExpression                              {$$ = $1;}
    ;

multiplicativeExpression:
    multiplicativeExpression MULT prefixUnaryExpression     {$$ = alctoken(mult, "mult", 2, $1, $3);}
    | multiplicativeExpression DIV prefixUnaryExpression    {$$ = alctoken(div_k, "div", 2, $1, $3);}
    | multiplicativeExpression MOD prefixUnaryExpression    {$$ = alctoken(mod, "mod", 2, $1, $3);}
    | prefixUnaryExpression                                 {$$ = $1;}
    ;

prefixUnaryExpression:
    prefixUnaryOperator prefixUnaryExpression               {$$ = alctoken(prefix, "prefix", 2, $1, $2);}
    | postfixExpression                                     {$$ = $1;}
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
    | ifExpression                              {$$ = $1;}
    | whenExpression                            {$$ = $1;}
    | jumpExpression                            {$$ = $1;}
    | IDENTIFIER LSQUARE expression RSQUARE     {$$ = alctoken(arrayAccess, "arrayAccess", 2, $1, $3);}
    ;

postfixExpression:
    primaryExpression {$$ = $1;}
    | postfixExpression LPAREN expressionList RPAREN                                {$$ = alctoken(postfixExpr, "postfixExpr", 2, $1, $3);}
    | postfixExpression LPAREN RPAREN                                               {$$ = alctoken(postfixNoExpr, "postfixNoExpr", 1, $1);}
    | postfixExpression DOT IDENTIFIER                                              {$$ = alctoken(postfixDotID, "postfixDotID", 3, $1, $2, $3);}
    | postfixExpression DOT IDENTIFIER LPAREN expressionList RPAREN                 {$$ = alctoken(postfixDotIDExpr, "postfixDotIDExpr", 4, $1, $2, $3, $5);}
    | postfixExpression DOT IDENTIFIER LPAREN RPAREN                                {$$ = alctoken(postfixDotIDNoExpr, "postfixDotIDNoExpr", 3, $1, $2, $3);}
    | postfixExpression QUEST_NO_WS DOT IDENTIFIER                                  {$$ = alctoken(postfixSafeDotID, "postfixSafeDotID", 4, $1, $2, $3, $4);}
    | postfixExpression QUEST_NO_WS DOT IDENTIFIER LPAREN expressionList RPAREN     {$$ = alctoken(postfixSafeDotIDExpr, "postfixSafeDotIDExpr", 5, $1, $2, $3, $4, $6);}
    | postfixExpression QUEST_NO_WS DOT IDENTIFIER LPAREN RPAREN                    {$$ = alctoken(postfixSafeDotIDNoExpr, "postfixSafeDotIDNoExpr", 4, $1, $2, $3, $4);}
    | postfixExpression LSQUARE expression RSQUARE                                  {$$ = alctoken(postfixArrayAccess, "postfixArrayAccess", 2, $1, $3);}
    | postfixExpression INCR                                                        {$$ = alctoken(postfixIncr, "postfixIncr", 1, $1);}
    | postfixExpression DECR                                                        {$$ = alctoken(postfixDecr, "postfixDecr", 1, $1);}
    ;

expressionList:
    expression COMMA expressionList             {$$ = alctoken(expressionList, "expressionList", 2, $1, $3);}
    | expression                                {$$ = $1;}
    ;

parenthesizedExpression:
    LPAREN expression RPAREN                    {$$ = $2;}
    ;

ifExpression:
    IF LPAREN expression RPAREN SEMICOLON                 {$$ = alctoken(emptyIf, "emptyIf", 2, $1, $3);}
    | IF LPAREN expression RPAREN block                   {$$ = alctoken(if_k, "if", 3, $1, $3, $5);}
    | IF LPAREN expression RPAREN block ELSE block        {$$ = alctoken(ifElse, "ifElse", 5, $1, $3, $5, $6, $7);}
    ;

whenExpression:
    WHEN LCURL RCURL                                                                    {$$ = alctoken(whenNoSubNoEnt, "whenNoSubNoEnt", 1, $1);}
    | WHEN LCURL whenEntries RCURL                                                      {$$ = alctoken(whenEnt, "whenEnt", 2, $1, $3);}
    | WHEN whenSubject LCURL RCURL                                                      {$$ = alctoken(whenSub, "whenSub", 1, $1, $2);}
    | WHEN whenSubject LCURL whenEntries RCURL                                          {$$ = alctoken(whenSubEnt, "whenSubEnt", 3, $1, $2, $3);}
    ;

whenSubject:
    LPAREN expression RPAREN                                                            {$$ = alctoken(whenSubExp, "whenSubExp", 1, $2);}
    | LPAREN VAL variableDeclaration ASSIGNMENT expression RPAREN                       {$$ = alctoken(whenSubVar, "whenSubVar", 4, $2, $3, $4, $5);}
    ;

whenEntries:
    whenEntry whenEntries                                                              {$$ = alctoken(whenEntries, "whenEntries", 2, $1, $2);}
    | whenEntry                                                                        {$$ = $1;}
    ;

whenEntry:
    whenConditionList ARROW controlStructureBody SEMICOLON                              {$$ = alctoken(whenEntryConds, "whenEntryConds", 3, $1, $2, $3);}
    | ELSE ARROW controlStructureBody SEMICOLON                                         {$$ = alctoken(whenEntryElse, "whenEntryElse", 3, $1, $2, $3);}
    ;

whenConditionList:
    whenCondition COMMA whenConditionList                                               {$$ = alctoken(whenConds, "whenConds", 2, $1, $3);}
    | whenCondition                                                                     {$$ = $1;}
    ;

whenCondition:
    expression                                                                          {$$ = $1;}
    | IN expression                                                                     {$$ = alctoken(whenCondsIn, "whenCondsIn", 2, $1, $2);}
    ;

controlStructureBody:
    block                   {$$ = $1;}
    | statement SEMICOLON   {$$ = alctoken(controlStmnt, "controlStmnt", 1, $1);}
    ;

jumpExpression: // SEMICOLON added for shift/reduce conflict. Exclude in semantic value?
    RETURN                                                                              {$$ = $1;}
    | RETURN expression                                                                 {$$ = alctoken(returnVal, "returnVal", 2, $1, $2);}
    | CONTINUE                                                                          {$$ = $1;}
    | BREAK                                                                             {$$ = $1;}
    ;

%%
const char *yyname(int sym)
{
   return yytname[sym - YYerror + 1];
}