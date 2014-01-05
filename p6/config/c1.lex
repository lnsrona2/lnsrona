/*
 * expr.lex : Scanner for a simple
 *            expression parser.
 */

%{
#include "cscanner.h"
//using namespace std;
%}

%option noyywrap c++ yyclass="FlexScanner" yylineno

digit		[0-9]+
octal		0[0-7]+	
hex			0[Xx][0-9A-Fa-f]+
unsigned	{digit}[uU]
decimal		[{digit}]?"."{digit}[fFdD]
sci_float	{decimal}[eE]["+""-"]?{digit}
ident		[A-Za-z_][A-Za-z_0-9]*

%{
#define yyterminate() return token::eof
# define YY_USER_ACTION yylloc->columns (yyleng);
%}

%{
typedef C1::BisonParser::token token;
%} 

%%
%{
	yylloc->step ();
%}
{comment}    	{
					yylloc->lines (yyleng);
					yylloc->step (); 
				}
{linecomment} 	{
					yylloc->lines (yyleng);
					yylloc->step (); 
				}
[\n] 			{ 
					yylloc->lines (yyleng);
					yylloc->step (); 
				}
[\t ]*     		{ 
					yylloc->step (); 
				}

"auto"			{ return(token::AUTO); }
"break"			{ return(token::BREAK); }
"case"			{ return(token::CASE); }
"char"			{ return(token::CHAR); }
"continue"		{ return(token::CONTINUE); }
"default"		{ return(token::DEFAULT); }
"do"			{ return(token::DO); }
"double"		{ return(token::DOUBLE); }
"else"			{ return(token::ELSE); }
"enum"			{ return(token::ENUM); }
"extern"		{ return(token::EXTERN); }
"float"			{ return(token::FLOAT); }
"for"			{ return(token::FOR); }
"goto"			{ return(token::GOTO); }
"if"			{ return(token::IF); }
"int"			{ return(token::INT); }
"long"			{ return(token::LONG); }
"register"		{ return(token::REGISTER); }
"return"		{ return(token::RETURN); }
"short"			{ return(token::SHORT); }
"signed"		{ return(token::SIGNED); }
"sizeof"		{ return(token::SIZEOF); }
"static"		{ return(token::STATIC); }
"struct"		{ return(token::STRUCT); }
"switch"		{ return(token::SWITCH); }
"typedef"		{ return(token::TYPEDEF); }
"union"			{ return(token::UNION); }
"unsigned"		{ return(token::UNSIGNED); }
"void"			{ return(token::VOID); }
"while"			{ return(token::WHILE); }
"restrict"		{ 
					yyval->ival = C1::TypeQualifierEnum::RESTRICT; 
					return(token::RESTRICT);
				}
"const"			{
					yyval->ival = C1::TypeQualifierEnum::Const; 
					return(token::CONST);
				}
"volatile"		{
					yyval->ival = C1::TypeQualifierEnum::VOLATILE; 
					return(token::VOLATILE);
				}

{octal} 		{ 
						yylval->ival = strtol(yytext,NULL,8);//atol(yytext);
						return(token::INT_LITERAL);
				}
{digit}    		{ 
					yylval->ival = strtol(yytext,NULL,10);//atol(yytext);
					return(token::INT_LITERAL);
				}
{hex}		   	{ 
					yylval->ival = strtol(yytext,NULL,16);//atol(yytext);
					return(token::INT_LITERAL);
				}
{ident}	 		{ 
					yylval->identifier = new std::string(yytext);
					auto decl = pContext->CurrentDeclContext()->Lookup(*yylval->identifier);
					if (decl){
						if (is<TypeDeclaration*>(decl))
							return token::TypeIdentifier;
						else
							return token::ObjectIdentifier;
					} else
					return(token::NewIdentifier);
				}
"="		{ 
			yylval->ival = OP_ASGN;
			return(token::ASGN);
		}INT_LITERAL
"<"		{ 
			yylval->ival = OP_LT;
			return(token::LSS);
		}
"=="		{
			yylval->ival = OP_EQ;
			return(token::EQL);
		}
"<="		{
			yylval->ival = OP_LE;
			return(token::LEQ);
		}
"!="		{
			yylval->ival = OP_NE;
			return(token::NEQ);
		}
">"		{
			yylval->ival = OP_GT;
			return(token::GTR);
		}
">="		{
			yylval->ival = OP_BE;
			return(token::GEQ);
		}						
"+"        	{
			yylval->ival = OP_PLUS;
			return(token::PLUS);
		}
"-"		{
			yylval->ival = OP_MINUS;
			return(token::MINUS);
		}
"*"		{
			yylval->ival = OP_MULT;
			return(token::MULT);
		}
"/"		{
			yylval->ival = OP_DIV;
			return(token::DIV);
		}
"%"		{
			yylval->ival = OP_MOD;
			return(token::MOD);
		}

"{"		{ return(token::LBRACE);}
"}"		{ return(token::RBRACE);}
"("		{ return(token::LPAREN);}
")"		{ return(token::RPAREN);}
"["		{ return(token::LBRACKET);}
"]"		{ return(token::RBRACKET);}
","		{ return(token::COMMA);}
"."		{ return(token::DOT);}
";"		{ return(token::SEMICOLON);}

.		{ 
			yylloc->steop();
			printf("Invaliad char.");
		}

%%
