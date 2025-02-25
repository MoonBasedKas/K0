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
    TYPE,
    USERTYPE,
    SIMPLEUSERTYPE,
    TYPEARGUMENTSLIST,
    TYPEARGUMENT,
    RECIVERTYPE,
    FUNCTIONTYPE,
    FUNCTIONTYPEPARAMETERS,
    FUNCTIONTYPEPARAMLIST,
    PARENTHESIZEDTYPE_OPT,
    QUESTS,
    QUEST,
    FUNCTIONBODY,
    BLOCK,
    STATEMENTS,
    STATEMENT,
    ASSIGNMENT,
    LOOPSTATEMENT,
    FORSTATEMENT,
    WHILESTATEMENT,
    DOWHILESTATEMENT,
    VARIABLEDECLARATIONS,
    VARIABLEDECLARATION,
    VARIABLEDECLARATION
    MULTIVARIABLEDECLARATION,
    VARIABLEDECLARATIONLIST


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
    topLevelObjectList {root = $$ = $1;}
    ;

topLevelObjectList:
    topLevelObject {$$ = $1;}
    | topLevelObjectList topLevelObject {$$ = alctoken(TOPLEVELOBJECT, "topLevelObject", 2, $1, $2);}
    ;

topLevelObject:
    declaration {$$ = $1;}
    ;

declaration:
    functionDeclaration {$$ = $1;}
    | propertyDeclaration {$$ = $1;}
    ;

propertyDeclaration: // Do we need modifiers here? (modifier->propertyModifier->const)
    variable variableDeclaration SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration", 3, $1, $2, $3);}
    | variable variableDeclaration assignment SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration", 4, $1, $2, $3, $4);}
    | variable reciverType variableDeclaration SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration", 4, $1, $2, $3, $4);}
    | variable reciverType variableDeclaration assignment SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration", 5, $1, $2, $3, $4, $5);}
    | variable typeParameters variableDeclaration SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration", 4, $1, $2, $3, $4);}
    | variable typeParameters variableDeclaration assignment SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration", 5, $1, $2, $3, $4, $5);}
    | variable typeParameters reciverType variableDeclaration SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration", 5, $1, $2, $3, $4, $5);}
    | variable typeParameters reciverType variableDeclaration assignment SEMICOLON {$$ = alctoken(PROPERTYDECLARATION, "propertyDeclaration" 6, , $1, $2, $3, $4, $5, $6);}

variable:
    VAL {$$ = $1;}
    | VAR {$$ = $1;}
    ;

typeParameters:
    LANGLE variableDeclarationList RANGLE {$$ = alctoken(TYPEPARAMETERS, "typeParameters", 3, $1, $2, $3);}
    ;


functionDeclaration:
    FUN IDENTIFIER functionValueParameters COLON type functionBody {$$ = alctoken(FUNCTIONDECLARATION, "functionDeclaration", 6, $1, $2, $3, $4, $5, $6);}
    | FUN IDENTIFIER functionValueParameters COLON type {$$ = alctoken(FUNCTIONDECLARATION, "functionDeclaration", 5, $1, $2, $3, $4, $5);}
    | FUN IDENTIFIER functionValueParameters functionBody {$$ = alctoken(FUNCTIONDECLARATION, "functionDeclaration", 4, $1,$2,$3,$4);}
    ;

functionValueParameters:
    LPAREN functionValueParamList RPAREN {$$ = alctoken(FUNCTIONVALUEPARAMETERS, "functionValueParameters", 3, $1, $2, $3);}
    ;

functionValueParamList:
    functionValueParameter COMMA functionValueParamList {$$ = alctoken(FUNCTIONVALUEPARAMLIST, "functionValueParamList", 3, $1, $2, $3);}
    | functionValueParameter {$$ = $1;}
    ;

functionValueParameter:
    variableDeclaration {$$ = $1;}
    | variableDeclaration ASSIGNMENT expression {$$ = alctoken(FUNCTIONVALUEPARAMETER, "functionValueParameter", 3, $1, $2, $3);}
    ;

type:
    functionType {$$ = $1;}
    | parenthesizedType_opt {$$ = $1;}
    | userType {$$ = $1;}
    ;

userType:
    simpleUserType {$$ = $1;}
    | userType DOT simpleUserType {$$ = alctoken(USERTYPE, "userType", 3, $1, $2, $3);}
    ;

simpleUserType:
    IDENTIFIER {$$ = $1;}
    | IDENTIFIER LANGLE typeArgumentsList RANGLE {$$ = alctoken(SIMPLEUSERTYPE, "simpleUserType", 4, $1, $2, $3, $4);}
    ;

typeArgumentsList:
    typeArgument {$$ = $1;}
    | typeArgument COMMA typeArgumentsList {$$ = alctoken(TYPEARGUMENTSLIST, "typeArgumentsList", 3, $1, $2, $3);}
    ;

typeArgument:
    type {$$ = $1;}
    | MULT {$$ = $1;}
    ;

reciverType:
    parenthesizedType_opt {$$ = $1;}
    ;

functionType:
    reciverType DOT functionTypeParameters ARROW type {$$ = alctoken(FUNCTIONTYPE, "functionType", 5, $1, $2, $3, $4, $5);}
    | functionTypeParameters ARROW type {$$ = alctoken(FUNCTIONTYPE, "functionType", 3, $1, $2, $3);}
    ;

functionTypeParameters:
    LPAREN functionTypeParamList RPAREN {$$ = alctoken(FUNCTIONTYPEPARAMETERS, "functionTypeParameters", 3, $1, $2, $3);}
    | LPAREN RPAREN {$$ = alctoken(FUNCTIONTYPEPARAMETERS, "functionTypeParameters", 2, $1, $2);}
    ;

functionTypeParamList:
    functionTypeParameter COMMA functionTypeParamList {$$ = alctoken(FUNCTIONTYPEPARAMLIST, "functionTypeParamList", 3, $1, $2, $3);}
    | functionTypeParameter {$$ = $1;}
    ;

functionTypeParameter:
    variableDeclaration {$$ = $1;}
    | type {$$ = $1;}
    ;

parenthesizedType_opt:
    LPAREN type RPAREN quests {$$ = alctoken(PARENTHESIZEDTYPE_OPT, "parenthesizedType_opt", 4, $1, $2, $3, $4);}
    | LPAREN type RPAREN {$$ = alctoken(PARENTHESIZEDTYPE_OPT, "parenthesizedType_opt", 3, $1, $2, $3);}
    ;

quests:
    quest {$$ = $1;}
    | quests quest {$$ = alctoken(QUESTS, "quests", 2, $1, $2);}
    ;

quest:
    QUEST_NO_WS {$$ = $1;}
    | QUEST_WS {$$ = $1;}
    ;

functionBody:
    block {$$ = $1;}
    | ASSIGNMENT expression {$$ = alctoken(FUNCTIONBODY, "functionBody", 2, $1, $2);}
    ;

block:
    LCURL RCURL {$$ = alctoken(BLOCK, "block", 2, $1, $2);}
    | LCURL statements RCURL {$$ = alctoken(BLOCK, "block", 3, $1, $2, $3);}
    ;

statements:
    statement SEMICOLON {$$ = alctoken(STATEMENTS, "block", 2, $1, $2);}
    | statements SEMICOLON statement {$$ = alctoken(STATEMENTS, "block", 3, $1, $2, $3);}
    | SEMICOLON {$$ = $1;}
    ;

statement:
    declaration {$$ = $1;}
    | loopStatement {$$ = $1;}
    | expression {$$ = $1;}
    ;

assignment:
    IDENTIFIER ASSIGNMENT expression {$$ = alctoken(ASSIGNMENT, "assignment", 3, $1, $2, $3);}
    | IDENTIFIER ADD_ASSIGNMENT expression {$$ = alctoken(ASSIGNMENT, "assignment", 3, $1, $2, $3);}
    | IDENTIFIER SUB_ASSIGNMENT expression {$$ = alctoken(ASSIGNMENT, "assignment", 3, $1, $2, $3);}
    ;

loopStatement:
    forStatement {$$ = $1;}
    | whileStatement {$$ = $1;}
    | doWhileStatement {$$ = $1;}
    ;

forStatement:
    FOR LPAREN variableDeclarations IN expression RPAREN controlStructureBody {$$ = alctoken(FORSTATEMENT, "forStatement", 7, $1, $2, $3, $4, $5, $6, $7);}
    ;

whileStatement:
    WHILE LPAREN expression RPAREN controlStructureBody {$$ = alctoken(WHILESTATEMENT, "whileStatement", 5, $1, $2, $3, $4, $5);}
    | WHILE LPAREN expression RPAREN SEMICOLON {$$ = alctoken(WHILESTATEMENT, "whileStatement", 5, $1, $2, $3, $4, $5);}
    ;

doWhileStatement:
    DO controlStructureBody WHILE LPAREN expression RPAREN {$$ = alctoken(DOWHILESTATEMENT, "doWhileStatement", 6, $1, $2, $3, $4, $5, $6);}
    ;

variableDeclarations:
    variableDeclaration {$$ = $1;}
    | multiVariableDeclaration {$$ = $1;}
    ;

variableDeclaration:
    IDENTIFIER COLON type {$$ = alctoken(VARIABLEDECLARATION, "variableDeclaration", 3, $1, $2, $3);}
    ;

multiVariableDeclaration:
    LPAREN variableDeclarationList RPAREN {$$ = alctoken(MULTIVARIABLEDECLARATION, "multiVariableDeclaration", 3, $1, $2, $3);}
    ;
    
variableDeclarationList:
    variableDeclaration {$$ = $1}
    | variableDeclarationList COMMA variableDeclaration {$$ = alctoken(VARIABLEDECLARATIONLIST, "variableDeclarationList", 3, $1, $2, $3)}
    ;

expression:
    disjunction                         {$$ = $1;}
    ;

disjunction:
    disjunction DISJ conjunction        {$$ = alctoken(1000, "disj", 3, $1, $2, $3);}
    | conjunction                       {$$ = $1;}
    ;

conjunction:
    conjunction CONJ equality           {$$ = alctoken(1001, "conj", 3, $1, $2, $3);}
    | equality                          {$$ = $1;}
    ;

equality:
    equality EQEQ comparison            {$$ = alctoken(1002, "equal", 3, $1, $2, $3);}
    | equality EXCL_EQ comparison       {$$ = alctoken(1003, "notEqual", 3, $1, $2, $3);}
    | equality EQEQEQ comparison        {$$ = alctoken(1004, "eqeqeq", 3, $1, $2, $3);}
    | equality EXCL_EQEQ comparison     {$$ = alctoken(1005, "not-eqeqeq", 3, $1, $2, $3);}
    | comparison                        {$$ = $1;}
    ;

comparison:
    comparison LANGLE infixOperation    {$$ = alctoken(1006, "less", 3, $1, $2, $3);}
    | comparison RANGE infixOperation   {$$ = alctoken(1007, "greater", 3, $1, $2, $3);}
    | comparison GE infixOperation      {$$ = alctoken(1008, "lessEqual", 3, $1, $2, $3);}
    | comparison LE infixOperation      {$$ = alctoken(1009, "greaterEqual", 3, $1, $2, $3);}
    | infixOperation                    {$$ = $1;}
    ;   

infixOperation:
    infixOperation IN elvisExpression   {$$ = alctoken(1010, "in", 3, $1, $2, $3);}
    | elvisExpression                   {$$ = $1;}
    ;

elvisExpression:
    elvisExpression QUEST_NO_WS COLON infixFunctionCall     {$$ = alctoken(1011, "elvis", 3, $1, $2, $3);}
    | infixFunctionCall                                     {$$ = $1;}
    ;

infixFunctionCall:
    infixFunctionCall IDENTIFIER rangeExpression            {$$ = alctoken(1012, "infixFunction", 3, $1, $2, $3);}
    | rangeExpression                                       {$$ = $1;}
    ;   

rangeExpression:
    rangeExpression RANGE additiveExpression                {$$ = alctoken(1013, "range", 3, $1, $2, $3);}
    | rangeExpression RANGE_UNTIL additiveExpression        {$$ = alctoken(1014, "rangeUntil", 3, $1, $2, $3);}
    | additiveExpression                                    {$$ = $1;}  
    ;

additiveExpression:
    additiveExpression ADD multiplicativeExpression         {$$ = alctoken(1015, "add", 3, $1, $2, $3);}
    | additiveExpression SUB multiplicativeExpression       {$$ = alctoken(1016, "sub", 3, $1, $2, $3);}
    | multiplicativeExpression                              {$$ = $1;} 
    ;

multiplicativeExpression:
    multiplicativeExpression MULT prefixUnaryExpression     {$$ = alctoken(1017, "mult", 3, $1, $2, $3);}
    | multiplicativeExpression DIV prefixUnaryExpression    {$$ = alctoken(1018, "div", 3, $1, $2, $3);}
    | multiplicativeExpression MOD prefixUnaryExpression    {$$ = alctoken(1019, "mod", 3, $1, $2, $3);}
    | prefixUnaryExpression                                 {$$ = $1;} 
    ;

prefixUnaryExpression:
    postfixUnaryExpression                                  {$$ = $1;} 
    | prefixUnaryOperator postfixUnaryExpression            {$$ = alctoken(1020, "prefix", 2, $1, $2);}
    ;

prefixUnaryOperator:
    INCR                {$$ = $1;}
    | DECR              {$$ = $1;}
    | SUB               {$$ = $1;}
    | ADD               {$$ = $1;}
    | excl              {$$ = $1;}
    ;

postfixUnaryExpression:
    primaryExpression postfixUnaryOperator      {$$ = alctoken(1021, "postfix", 2, $1, $2);}
    | primaryExpression                         {$$ = $1;}
    ;

postfixUnaryOperator:
    INCR                                        {$$ = $1;}
    | DECR                                      {$$ = $1;}
    | EXCL_NO_WS excl                           {$$ = $1;}
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
    | IDENTIFIER LPAREN expressionList RPAREN   {$$ = alctoken(1022, "funCallParams", 2, $1, $3);}
    | IDENTIFIER LPAREN RPAREN                  {$$ = alctoken(1023, "funCallNoParams", 1, $1);}
    ;

expressionList:
    expression COMMA expression                 {$$ = alctoken(1024, "expressionList", 2, $1, $3);}
    | expression                                {$$ = $1;}
    ;

parenthesizedExpression:
    LPAREN expression RPAREN                    {$$ = alctoken(1025, "parenthesizedExpression", 1, $2);}
    ;

ifExpression:
    IF LPAREN expression RPAREN SEMICOLON                                               {$$ = alctoken(1026, "emptyIf", 2, $1, $3);}
    | IF LPAREN expression RPAREN controlStructureBody                                  {$$ = alctoken(1027, "if", 3, $1, $3, $5);}
    | IF LPAREN expression RPAREN controlStructureBody ELSE controlStructureBody        {$$ = alctoken(1028, "ifElse", 5, $1, $3, $5, $6, $7);}
    ;

whenExpression:
    WHEN LCURL RCURL                                                                    {$$ = alctoken(1029, "whenNoSubNoEnt", 1, $1);}
    | WHEN LCURL whenEntries RCURL                                                      {$$ = alctoken(1030, "whenEnt", 2, $1, $3);}
    | WHEN whenSubject LCURL RCURL                                                      {$$ = alctoken(1031, "whenSub", 1, $1, $2);}
    | WHEN whenSubject LCURL whenEntries RCURL                                          {$$ = alctoken(1032, "whenSubEnt", 3, $1, $2, $3);}
    ;

whenSubject:
    LPAREN expression RPAREN                                                            {$$ = alctoken(1033, "whenSubExp", 1, $2);}
    | LPAREN VAL variableDeclaration ASSIGNMENT expression RPAREN                       {$$ = alctoken(1034, "whenSubVar", 4, $2, $3, $4, $5);}
    ;

whenEntries:
    whenEntries whenEntry                                                               {$$ = alctoken(1035, "whenEntries", 2, $1, $2);}
    | whenEntry                                                                         {$$ = $1;}
    ;

whenEntry:
    whenConditionList ARROW controlStructureBody SEMICOLON                              {$$ = alctoken(1036, "whenEntryConds", 3, $1, $2, $3);}
    | ELSE ARROW controlStructureBody SEMICOLON                                         {$$ = alctoken(1037, "whenEntryElse", 3, $1, $2, $3);}
    ;

whenConditionList:
    whenConditionList COMMA whenCondition                                               {$$ = alctoken(1038, "whenConds", 2, $1, $3);}
    | whenCondition;                                                                    {$$ = $1;}

whenCondition:
    expression                                                                          {$$ = $1;}
    | IN expression                                                                     {$$ = alctoken(1039, "whenCondsIn", 2, $1, $2);}
    ;

controlStructureBody:
    block                                                                               {$$ = $1;}
    | statement                                                                         {$$ = $1;}
    ;

jumpExpression: // SEMICOLON added for shift/reduce conflict. Exclude in semantic value?
    RETURN SEMICOLON                                                                    {$$ = $1;}
    | RETURN expression                                                                 {$$ = alctoken(1038, "returnVal", 2, $1, $2);}
    | CONTINUE                                                                          {$$ = $1;}
    | BREAK                                                                             {$$ = $1;}
    ;