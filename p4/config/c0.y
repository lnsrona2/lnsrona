/*
 * asgn.y : A simple yacc assignment statement parser
 *          Based on the Bison manual example. 
 */

%{
	#include <stdio.h>
	#include <math.h>
%}

%token number ident
%token whilesym ifsym
%token constsym intsym
%token voidsym mainsym

%token eql leq neq geq
%token '=' '<' '>'
%token plus minus times divide
%token '{' '}' '(' ')' ','  ';'

%left '+' '-'
%left '*' '/'


%%
Program		:Block MainDef
		;
Block		:Block Decls
		|
		;
Decls		:Vdecl
		|Cdecl
		|FunctionDef
		;
Vdecl		:intsym ident Vdelf ';'		
		;
Vdelf		:Vdelf ',' ident
		|
		;
Cdecl		:constsym intsym ident '=' number Cdelf ';'
		;
Cdelf		:Cdelf ',' ident '=' number
		|
		;
FunctionDef	:voidsym ident '(' ')'  CompStat 
		;
MainDef		:voidsym mainsym '(' ')'  CompStat
		;
CompStat	:'{'Statf'}'
		;
Statf		:Statf Stat
		|
		;
Stat		:Assign 
		|Condition
		|Wlop
		|Vdecl
		|Cdecl
		|Functioncall
		|CompStat
		;
Functioncall	:ident '(' ')' ';'
		;
Wlop		:whilesym '(' Relation ')'  Stat
		;
Condition	:ifsym '(' Relation ')' Stat  
		;
Assign		:ident '=' Expr ';'
		;
Relation	:Expr Relopr Expr
		;
Relopr		:'>'
		|'<'
		|eql
		|neq
		|geq
		|leq
		;
Expr		:Expr Lopr Term
		|Term
		;
Term		:Term Hopr Factor
		|Factor
		;
Factor		:ident
		|number
		|'(' Expr ')'
		;
Lopr		:plus 
		|minus
		;
Hopr		:times
		|divide
		;
%%


yyerror(char *message)
{
	printf("%s\n",message);
}

int main(int argc, char *argv[])
{
	yyparse();
	return(0);
}
	

	
