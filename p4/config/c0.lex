%{
	#include "c0.tab.h"
	#include <stdio.h>
%}

octal		0[0-7]+	
digit		[0-9]+
hex		0[Xx][0-9A-Fa-f]+
ident		[A-Za-z_][A-Za-z_0-9]*	
comment		"/*"([^*]|(\*+[^*/]))*\**"*/"
%%

"while"		{ECHO;return(whilesym);}
"if"		{ECHO;return(ifsym);}
"const"		{ECHO;return(constsym);}
"int"    	{ECHO;return(intsym);}
"void"		{ECHO;return(voidsym);}
"main"		{ECHO;return(mainsym);}


{octal}	     	{ECHO;return(number);}
{digit}    	{ECHO;return(number);}
{hex}	     	{ECHO;return(number);}
{ident} 	{ECHO;return(ident);}
{comment}    	{ECHO;}
[\n]	   	{ECHO;}
[\t ]*  	{ECHO;}
   	
"=="		{ECHO;return(eql);}
"<="		{ECHO;return(leq);}
">="		{ECHO;return(geq);}
"!="		{ECHO;return(neq);}

"="		{ECHO;return('=');}
"<"		{ECHO;return('<');}
">"		{ECHO;return('>');}
								
"+"        	{ECHO;return(plus);}
"-"		{ECHO;return(minus);}
"*"		{ECHO;return(times);}
"/"		{ECHO;return(divide);}

"{"		{ECHO;return('{');}
"}"		{ECHO;return('}');}
"("		{ECHO;return('(');}
")"		{ECHO;return(')');}
","		{ECHO;return(',');}
";"		{ECHO;return(';');}
	

%%



