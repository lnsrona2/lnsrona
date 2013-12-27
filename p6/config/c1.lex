/*
 * expr.lex : Scanner for a simple
 *            expression parser.
 */

%{
#include "cscanner.h"
%}

%option nodefault yyclass="FlexScanner" noyywrap c++ yylineno

octal		0[0-7]+	
digit		[0-9]+
hex		0[Xx][0-9A-Fa-f]+
ident		[A-Za-z_][A-Za-z_0-9]*	
comment		"/*"([^*]|(\*+[^*/]))*\**"*/"
linecomment	"//"[^\n]+

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
			yylval.ival = strtol(yytext,NULL,8);//atol(yytext);
			return(number);
		}
{digit}    	{ 
			yylval.ival = strtol(yytext,NULL,10);//atol(yytext);
			return(number);
		}
{hex}	     	{ 
			yylval.ival = strtol(yytext,NULL,16);//atol(yytext);
			return(number);
		}
{ident} 	{ 
			yylval.name = (char*)malloc(yyleng+1);
			yylval.name[yyleng] = '\0';
			strncpy(yylval.name,yytext,yyleng);
			return(ident);
		}
{comment}    	{ }
{linecomment}    	{ }
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
"%"		{
			yylval.ival = OP_MOD;
			return(MOD);
		}

"{"		{ return('{');}
"}"		{ return('}');}
"("		{ return('(');}
")"		{ return(')');}
","		{ return(',');}
"."		{ return('.');}
";"		{ return(';');}
	

%%
