/* -*-bison-*- */
/* Simple wc implementation in Flex */
%option noyywrap
%option noinput
%option nounput
%{
			#include "k0gram.tab.h"
            #include "tree.h"
            #include "lex.h"
			int rows = 1;
            struct token *nextToken;
            struct token *prevToken = NULL;



%}

new_line				\n
white_space  			[ \t\r]+
line_comment			"//".*
shebang_line			"#!".*
delimited_comment		"/*"([^*]|"*"+[^/*])*"*"+"/"
nested_comment          "/*"([^*]|"*"+[^/*])*"/*"([^*]|"*"+[^/*])*"*"+"/"([^*]|"*"+[^/*])*"*"+"/"
hidden					{delimited_comment}|{line_comment}|{white_space}

dec_digit_no_zero		[1-9]
dec_digit				[0-9]
dec_digit_or_seperator	{dec_digit}|"_"
dec_digits 				({dec_digit}{dec_digit_or_seperator}*{dec_digit})|{dec_digit}
integer_literal			({dec_digit_no_zero}{dec_digit_or_seperator}*{dec_digit})|{dec_digit}

double_exponent			(e|E)("+"|-)?{dec_digits}
exponent_literal        ({dec_digits}?"."{dec_digits}{double_exponent})|({dec_digits}{double_exponent})
double_literal			{dec_digits}?"."{dec_digits}
float_literal			({double_literal}|{dec_digits})(f|F)

hex_digit				{dec_digit}|[a-fA-F]
hex_digit_or_seperator	{hex_digit}|"_"
hex_literal				0(x|X)({hex_digit}{hex_digit_or_seperator}*{hex_digit}|{hex_digit})

bin_digit				[0-1]
bin_digit_or_seperator	{bin_digit}|"_"
bin_literal				0(b|B)({bin_digit}{bin_digit_or_seperator}*{bin_digit}|{bin_digit})

unsigned_literal		({integer_literal}|{hex_literal}|{bin_literal})(u|U)L?
long_literal			({integer_literal}|{hex_literal}|{bin_literal})L

escape_seq				\\(t|b|r|n|"'"|\"|\\|"$"|0)
uni_character_literal	\\u{hex_digit}{hex_digit}{hex_digit}{hex_digit}
character_literal		"'"(escape_seq|[^\n\t '\\])"'"
character_error			"'"(escape_seq|[^\n\t '\\])(escape_seq|[^\n\t '\\])+"'"

letter					[a-zA-Z_]
identifier				{letter}({letter}|{dec_digit})*
backtick_identifier		"`"[^\n]+"`"

quote					\"
triple_quote			\"\"\"
field_identifier		"$"{identifier}

line_str_text			[^\\"$\n]+|$
line_string 			{quote}({field_identifier}|{line_str_text}|{escape_seq})*{quote}

multi_line_string_quote {triple_quote}\"*
multi_line_string_text  [^"$]*|$
multi_line_string       {triple_quote}({field_identifier}|{multi_line_string_text})*{multi_line_string_quote}?{triple_quote}

directive               "# "{integer_literal}" "\"{identifier}\"

%%

{new_line}										{ rows++; if (addSemi() == 1){return leaf(SEMICOLON);} }

{white_space}									{ /* Discard white space */ }
{line_comment}          						{ /* Discard comments */  }
{delimited_comment}								{ countNewLines(); /* Discard comments */ }
{shebang_line}									{ /* Discard comments */  }

    /* Primitive Types */
"Byte"											{ return leaf(BYTE); }
"Short"											{ return leaf(SHORT); }
"Int"											{ return leaf(INT); }
"Long"											{ return leaf(LONG); }
"Float"											{ return leaf(FLOAT); }
"Double"										{ return leaf(DOUBLE); }
"Boolean"										{ return leaf(BOOLEAN); }
"Char"											{ return leaf(CHAR); }
"String"										{ return leaf(STRING); }

"="												{ return leaf(ASSIGNMENT); }
"+="											{ return leaf(ADD_ASSIGNMENT); }
"-="											{ return leaf(SUB_ASSIGNMENT); }
"+"												{ return leaf(ADD); }
"-"												{ return leaf(SUB); }
"*"												{ return leaf(MULT); }
"/"												{ return leaf(DIV); }
"%"												{ return leaf(MOD); }
"++"											{ return leaf(INCR); }
"--"											{ return leaf(DECR); }
"=="											{ return leaf(EQEQ); }
"!="											{ return leaf(EXCL_EQ); }
"<"												{ return leaf(LANGLE); }
">"												{ return leaf(RANGLE); }
"<="											{ return leaf(LE); }
">="											{ return leaf(GE); }
"==="											{ return leaf(EQEQEQ); }
"!=="											{ return leaf(EXCL_EQEQ); }
"||"											{ return leaf(DISJ); }
"&&"											{ return leaf(CONJ); }
"!"												{ return leaf(EXCL_NO_WS); }
"!"hidden										{ return leaf(EXCL_WS); }
"?"{hidden}										{ return leaf(QUEST_WS); }
"?"												{ return leaf(QUEST_NO_WS); }
".."											{ return leaf(RANGE); }
"..<"											{ return leaf(RANGE_UNTIL); }
"."												{ return leaf(DOT); }
","												{ return leaf(COMMA); }
"("												{ return leaf(LPAREN); }
")"												{ return leaf(RPAREN); }
"["												{ return leaf(LSQUARE); }
"]"												{ return leaf(RSQUARE); }
"{"												{ return leaf(LCURL); }
"}"												{ return leaf(RCURL); }
":"												{ return leaf(COLON); }
";"												{ return leaf(SEMICOLON); }

"*="											{ unsupportedOperator(); }
"/="											{ unsupportedOperator(); }
"%="											{ unsupportedOperator(); }

"#"												{ unsupportedOperator(); }
"@"												{ unsupportedOperator(); }
"@"({hidden}|{new_line})						{ unsupportedOperator(); }
({hidden}|{new_line})"@"						{ unsupportedOperator(); }
({hidden}|{new_line})"@"({hidden}|{new_line})	{ unsupportedOperator(); }
"return@"{identifier}							{ unsupportedOperator(); }
"continue@"{identifier}							{ unsupportedOperator(); }
"break@"{identifier}							{ unsupportedOperator(); }
"this@"{identifier}								{ unsupportedOperator(); }
"super@"{identifier}							{ unsupportedOperator(); }

"$"												{ unsupportedOperator(); }
\\				    							{ unsupportedOperator(); }

"..." 											{ return leaf(RESERVED); }
"->"											{ return leaf(ARROW); }
"=>"											{ return leaf(DOUBLE_ARROW); }
"::"											{ return leaf(COLONCOLON); }
";;"											{ return leaf(DOUBLE_SEMICOLON); }
"'"												{ return leaf(SINGLE_QUOTE); }

"break"											{ return leaf(BREAK); }
"continue"										{ return leaf(CONTINUE); }
"do"											{ return leaf(DO); }
"else"											{ return leaf(ELSE); }
"false"                                         { return leaf(FALSE); }
"for"											{ return leaf(FOR); }
"fun"											{ return leaf(FUN); }
"if"											{ return leaf(IF); }
"in"											{ return leaf(IN); }
"null"                                          { return leaf(NULL_K); }
"return"										{ return leaf(RETURN); }
"true"                                          { return leaf(TRUE); }
"val"											{ return leaf(VAL); }
"var"											{ return leaf(VAR); }
"when"											{ return leaf(WHEN); }
"while"											{ return leaf(WHILE);}

"import"										{ return leaf(IMPORT); }

"const"											{ return leaf(CONST); }

"as"											{ unsupportedKeyword(); }
"as?"											{ unsupportedKeyword(); }
"class"											{ unsupportedKeyword(); }
"!in"											{ unsupportedKeyword(); }
"is"											{ unsupportedKeyword(); }
"!is"											{ unsupportedKeyword(); }
"object"										{ unsupportedKeyword(); }
"package"										{ unsupportedKeyword(); }
"super"											{ unsupportedKeyword(); }
"this"											{ unsupportedKeyword(); }
"throw"											{ unsupportedKeyword(); }
"try"											{ unsupportedKeyword(); }
"typealias"										{ unsupportedKeyword(); }
"typeof"										{ unsupportedKeyword(); }

"by"											{ unsupportedKeyword(); }
"catch"											{ unsupportedKeyword(); }
"constructor"									{ unsupportedKeyword(); }
"delegate"										{ unsupportedKeyword(); }
"dynamic"										{ unsupportedKeyword(); }
"field"											{ unsupportedKeyword(); }
"file"											{ unsupportedKeyword(); }
"finally"										{ unsupportedKeyword(); }
"get"											{ unsupportedKeyword(); }
"init"											{ unsupportedKeyword(); }
"param"											{ unsupportedKeyword(); }
"property"										{ unsupportedKeyword(); }
"receiver"										{ unsupportedKeyword(); }
"set"											{ unsupportedKeyword(); }
"setparam"										{ unsupportedKeyword(); }
"where"											{ unsupportedKeyword(); }

"abstract"										{ unsupportedKeyword(); }
"actual"										{ unsupportedKeyword(); }
"annotation"									{ unsupportedKeyword(); }
"companion"										{ unsupportedKeyword(); }
"crossinline"									{ unsupportedKeyword(); }
"data"											{ unsupportedKeyword(); }
"enum"											{ unsupportedKeyword(); }
"expect"										{ unsupportedKeyword(); }
"external"										{ unsupportedKeyword(); }
"final"											{ unsupportedKeyword(); }
"infix"											{ unsupportedKeyword(); }
"inline"										{ unsupportedKeyword(); }
"inner"											{ unsupportedKeyword(); }
"internal"										{ unsupportedKeyword(); }
"lateinit"										{ unsupportedKeyword(); }
"noinline"										{ unsupportedKeyword(); }
"open"											{ unsupportedKeyword(); }
"operator"										{ unsupportedKeyword(); }
"out"											{ unsupportedKeyword(); }
"private"										{ unsupportedKeyword(); }
"protected"										{ unsupportedKeyword(); }
"public"										{ unsupportedKeyword(); }
"reified"										{ unsupportedKeyword(); }
"sealed"										{ unsupportedKeyword(); }
"suspend"										{ unsupportedKeyword(); }
"tailrec"										{ unsupportedKeyword(); }
"vararg"										{ unsupportedKeyword(); }

"interface"										{ return leaf(INTERFACE); }
"override"										{ return leaf(OVERRIDE); }

{integer_literal}								{ return intLiteral(INTEGER_LITERAL); }
{long_literal}									{ return longLiteral(INTEGER_LITERAL); }
{hex_literal}									{ return hexLiteral(HEX_LITERAL); }
{float_literal}									{ return floatLiteral(REAL_LITERAL); }
{double_literal}								{ return doubleLiteral(REAL_LITERAL); }
{character_literal}								{ return leaf(CHARACTER_LITERAL); }
{identifier}									{ return leaf(IDENTIFIER); }
{line_string}									{ return stringLiteral(LINE_STRING); }
{multi_line_string}                             { countNewLines(); return multiLineString(MULTILINE_STRING); }

{directive}                                     { /* directives ignored for HW 2 */ }

{nested_comment}								{ fprintf(stderr, "File: %s Line:%d Nested Comments not supported by k0:\t%s\n", filename, rows, yytext); exit(1); }
{bin_literal}									{ fprintf(stderr, "File: %s Line:%d Binary Literals not supported by k0:\t%s\n", filename, rows, yytext); exit(1); }
{unsigned_literal}								{ fprintf(stderr, "File: %s Line:%d Unsigned Integers not supported by k0:\t%s\n", filename, rows, yytext); exit(1);; }
(exponent_literal)                              { fprintf(stderr, "File: %s Line:%d Exponential Notation not supported by k0:\t%s\n", filename, rows, yytext); exit(1);}
{backtick_identifier}							{ fprintf(stderr, "File: %s Line:%d Backtick identifiers not supported by k0:\t%s\n", filename, rows, yytext); exit(1); }
{uni_character_literal}							{ fprintf(stderr, "File: %s Line:%d Unicode characters not supported by k0\n", filename, rows); exit(1); }
{character_error}								{ fprintf(stderr, "File: %s Line:%d Character Literals cannot have more than one character:\t%s\n", filename, rows, yytext); exit(1); }
\"[^\"\n]*                                      { fprintf(stderr, "File: %s Line:%d Unclosed string literal\n", filename, rows); exit(1);}
.												{ fprintf(stderr, "File: %s Line:%d Unknown error\n", filename, rows); exit(1); }


<<EOF>>     									{ return 0; }

%%