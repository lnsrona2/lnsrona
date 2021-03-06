%{
#include <stdio.h>
#include <math.h>
#include <common.h>
Table symtab;
ASTTree ast;
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
%token PLUS MINUS MULT DIV ASGN LT GT EQ LE NE BE
%token '{' '}' '(' ')' ',' '.' ';'

%left PLUS MINUS
%left MULT DIV
//%right UMINUS

%type <ival> number
%type <ival> PLUS MINUS MULT DIV ASGN LT GT EQ LE NE BE
%type <name> ident
%type <node> goal Program Block Decls Vdecl Vdelf 
%type <node> Cdecl Assn Cdelf FunctionDef MainDef CompStat Statf Stat 
%type <node> Relation Exp

%%
goal		:Program
		{
			debug("goal ::= Program \n");
	    		ast->root = $$;
	  	}
		;
Program		:Block MainDef
		{
			debug("Program ::= Block MainDef \n");
			$$ = newProgram($1,$2);
			setLoc($$,(Loc)&(@$));
		}
		;
Block		:
		{
			debug("Block ::= \n");
			$$ = newBlock();
		}
		|Block Decls
		{
			debug("Block ::= Block Decls \n");
			addLast($1->block->stmts, $2);
	    		$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		;
Decls		:Vdecl
		{
			debug("Decls ::= Vdecl \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		|Cdecl
		{
			debug("Decls ::= Cdecl \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		|FunctionDef
		{
			debug("Decls ::= FunctionDef \n");
			$$ = $1;
			//setLoc($$,(Loc)&(@$));
		}
		;
Vdecl		:intsym ident Vdelf ';'	
		{
			debug("Vdecl ::= intsym ident Vdelf ;\n");
			$$ = newVdecl(newName(symtab,$2),$3);
			setLoc($$,(Loc)&(@$));
		}
		;
Vdelf		:Vdelf ',' ident
		{
			debug("Vdelf ::= Vdelf , ident \n");
			addLast($1->block->stmts, newName(symtab,$3));
	    		$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		|
		{
			debug("Vdelf ::= \n");
			$$ = newVdelf();
		}
		;
Cdecl		:constsym intsym Assn Cdelf ';'
		{
			debug("Cdecl ::= constsym intsym Assn Cdelf ;\n");
			$$ = newCdecl($3,$4);
			setLoc($$,(Loc)&(@$));
		}
		;
Assn		: ident ASGN number
		{
			debug("constdef ::= ident ASGN number \n");
			$$ = newAssn(newName(symtab,$1),$3);
			setLoc($$,(Loc)&(@$));
		}
		;
Cdelf		:Cdelf ',' Assn
		{
			debug("Cdelf ::= Cdelf , Assn \n");
		  	addLast($1->block->stmts,$3);
		  	$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		|
		{
			debug("Cdelf ::= \n");
			$$ = newCdelf();
		}
		;
FunctionDef	:voidsym ident '(' ')'  CompStat
		{
			debug("FunctionDef ::= voidsym ident ()  CompStat \n");
			$$ = newFunctionDef(newName(symtab,$2),$5);
			setLoc($$,(Loc)&(@$));
		} 
		;
MainDef		:voidsym mainsym '(' ')'  CompStat
		{
			debug("MainDef ::= voidsym mainsym () CompStat \n");
			$$ = newMainDef($5);
			setLoc($$,(Loc)&(@$));
		}
		;
CompStat	:'{'Statf'}'
		{
			debug("CompStat ::= { Statf } \n");
			$$ = newCompStat($2);
			setLoc($$,(Loc)&(@$));
		}
		;
Statf		:Statf Stat
		{
			debug("Statf ::= Statf Stat \n");
			addLast($1->block->stmts,$2);
			$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		|
		{
			debug("Statf ::= \n");
			$$ = newStatf();
		}
		;
Stat		:ident ASGN Exp ';' 
		{
			debug("stat ::= ident ASGN Exp ; \n");
			$$=newAssignment($2,newName(symtab, $1), $3);
			setLoc($$,(Loc)&(@$));
		}
		|ifsym '(' Relation ')' Stat 
		{
			debug("stat ::= ifsym ( Relation ) Stat  \n");
			$$=newStatif($3,$5);
			setLoc($$,(Loc)&(@$));
		}
		|whilesym '(' Relation ')'  Stat
		{
			debug("stat ::= whilesym ( Relation )  Stat \n");
			$$ = newWlop($3,$5);
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
			$$ = newFunctioncall(newName(symtab,$1));
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
			$$=newRelation($2,$1,$3);
		  	setLoc($$,(Loc)&(@$));
		}
		|Exp LT Exp
		{
			debug("Relation ::= Exp LT Exp \n");
			$$=newRelation($2,$1,$3);
		  	setLoc($$,(Loc)&(@$));
		}
		|Exp EQ Exp
		{
			debug("Relation ::= Exp EQ Exp \n");
			$$=newRelation($2,$1,$3);
		  	setLoc($$,(Loc)&(@$));
		}
		|Exp NE Exp
		{
			debug("Relation ::= Exp NQ Exp \n");
			$$=newRelation($2,$1,$3);
		  	setLoc($$,(Loc)&(@$));
		}
		|Exp LE Exp
		{
			debug("Relation ::= Exp LE Exp \n");
			$$=newRelation($2,$1,$3);
		  	setLoc($$,(Loc)&(@$));
		}
		|Exp BE Exp
		{
			debug("Relation ::= Exp BE Exp \n");
			$$=newRelation($2,$1,$3);
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
			    $$ = newName(symtab, $1); 
			    setLoc($$, (Loc)&(@$));
		  }
		| Exp PLUS Exp
		  {
			    debug("Exp ::= Exp PLUS Exp\n");
			    $$ = newInfixExp($2, $1, $3); 
			    setLoc($$, (Loc)&(@$));
		  }
		| Exp MINUS Exp
		  {
			    debug("Exp ::= Exp MINUS Exp\n");
			    $$ = newInfixExp($2, $1, $3); 
			    setLoc($$, (Loc)&(@$));
		  }
		| Exp MULT Exp
		  {
			    debug("Exp ::= Exp MULT Exp\n");
			    $$ = newInfixExp($2, $1, $3); 
			    setLoc($$, (Loc)&(@$));
		  }
		| Exp DIV Exp
		  {
			    debug("Exp ::= Exp DIV Exp\n");
			    $$ = newInfixExp($2, $1, $3); 
			    setLoc($$, (Loc)&(@$));
		  }
        	| '(' Exp ')'
	 	{
		    	debug("Exp ::= ( Exp )\n");
		    	$$ = newParenExp($2);
	    		setLoc($$, (Loc)&(@$));
	  	}

%%

yyerror(char *message)
{
	printf("%s\n",message);
}


int main(int argc, char *argv[])
{
	symtab = newTable();
	ast = newAST();
	printf("Parsing ...\n");
	yyparse();
	printf("\n\nDump the program from the generated AST:\n  ");
	dumpAST(ast->root);
	destroyAST(&ast->root);
	printf("\n\nFinished destroying AST.\n");
	destroyTable(&symtab);
	printf("\n\nFinished destroying symbolic table.\n");
	return(0);
}
