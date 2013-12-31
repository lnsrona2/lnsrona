%require "3.0"
%skeleton "lalr1.cc"
%defines 
%define api.namespace {C1}
%define parser_class_name {BisonParser}
%parse-param { C1::FlexScanner &scanner }
%lex-param   { C1::FlexScanner &scanner }

%code requires {
	// Forward-declare the Scanner class; the Parser needs to be assigned a 
	// Scanner, but the Scanner can't be declared without the Parser
	namespace C1 {
		class FlexScanner;
	}
	#include <stdio.h>
	#include <math.h>
	#include "common.h"
	static ASTree ast = NULL;
}

%code {
	// Prototype for the yylex function
	static int yylex(C1::BisonParser::semantic_type * yylval,C1::BisonParser::location_type * yylloc, C1::FlexScanner &scanner);
	static int debug(const char* message)
	{
		return printf(message);
	}
}

%union{
	int		ival;
	Expr*	expr;
	Stmt*	stmt;
	Type*	type;
	Decl*	decl;
}

%locations
%token EOF
%token WHILE IF ELSE FOR DO SWITCH GOTO
%token BREAK RETURN CONTINUE CASE DEFAULT
%token CONST VOLATILE RESTRICT STATIC EXTERN REGISTER
%token INT VOID FLOAT DOUBLE SIGNED UNSIGNED LONG CHAR SHORT
%token STRUCT UNION ENUM
%token TYPEDEF
%token INTLITERAL FLOATLITERAL STRINGLITERAL
%token IDENTIFIER
%token COMMA SEMICOLON COLON QUESTION

%token ASGN MULT_ASGN PLUS_ASGN DIV_ASGN MINUS_ASGN AND_ASGN OR_ASGN LSH_ASGN RSH_ASGN MOD_ASGN XOR_ASGN
%token PLUS MINUS MULT DIV MOD
%token EQL NEQ LSS GTR LEQ GEQ
%token LOGIC_NOT LOGIC_AND LOGIC_OR
%token LSH RSH NOT AND OR
%token DOT ARROW
%token SIZEOF
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE

%left NOELSE
%left ELSE
%right ASGN MULT_ASGN PLUS_ASGN DIV_ASGN MINUS_ASGN AND_ASGN OR_ASGN LSH_ASGN RSH_ASGN MOD_ASGN XOR_ASGN
%left OR
%left AND
%left BITOR
%left BITAND
%left EQL NEQ
%left GEQ GTR LSS LEQ
%left LSH RSH
%left PLUS MINUS
%left MULT DIV MOD
%left MEMBER
%right NEG NOT ADDRESS DEREF BITNOT SIZEOF

%type <expr> INTLITERAL FLOATLITERAL STRINGLITERAL
%type <expr> Expr
%type <type> Type
%type <ival> AssignmentOperator PLUS MINUS MULT DIV MOD ASGN LT GT EQ LE NE BE
%type <name> ident

%type <node> Program FunctionDecl FunctionDefine MainDefine
%type <node> Decl Decls Vdecl VarList Cdecl ConstList
%type <node> CompStat Statf Stat 
%type <node> Relation Exp InitExpr

%%
TypeSpecifier
	: VOID
	| INT
	| FLOAT
	| StructSpecifier
	| UnionSpecifier
	| EnumSpecifier
	| TypedefName
	;

TypeQualifier
	: CONST
	| VOLATILE 
	| RESTRICT
	;

StorageClassSpecifier
	: EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

StructSpecifier
	: STRUCT IDENTIFIER LBRACE StructDeclList RBRACE
	| STRUCT LBRACE StructDeclList RBRACE
	| STRUCT IDENTIFIER
	;

UnionSpecifier
	: UNION IDENTIFIER LBRACE StructDeclList RBRACE
	| UNION LBRACE StructDeclList RBRACE
	| UNION IDENTIFIER
	;

EnumSpecifier
	: ENUM IDENTIFIER LBRACE EnumeratorList RBRACE
	| ENUM IDENTIFIER LBRACE EnumeratorList COMMA RBRACE
	| ENUM LBRACE EnumeratorList RBRACE
	| ENUM LBRACE EnumeratorList COMMA RBRACE
	| ENUM IDENTIFIER
	;

StructDeclList
	: StructDeclaration
	| StructDeclList StructDeclaration
	;

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
Vdecl		:int VarList ';'	
		{
			debug("Vdecl ::= int ident Vdelf ;\n");
			$$ = $2;
			setLoc($$,(Loc)&(@$));
		}
		;

VarList		:VarList ',' ident InitExpr
		{
			debug("Vdelf ::= Vdelf , ident \n");
			addLast($1->vardeclstmt->vars, newVariable(ast->Tab,$3,$4));
			$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		| ident InitExpr
		{
			debug("Vdelf ::= \n");
			$$ = newVarDeclStmt(INT);
			addLast($$->vardeclstmt->vars, newVariable(ast->Tab,$1,$2));
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

Cdecl		:const int ConstList ';'
		{
			debug("Cdecl ::= const int Assn Cdelf ;\n");
			$$ = $3;
			setLoc($$,(Loc)&(@$));
		}
		;

ConstList		:ConstList ',' ident InitExpr
		{
			debug("Vdelf ::= Vdelf , ident \n");
			addLast($1->vardeclstmt->vars, newConstant(ast->Tab,$3,$4));
			$$ = $1;
			setLoc($$,(Loc)&(@$));
		}
		| ident InitExpr
		{
			debug("Vdelf ::= \n");
			$$ = newConstDeclStmt(CONST_INT);
			addLast($$->vardeclstmt->vars, newConstant(ast->Tab,$1,$2));
			setLoc($$,(Loc)&(@$));
		}
		;

FunctionDecl:void ident '(' ')'
		{
			debug("FunctionDecl ::= void ident ()\n");
			$$ = newFunction(ast->Tab, $2, NULL);
		}
		;

FunctionDefine	:FunctionDecl CompStat
		{
			debug("FunctionDef ::= void ident ()  CompStat \n");
			$$->function->body = $2;
			setLoc($$,(Loc)&(@$));
		} 
		;

MainDefine		:void main '(' ')'  CompStat
		{
			debug("MainDefine ::= void main () CompStat \n");
			$$ = newFunction(ast->Tab,"main",$5);
			setLoc($$,(Loc)&(@$));
		}
		;

CompStat	:'{'Statf'}'
		{
			debug("CompStat ::= { Statf } \n");
			$$ = $2; 
			popTable(ast->Tab);
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
			pushTable(ast->Tab);
			$$ = newComposeStmt();
		}
		;
Stat		:ident ASGN Exp ';' 
		{
			debug("stat ::= ident ASGN Exp ; \n");
			$$=newAssignExpr($2,newVarExpr(ast->Tab, $1), $3);
			setLoc($$,(Loc)&(@$));
		}
		|if '(' Relation ')' Stat 
		{
			debug("stat ::= if ( Relation ) Stat  \n");
			$$=newIfStmt($3,$5);
			setLoc($$,(Loc)&(@$));
		}
		|while '(' Relation ')'  Stat
		{
			debug("stat ::= while ( Relation )  Stat \n");
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
			$$ = newCallExpr(ast->Tab,$1);
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
			$$ = newVarExpr(ast->Tab, $1); 
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

// We have to implement the error function
void C1::BisonParser::error(const C1::BisonParser::location_type &loc, const std::string &msg) {
	std::cerr << "Error: " << msg << std::endl;
}

// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function
#include "cscanner.h"
static int yylex(C1::BisonParser::semantic_type * yylval,C1::BisonParser::location_type * yylloc, C1::FlexScanner &scanner) {
	return scanner.yylex(yylval,yylloc);
}

/*
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
} */