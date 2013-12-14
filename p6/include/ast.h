#ifndef _AST_H
#define _AST_H

#include "util.h"
#include "symtab.h"

typedef struct location {
	int first_line;
	int first_column;
	int last_line;
	int last_column;
} *Loc;

typedef struct {
	int 	op;
	//int type;
	int 	val;
	struct astnode	*kids[2];// kids of the AST node
} *Exp;


typedef struct {
	List  stmts;
} *Block;

typedef struct{
	int relop;
	bool truth;
	struct astnode *kid[2];
} *Relation;

typedef struct{
	char *name;
} *Functioncall;

typedef struct{
	struct astnode *relation;
	struct astnode *stat;
} *WhileLoop;

typedef struct{
	struct astnode *condition;
	struct astnode *thenAction;
	struct astnode *elseAction;
} *Conditional;

//typedef struct{
//	struct astnode *compstat;
//} *MainDef;

typedef struct{
	char *name;
	struct astnode *compstat;
} *FunctionDef;

typedef struct{
	char *name;
	int num;
} *Assn;

typedef struct{
	int type;
	List vars;
} *VarDeclList;

typedef struct{
	struct astnode *block;
	struct astnode *maindef;
} *Program;

//------------------------------------------------------------------

typedef struct astnode{
	enum {
		KValue = 0x200,		// numerial value:
		KName,			// name, such as variable name
		KInfixExp,		// infix expression
		KAssignExp,		// assignment expression
		KParenExp,		// parentheses expression
		KProgram,
		KDecls,
		KBlock,			// block
		KVdecl,
		KCdecl,
		KAssn,
		KCdelf,
		KFunctionDef,
		KMainDef,
		KCompStat,
		//KStatf,
		KConditional,	//If statement
		KWlop,
		KFunctioncall,
		KRelation,
	} kind;	// kind of the AST node
	union {		// information of various kinds of AST node 
		int  val;		// KValue: numerial value
		Symbol sym;		// KName: symbols 
		Exp   exp;		// KPrefixExp,
		// KInfixExp,
		// KAssignExp,
		// KParenExp
		Program	program;
		//Block block;			// block
		VarDeclList varlist;
		FunctionDef functiondef;
		Block compstat;
		Block decls;
		WhileLoop loop;
		Conditional conditional;
		Functioncall functioncall;
		Relation relation;
	};
	Loc 	loc;			// locations
} *ASTNode;

typedef struct ASTtree {
	ASTNode root;
	SymbolTable symTab;
} *ASTTree;

// functions for creating various kinds of ASTnodes
// Symbols and declarations
ASTNode newVariable(const char* name, ASTNode initExpr);
ASTNode newConstant(const char* name, ASTNode initExpr);
ASTNode newFunction(const char* name, ASTNode body);
void destroyVariable();
void destroyFunction();

ASTNode newVarDecl(int type);
ASTNode newConstDecl(int type);
void	destroyVarDecl(Vdecl *pnode);
void	destroyConstDecl(Cdecl *pnode);

// Expressions
ASTNode newNumber(int value);
ASTNode newPrefixExp(int op, ASTNode exp);
ASTNode newParenExp(ASTNode exp);
ASTNode newInfixExp(int op, ASTNode left, ASTNode right);
ASTNode newAssignment(int op, ASTNode left, ASTNode right);
void	destroyExp(Exp *pexp);

ASTNode newProgram(ASTNode block, ASTNode maindef);
void	destroyProgram(Program *prog);

ASTNode newBlock();
void	destroyBlock(Block *pblock);



//Statments
ASTNode newCompStat();
ASTNode newIf(ASTNode relation, ASTNode stat);
ASTNode newWlop(ASTNode relation, ASTNode stat);
void	destroyLoop(WhileLoop *loop);
ASTNode newFunctioncall(char* name);
ASTNode newRelation(int relop, ASTNode lkid, ASTNode rkid);
void	destroyRelation(Relation *prelation);

ASTTree newAST();
void	destroyAST(ASTNode *pnode);
void 	dumpAST(ASTNode node);
Loc		setLoc(ASTNode node, Loc loc);

#endif // !_AST_H
