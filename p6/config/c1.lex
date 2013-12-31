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
char		\'([^\'\n])+\'
bad_char	\'([^\'\n])+
string		\"([^\"\n])*\"
bad_string	\"([^\"\n])+\"
comment		"/*"([^*]|(\*+[^*/]))*\**"*/"
linecomment	"//"[^\n]+

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
{linecomment}    	{
			yylloc->lines (yyleng);
			yylloc->step (); 
		}
[\n]	   	{ 
			yylloc->lines (yyleng);
			yylloc->step (); 
		}
[\t ]*     	{ 
			yylloc->step (); 
		}
"while"		{
		  return(token::WHILE);
		}
"const"		{ return(token::CONST);}
"if"		{ return(token::IF);}
"int"    	{ return(token::INT);}
"void"		{ return(token::VOID);}
"main"		{ return(token::MAIN);}

{octal}	     	{ 
			yylval->ival = strtol(yytext,NULL,8);//atol(yytext);
			return(token::NUMBER);
		}
{digit}    	{ 
			yylval->ival = strtol(yytext,NULL,10);//atol(yytext);
			return(token::NUMBER);
		}
{hex}	     	{ 
			yylval->ival = strtol(yytext,NULL,16);//atol(yytext);
			return(token::NUMBER);
		}
{ident} 	{ 
			yylval->name = (char*)malloc(yyleng+1);
			yylval->name[yyleng] = '\0';
			strncpy(yylval->name,yytext,yyleng);
			return(token::IDENTIFIER);
		}
"="		{ 
			yylval->ival = OP_ASGN;
			return(token::ASGN);
		}
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
