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
	int		type;
	int 	val;
	struct astnode	*oprands[2];// kids of the AST node
} *Expr;

typedef struct{
	Symbol sym; //no ownership, symbol's owner's ship belongs to defination nodes
	List arglist;
} *CallExpr;

typedef struct{
	//SymbolTable localTable;
	List stmts;
} *ComposeStmt;

typedef struct{
	struct astnode *condition;
	struct astnode *thenAction;
	struct astnode *elseAction;
} *IfStmt;

typedef struct{
	char *name;
	int num;
} *AssignStmt;

typedef struct{
	Symbol sym;
	struct astnode *body;
} *Function;

typedef struct{
	Type type;
	List vars;
} *VarDeclStmt;

typedef struct{
	//SymbolTable globalTable;
	List decls;
	struct astnode * main;
} *Program;

//------------------------------------------------------------------
typedef struct{
	struct astnode *condition;
	struct astnode *action;
} *WhileStmt;

typedef struct astnode{
	enum AST_NODE_KINDS{
		KNumber = 0x200,		// numerial value:
		KVarExpr,
		KParenExpr,		// parentheses expression
		KInfixExpr,		// infix expression
		KPrefixExpr,
		KRelationExper,

		KProgram,	// Program is decls
		KFunction,
		KMainFunction,
		KVariable,
		KConstant,

		KVarDeclStmt,
		KConstDeclStmt,

		KComposeStmt,
		KIfStmt,	//If statement
		KWhileStmt,
		KCallExper,
		KAssignExpr,		// assignment expression
	} kind;	// kind of the AST node

	// information of various kinds of AST node 
	union {		
		int  val;		// KValue
		Symbol sym;		// KVariable , KConstant , KVarExpr
		Expr   expr;		// KPrefixExpr,KInfexExpr,KparenExpr,KAssignExpr,KRelationExpr
		CallExpr callexpr;	//KCallExpr

		Program	program;	//KProgram
		VarDeclStmt vardeclstmt;	//KVariableList,KConstantList
		Function function;	//KFunction

		ComposeStmt compstmt;	//KComposeStmt
		WhileStmt whilestmt;	//KWhileStmt
		IfStmt ifstmt;	//KIfStmt
	};
	Loc 	loc;			// locations
} *ASTNode;

typedef struct astree {
	ASTNode root;
	SymbolTable symTab;
} *ASTree;

// functions for creating various kinds of ASTnodes
bool IsStatement(ASTNode node);
bool IsExpersion(ASTNode node);
bool IsDeclartion(ASTNode node);
// Symbols and declarations
ASTNode newVariable(SymbolTable pTab, const char* name, ASTNode initExpr);
ASTNode newConstant(SymbolTable pTab, const char* name, ASTNode initExpr);
ASTNode newFunction(SymbolTable pTab, const char* name, ASTNode body);
//void destroyVariable();
void destroyFunction(Function *pFunc);

ASTNode newVarDeclStmt(Type type);
ASTNode newConstDeclStmt(Type type);
void	destroyVarDeclStmt(VarDeclStmt *pvarlist);

// Expressions
ASTNode newNumber(int value);
ASTNode newVarExpr(SymbolTable pTab, const char* name);
ASTNode newPrefixExpr(int op, ASTNode exp);
ASTNode newParenExpr(ASTNode exp);
ASTNode newInfixExpr(int op, ASTNode left, ASTNode right);
ASTNode newAssignExpr(int op, ASTNode left, ASTNode right);
ASTNode newRelationExpr(int relop, ASTNode lkid, ASTNode rkid);
void	destroyExpr(Expr *pexp);

ASTNode newCallExpr(SymbolTable pTab, char* name);
void	destroyCallExpr(CallExpr *loop);

ASTNode newProgram();
void	destroyProgram(Program *prog);

//Statments
ASTNode newComposeStmt();
void	destroyComposeStmt(ComposeStmt *loop);
ASTNode newIfStmt(ASTNode condition, ASTNode action);
void	destroyIfStmt(IfStmt *loop);
ASTNode newWhileStmt(ASTNode condition, ASTNode action);
void	destroyWhileStmt(WhileStmt *loop);

void	destroyASTNode(ASTNode *pnode);

ASTree newAST();
void	destroyAST(ASTree *pTree);

void 	dumpASTNode(ASTNode node, int indent);

Loc		setLoc(ASTNode node, Loc loc);

#endif // !_AST_H
