%{
#include <stdio.h>
#include <math.h>
#include "common.h"
//extern SymbolTable ast->symTab;
static ASTree ast = NULL;

extern int yylex();
int yyerror(char *message);

%}
%union{
	int ival;
	char *name;
	struct astnode *node;
}
%locations

%token number ident
%token whilesym ifsym
%token constsym intsym
%token voidsym mainsym

//%token EQ LE NE BE
//%token ASGN LT GT
%token PLUS MINUS MULT DIV MOD ASGN LT GT EQ LE NE BE
%token '{' '}' '(' ')' ',' '.' ';'

%left PLUS MINUS
%left MULT DIV MOD
//%right UMINUS

%type <ival> number
%type <ival> PLUS MINUS MULT DIV MOD ASGN LT GT EQ LE NE BE
%type <name> ident
%type <node> Program FunctionDecl FunctionDefine MainDefine
%type <node> Decl Decls Vdecl VarList Cdecl ConstList
%type <node> CompStat Statf Stat 
%type <node> Relation Exp InitExpr

%%
Program		:Decls MainDefine
		{
			debug("Program ::= Decls MainDefine \n");
			$$ = $1;
			$$->program->main = $2;
			setLoc($$,(Loc)&(@$));
			ast->root = $$;               
		}
		;
Decls		:
		{
			debug("Decls ::= \n");
			$$ = newProgram();
			//$$->kind = KDecls;
		}
		|Decls Decl
		{
			debug("Decls ::= Decls Decl \n");
			addLast($1->program->decls, $2);
			$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		;
Decl		:Vdecl
		{
			debug("Decl ::= Vdecl \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		| Cdecl
		{
			debug("Decl ::= Cdecl \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		| FunctionDefine
		{
			debug("Decl ::= FunctionDef \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		;
Vdecl		:intsym VarList ';'	
		{
			debug("Vdecl ::= intsym ident Vdelf ;\n");
			$$ = $2;
			setLoc($$,(Loc)&(@$));
		}
		;

VarList		:VarList ',' ident InitExpr
		{
			debug("Vdelf ::= Vdelf , ident \n");
			addLast($1->vardeclstmt->vars, newVariable(ast->symTab,$3,$4));
			$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		| ident InitExpr
		{
			debug("Vdelf ::= \n");
			$$ = newVarDeclStmt(INT);
			addLast($$->vardeclstmt->vars, newVariable(ast->symTab,$1,$2));
			setLoc($$,(Loc)&(@$));
		}
		;

InitExpr	: ASGN number
		{
			$$ = newNumber($2);
			setLoc($$, (Loc)&(@$));
		}
		| 
		{
			$$ = NULL;
		}
		;

Cdecl		:constsym intsym ConstList ';'
		{
			debug("Cdecl ::= constsym intsym Assn Cdelf ;\n");
			$$ = $3;
			setLoc($$,(Loc)&(@$));
		}
		;

ConstList		:ConstList ',' ident InitExpr
		{
			debug("Vdelf ::= Vdelf , ident \n");
			addLast($1->vardeclstmt->vars, newConstant(ast->symTab,$3,$4));
			$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		| ident InitExpr
		{
			debug("Vdelf ::= \n");
			$$ = newConstDeclStmt(CONST_INT);
			addLast($$->vardeclstmt->vars, newConstant(ast->symTab,$1,$2));
			setLoc($$,(Loc)&(@$));
		}
		;

FunctionDecl:voidsym ident '(' ')'
		{
			debug("FunctionDecl ::= voidsym ident ()\n");
			$$ = newFunction(ast->symTab, $2, NULL);
		}
		;

FunctionDefine	:FunctionDecl CompStat
		{
			debug("FunctionDef ::= voidsym ident ()  CompStat \n");
			$$->function->body = $2;
			setLoc($$,(Loc)&(@$));
		} 
		;

MainDefine		:voidsym mainsym '(' ')'  CompStat
		{
			debug("MainDefine ::= voidsym mainsym () CompStat \n");
			$$ = newFunction(ast->symTab,"main",$5);
			setLoc($$,(Loc)&(@$));
		}
		;

CompStat	:'{'Statf'}'
		{
			debug("CompStat ::= { Statf } \n");
			$$ = $2; 
			popTable(ast->symTab);
			setLoc($$,(Loc)&(@$));
		}
		;
Statf		:Statf Stat
		{
			debug("Statf ::= Statf Stat \n");
			addLast($1->compstmt->stmts,$2);
			$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		|
		{
			debug("Statf ::= \n");
			pushTable(ast->symTab);
			$$ = newComposeStmt();
		}
		;
Stat		:ident ASGN Exp ';' 
		{
			debug("stat ::= ident ASGN Exp ; \n");
			$$=newAssignExpr($2,newVarExpr(ast->symTab, $1), $3);
			setLoc($$,(Loc)&(@$));
		}
		|ifsym '(' Relation ')' Stat 
		{
			debug("stat ::= ifsym ( Relation ) Stat  \n");
			$$=newIfStmt($3,$5);
			setLoc($$,(Loc)&(@$));
		}
		|whilesym '(' Relation ')'  Stat
		{
			debug("stat ::= whilesym ( Relation )  Stat \n");
			$$ = newWhileStmt($3,$5);
			setLoc($$,(Loc)&(@$));
		}
		|Vdecl
		{
			debug("stat ::= Vdecl \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		|Cdecl
		{
			debug("stat ::= Cdecl \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		|ident '(' ')' ';'
		{
			debug("stat ::= ident ( ) ; \n");
			$$ = newCallExpr(ast->symTab,$1);
			setLoc($$,(Loc)&(@$));
		}
		|CompStat
		{
			debug("stat ::= CompStat \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		;
Relation	:Exp GT Exp
		{
			debug("Relation ::= Exp GT Exp \n");
			$$=newRelationExpr($2,$1,$3);
			setLoc($$,(Loc)&(@$));
		}
		|Exp LT Exp
		{
			debug("Relation ::= Exp LT Exp \n");
			$$=newRelationExpr($2,$1,$3);
			setLoc($$,(Loc)&(@$));
		}
		|Exp EQ Exp
		{
			debug("Relation ::= Exp EQ Exp \n");
			$$=newRelationExpr($2,$1,$3);
			setLoc($$,(Loc)&(@$));
		}
		|Exp NE Exp
		{
			debug("Relation ::= Exp NQ Exp \n");
			$$=newRelationExpr($2,$1,$3);
			setLoc($$,(Loc)&(@$));
		}
		|Exp LE Exp
		{
			debug("Relation ::= Exp LE Exp \n");
			$$=newRelationExpr($2,$1,$3);
			setLoc($$,(Loc)&(@$));
		}
		|Exp BE Exp
		{
			debug("Relation ::= Exp BE Exp \n");
			$$=newRelationExpr($2,$1,$3);
			setLoc($$,(Loc)&(@$));
		}
		;
Exp     	: number
		{
			debug("Exp ::= number\n");
			$$ = newNumber($1);
			setLoc($$, (Loc)&(@$));
		}
		| ident
		{
			debug("Exp ::= ident\n");
			$$ = newVarExpr(ast->symTab, $1); 
			setLoc($$, (Loc)&(@$));
		}
		| Exp PLUS Exp
		{
			debug("Exp ::= Exp PLUS Exp\n");
			$$ = newInfixExpr($2, $1, $3); 
			setLoc($$, (Loc)&(@$));
		}
		| Exp MINUS Exp
		{
			debug("Exp ::= Exp MINUS Exp\n");
			$$ = newInfixExpr($2, $1, $3); 
			setLoc($$, (Loc)&(@$));
		}
		| Exp MULT Exp
		{
			debug("Exp ::= Exp MULT Exp\n");
			$$ = newInfixExpr($2, $1, $3); 
			setLoc($$, (Loc)&(@$));
		}
		| Exp DIV Exp
		{
			debug("Exp ::= Exp DIV Exp\n");
			$$ = newInfixExpr($2, $1, $3); 
			setLoc($$, (Loc)&(@$));
		}
		| Exp MOD Exp
		{
			debug("Exp ::= Exp MOD Exp\n");
			$$ = newInfixExpr($2, $1, $3); 
			setLoc($$, (Loc)&(@$));
		}
		| '(' Exp ')'
		{
			debug("Exp ::= ( Exp )\n");
			$$ = newParenExpr($2);
			setLoc($$, (Loc)&(@$));
		}

%%

int yyerror(char *message)
{
	printf("%s\n",message);
	return 0;
}

ASTree parse()
{
	ASTree tree = newAST();
	ast = tree;
	yyparse();
	return tree;
}