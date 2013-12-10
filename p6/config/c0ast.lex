/*
 * expr.lex : Scanner for a simple
 *            expression parser.
 */

%{
#include "c0ast.tab.h"
#include "common.h"
#include "string.h"
#include "stdlib.h"
/* handle locations */
int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;
%}

%option noyywrap
%option yylineno

octal		0[0-7]+	
digit		[0-9]+
hex		0[Xx][0-9A-Fa-f]+
ident		[A-Za-z_][A-Za-z_0-9]*	
comment		"/*"([^*]|(\*+[^*/]))*\**"*/"
%%

"while"		{
		  return(whilesym);
		}
"const"		{ return(constsym);}
"if"		{ return(ifsym);}
"int"    	{ return(intsym);}
"void"		{ return(voidsym);}
"main"		{ return(mainsym);}

{octal}	     	{ 
			yylval.ival = atol(yytext);
			return(number);
		}
{digit}    	{ 
			yylval.ival = atol(yytext);
			return(number);
		}
{hex}	     	{ 
			yylval.ival = atol(yytext);
			return(number);
		}
{ident} 	{ 
			yylval.name = (char*)malloc(yyleng+1);
			yylval.name[yyleng] = '\0';
			strncpy(yylval.name,yytext,yyleng);
			return(ident);
		}
{comment}    	{ }
[\n]	   	{ 
			yycolumn = 1;
		}
[\t ]*     	{ 
		}

"="		{ 
			yylval.ival = OP_ASGN;
			return(ASGN);
		}
"<"		{ 
			yylval.ival = OP_LT;
			return(LT);
		}
"=="		{
			yylval.ival = OP_EQ;
			 return(EQ);
		}
"<="		{
			yylval.ival = OP_LE;
			 return(LE);
		}
"!="		{
			yylval.ival = OP_NE;
			 return(NE);
		}
">"		{
			yylval.ival = OP_GT;
			 return(GT);
		}
">="		{
			yylval.ival = OP_BE;
			 return(BE);
		}						
"+"        	{
			yylval.ival = OP_PLUS;
			 return(PLUS);
		}
"-"		{
			yylval.ival = OP_MINUS;
			return(MINUS);
		}
"*"		{
			yylval.ival = OP_MULT;
			 return(MULT);
		}
"/"		{
			yylval.ival = OP_DIV;
			 return(DIV);
		}

"{"		{ return('{');}
"}"		{ return('}');}
"("		{ return('(');}
")"		{ return(')');}
","		{ return(',');}
"."		{ return('.');}
";"		{ return(';');}
	

%%
