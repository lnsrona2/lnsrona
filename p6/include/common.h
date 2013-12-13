/* 
 * Common definitions and declarations for compilers 
 * Author: Yu Zhang (yuzhang@ustc.edu.cn)
 */
#ifndef _COMMON_H_
#define _COMMON_H_
#include "util.h"

// operator kinds
// You could add more kinds of error messages into op.h 
enum {
#define opxx(a, b) OP_##a,
#include "opcfg.h"
#undef opxx
	OPLAST
};

//extern char **opname;

#include "symtab.h"
// symbolic table
//typedef struct symbol {
//	char	*name;	// name of the symbol
//	bool	isInitial;	// whether it is initialized	
//	short	type;	// type of the symbol
//	float	val;	// value of the symbol
//	short	level;	// level of the symbol
//	long	addr;	// address of the symbol
//} *Symbol;
//
//typedef struct entry {
//	struct symbol sym;
//	struct entry *next;
//} *Entry;
//
//typedef struct table {
//	// a hashtable to store symbols
//	struct entry *buckets[256];
//} *Table;
//#define HASHSIZE 256

//// Function declarations corresponding to symbolic table
//Table 	newTable();
//Symbol 	lookup(Table ptab, const char *name);
//Symbol 	newSym(Table ptab, const char *name, short type, short lev);
//Symbol 	getSym(Table ptab, const char *name);
//float 	getVal(Table ptab, const char *name);
//Symbol 	setVal(Table ptab, const char *name, float val);
//void 	destroyTable();

// Error/warning message
// You could add more kinds of error messages into errcfg.h 

#include "error.h"

#include "ast.h"
//// An error/warning message
//
//typedef struct errmsg{
//	bool isWarn;
//	int type;
//	char* msg;
//	int line;
//	int column;
//} *Errmsg;
//
//// Error factory
//typedef struct errfactory { 
//	List	errors;
//	List	warnings;
//} *ErrFactory;
//
//// Function declarations on error message management
//Errmsg	newError	(ErrFactory errfactory, int type, int line, int col);
//Errmsg	newWarning	(ErrFactory errfactory, int type, int line, int col);
//void	dumpErrmsg	(Errmsg error);
//ErrFactory newErrFactory();
//void	dumpErrors	(ErrFactory errfactory);
//void	dumpWarnings	(ErrFactory errfactory);
//void	destroyErrFactory(ErrFactory *errfact);

// abstract syntax tree
// Structure for tracking locations, same as YYLTYPE in y.tab.h--------------------------------------

//typedef struct location {
//	int first_line;
//	int first_column;
//	int last_line;
//	int last_column;
//} *Loc;
//
//typedef struct {
//	int 	op;
//	//int type;
//	int 	val;
//	struct astnode	*kids[2];// kids of the AST node
//} *Exp;
//
//
//typedef struct {
//	List  stmts;
//} *Block;
//
//typedef struct{
//	int relop;
//	bool truth;	
//	struct astnode *kid[2];
//} *Relation;
//
//typedef struct{
//	struct astnode *name;
//} *Functioncall;
//
//typedef struct{
//	struct astnode *relation;
//	struct astnode *stat;
//} *Loop;
//
//typedef struct{
//	struct astnode *statf;
//} *CompStat;
//
//typedef struct{
//	struct astnode *compstat;
//} *MainDef;
//
//typedef struct{
//	struct astnode *name;
//	struct astnode *compstat;
//} *FunctionDef;
//
//typedef struct{
//	struct astnode *name;
//	int num;
//} *Assn;
//
//typedef struct{
//	struct astnode *assn;
//	struct astnode *cdelf;
//} *Cdecl;
//
//typedef struct{
//	struct astnode *name;
//	struct astnode *vdelf;
//} *Vdecl;
//
//typedef struct{
//	struct astnode *block;
//	struct astnode *maindef;
//} *Program;
//
////------------------------------------------------------------------
//
//typedef struct astnode{
//	enum {
//		KValue = 0x200,		// numerial value:
//		KName,			// name, such as variable name
//		KInfixExp,		// infix expression
//		KAssignExp,		// assignment expression
//		KParenExp,		// parentheses expression
//		KProgram,		
//		KBlock,			// block
//		KVdecl,
//		KVdelf,
//		KCdecl,
//		KAssn,
//		KCdelf,
//		KFunctionDef,
//		KMainDef,
//		KCompStat,
//		KStatf,
//		KStatif,
//		KWlop,
//		KFunctioncall,
//		KRelation,
//	} kind;	// kind of the AST node
//	union {		// information of various kinds of AST node 
//		float  val;		// KValue: numerial value
//		Symbol sym;		// KName: symbols 
//		Exp   exp;		// KPrefixExp,
//					// KInfixExp,
//					// KAssignExp,
//					// KParenExp
//		Program	program;	
//		Block block;			// block
//		Vdecl vdecl;
//		Cdecl cdeclar; 
//			//"cdecl" is a common world represent C style calling convention, suggest aother name here
//			//name in "cdecl" caused build break in MSVC 12(VS 2013)
//		Assn assn;
//		FunctionDef functiondef;
//		MainDef maindef;
//		CompStat compstat;
//		Loop loop;
//		Functioncall functioncall;
//		Relation relation;
//	};
//	Loc 	loc;			// locations
//} *ASTNode;
//
//typedef struct ASTtree {
//	struct astnode *root;
//} *ASTTree;
//
//// functions for creating various kinds of ASTnodes
//ASTNode newNumber(float value);
//ASTNode newName(Table ptab, char *name);
//ASTNode newPrefixExp(int op, ASTNode exp);
//ASTNode newParenExp(ASTNode exp);
//ASTNode newInfixExp(int op, ASTNode left, ASTNode right);
//ASTNode newAssignment(int op, ASTNode left, ASTNode right);
//void	destroyExp(Exp *pexp);
//ASTNode newProgram(ASTNode block,ASTNode maindef);
//void	destroyProgram(Program *prog);
//ASTNode newBlock();                       
//void	destroyBlock(Block *pblock);
//ASTNode newVdecl(ASTNode name,ASTNode vdelf);
//void	destroyVdecl(Vdecl *pnode);
//ASTNode newVdelf();                      
//ASTNode newCdecl(ASTNode assn,ASTNode cdelf);
//void	destroyCdecl(Cdecl *pnode);
//ASTNode newAssn(ASTNode name,int num);
//ASTNode newCdelf();                     
//ASTNode newFunctionDef(ASTNode name,ASTNode compstat);
//ASTNode newMainDef(ASTNode compstat);
//ASTNode newCompStat(ASTNode statf);
//void	destroyCompStat(CompStat *pmain);
//ASTNode newStatf();                  
//ASTNode newStatif(ASTNode relation,ASTNode stat);
//ASTNode newWlop(ASTNode relation,ASTNode stat);
//void	destroyLoop(Loop *loop);
//ASTNode newFunctioncall(ASTNode name);
//ASTNode newRelation(int relop,ASTNode lkid,ASTNode rkid);
//void	destroyRelation(Relation *prelation);
//ASTTree newAST();
//void	destroyAST(ASTNode *pnode);
//void 	dumpAST(ASTNode node);
//Loc	setLoc(ASTNode node, Loc loc);

#endif // !def(_COMMON_H_)
