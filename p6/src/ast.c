/*
 * TODO Functions of Abstract Syntax Tree
 * Author: Yu Zhang (yuzhang@ustc.edu.cn)
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

int indent = 0;

char *opname[]={
#undef opxx
#define opxx(a, b) b,
#include "opcfg.h"
	"Undefined Op"
};

ASTNode 
newNumber(int value)
{
	ASTNode node;
	NEW0(node);
	node->kind = KNumber;
	node->val = value;
	return node;
}

ASTNode
newParenExpr(ASTNode exp)
{
	ASTNode node;
	NEW0(node);
	node->kind = KParenExpr;
	Expr newexp;
	NEW0(newexp);
	newexp->op = -1;
	newexp->oprands[0] = exp;
	node->expr = newexp;
	return node;
}

ASTNode
newInfixExpr(int op, ASTNode left, ASTNode right)
{
	ASTNode node;
	NEW0(node);
	node->kind = KInfixExpr;
	Expr newexp;
	NEW0(newexp);
	node->expr = newexp;

	newexp->op = op;
	newexp->oprands[0] = left;
	newexp->oprands[1] = right;
	return node;
}

ASTNode
newAssignExpr(int op, ASTNode left, ASTNode right)
{
	ASTNode node;
	Expr newexp;
	NEW0(node);
	node->kind = KAssignExpr;
	NEW0(newexp);

	newexp->op = op;
	newexp->oprands[0] = left;
	newexp->oprands[1] = right;
	node->expr = newexp;
	return node;
}

ASTNode
newVarExpr(SymbolTable pTab, const char* name)
{
	ASTNode node;
	NEW0(node);
	node->kind = KVarExpr;
	Symbol sym;
	sym = lookupSymbol(pTab, name);
	node->sym = sym;
	if (sym == NULL)
	{
		printf("Error: can't find defination of %s",name);
		//sym = createSymbol(pTab, name);
	}
	return node;
}

ASTNode newProgram()
{
	ASTNode node;
	Program newprog;
	NEW0(node);
	node->kind = KProgram;
	NEW0(newprog);
	node->program = newprog;

	newprog->decls = newList();
	return node;	
}

ASTNode newVarDeclStmt(Type type)
{
	ASTNode node;
	VarDeclStmt decl;
	NEW0(node);
	NEW0(decl);
	node->kind = KVarDeclStmt;
	node->vardeclstmt = decl;
	decl->type = type;
	decl->vars = newList();
	return node;
}

ASTNode newConstDeclStmt(Type type)
{
	ASTNode node;
	VarDeclStmt decl;
	NEW0(node);
	NEW0(decl);
	node->kind = KConstDeclStmt;
	node->vardeclstmt = decl;
	decl->type = type;
	decl->vars = newList();
	return node;
}

ASTNode newVariable(SymbolTable pTab, const char* name, ASTNode initExpr)
{
	ASTNode node;
	Symbol sym = checkSymbol(pTab, name);
	if (sym == NULL)
	{
		sym = createSymbol(pTab, name);
		sym->catalog = SYMBOL_Variable;
		sym->type = INT;
		sym->initexpr = initExpr;
	}
	else
	{
		printf("Error: Re-defination of identifier %s", name);
	}
	NEW0(node);
	node->kind = KVariable;
	node->sym = sym;
	return node;
}

ASTNode newConstant(SymbolTable pTab, const char* name, ASTNode initExpr)
{
	ASTNode node;
	Symbol sym = checkSymbol(pTab, name);
	if (initExpr == NULL)
	{
		printf("Error: Constant must be initialized.");
	}
	if (sym == NULL)
	{
		sym = createSymbol(pTab, name);
		sym->catalog = SYMBOL_Constant;
		sym->type = INT;
		sym->initexpr = initExpr;
	}
	else
	{
		printf("Error: Re-defination of identifier %s.", name);
	}
	NEW0(node);
	node->kind = KConstant;
	node->sym = sym;
	return node;
}


ASTNode newFunction(SymbolTable pTab,const char* name, ASTNode body)
{
	ASTNode node;
	Function func;
	Symbol sym = checkSymbol(pTab, name);
	if (sym == NULL)
	{
		sym = createSymbol(pTab, name);
		sym->catalog = SYMBOL_Functional;
		sym->type = VOID;
	}
	else
	{
		printf("Error: Re-defination of identifier %s", name);
	}
	NEW0(node);
	node->kind = KFunction;
	NEW0(func);
	node->function = func;

	func->sym = sym;
	func->body = body;
	return node;
}

ASTNode newComposeStmt()
{
	ASTNode node;
	NEW0(node);
	node->kind = KComposeStmt;
	ComposeStmt cstmt;
	NEW0(cstmt);
	node->compstmt = cstmt;

	cstmt->stmts = newList();
	return node;
}

void destroyCompStat()
{}


ASTNode newIfStmt(ASTNode condition, ASTNode action)
{
	ASTNode node;
	NEW0(node);
	node->kind = KIfStmt;
	IfStmt ifs;
	NEW0(ifs);
	node->ifstmt = ifs;

	ifs->condition = condition;
	ifs->thenAction = action;
	return node;	
}

ASTNode newWhileStmt(ASTNode condition, ASTNode action)
{
	ASTNode node;
	NEW0(node);
	node->kind = KWhileStmt;
	WhileStmt newwlop;
	NEW0(newwlop);
	node->whilestmt = newwlop;

	newwlop->condition = condition;
	newwlop->action = action;
	return node;	
}

ASTNode newCallExpr(SymbolTable pTab, char* name)
{
	ASTNode node;
	NEW0(node);
	node->kind = KCallExper;
	CallExpr callexpr;
	NEW0(callexpr);
	node->callexpr = callexpr;
	callexpr->sym = lookupSymbol(pTab, name);
	return node;
}

ASTNode newRelationExpr(int relop, ASTNode lkid, ASTNode rkid)
{
	ASTNode node;
	NEW0(node);
	node->kind = KRelationExper;
	Expr newrelat;
	NEW0(newrelat);
	node-> expr = newrelat;

	newrelat->op = relop;
	newrelat->oprands[0] = lkid;
	newrelat->oprands[1] = rkid; 
	return node;
}

void destroyExpr(Expr *pnode)
{
	Expr node = *pnode;
	if (node == NULL) return;
	destroyASTNode(&node->oprands[0]);
	destroyASTNode(&node->oprands[1]);
	free(node);
	*pnode = NULL;
}

void destroyProgram(Program *prog)
{
	Program node=*prog;
	if (node == NULL) return;
	destroyList(&node->decls, (void(*)(void**)) destroyASTNode);
	destroyASTNode(&node->main);
	free(node);
	*prog = NULL;
}

void destroyWhileStmt(WhileStmt *pnode)
{
	WhileStmt node = *pnode;
	if (node == NULL) return;
	destroyASTNode(&node->condition);
	destroyASTNode(&node->action);
	free(node);
	*pnode = NULL;
}

void destroyIfStmt(IfStmt *pnode)
{
	IfStmt node = *pnode;
	if (node == NULL) return;
	destroyASTNode(&node->condition);
	destroyASTNode(&node->thenAction);
	destroyASTNode(&node->elseAction);
	free(node);
	*pnode = NULL;
}

void destroyComposeStmt(ComposeStmt *pnode)
{
	ComposeStmt node = *pnode;
	if (node == NULL) return;
	destroyList(&node->stmts, (void(*)(void**)) destroyASTNode);
	free(node);
	*pnode = NULL;
}

void destroyFunction(Function *pnode)
{
	Function node = *pnode;
	if (node == NULL) return;
	destroySymbol(&node->sym);
	destroyASTNode(&node->body);
	free(node);
	*pnode = NULL;
}

void destroySymbol(Symbol *pSym)
{
	Symbol sym = *pSym;
	if (sym == NULL) return;
	destroyASTNode(&sym->initexpr);
	free(sym->name);
	free(sym);
	*pSym = NULL;
}

void destroyVarDeclStmt(VarDeclStmt *pnode)
{
	VarDeclStmt decls = *pnode;
	if (decls == NULL) return;
	destroyList(&decls->vars, (void(*)(void**)) destroyASTNode);
	free(decls);
	*pnode = NULL;
}

void destroyCallExpr(CallExpr *pnode)
{
	CallExpr node = *pnode;
	if (node == NULL) return;
	destroyList(&node->arglist, (void(*)(void**)) destroyASTNode);
	free(node);
	*pnode = NULL;
}

ASTree newAST()
{
	ASTree node;
	NEW0(node);
	node->symTab = newTable();
	return node;
}

void destroyAST(ASTree *pTree)
{
	ASTree tree = *pTree;
	if (tree == NULL) return;
	destroyASTNode(&tree->root);
	destroyTable(&tree->symTab);
}

void destroyASTNode(ASTNode *pnode)
{
	ASTNode node = *pnode;
	if (*pnode == NULL) return;
	enum AST_NODE_KINDS kind = node->kind;
	switch (kind)
	{
	case KNumber:
		break;
	case KVarExpr:
		break;
	case KAssignExpr:
	case KParenExpr:
	case KInfixExpr:
	case KPrefixExpr:
	case KRelationExper:
		destroyExpr(&node->expr);
		break;
	case KProgram:
		destroyProgram(&node->program);
		break;
	case KFunction:
	case KMainFunction:
		destroyFunction(&node->function);
		break;
	case KVarDeclStmt:
	case KConstDeclStmt:
		destroyVarDeclStmt(&node->vardeclstmt);
		break;
	case KVariable:
	case KConstant:
		destroySymbol(&node->sym);
		break;
	case KComposeStmt:
		destroyComposeStmt(&node->compstmt);
		break;
	case KIfStmt:
		destroyIfStmt(&node->ifstmt);
		break;
	case KWhileStmt:
		destroyWhileStmt(&node->whilestmt);
		break;
	case KCallExper:
		destroyCallExpr(&node->callexpr);
		break;
	default:
		break;
	}
	free(node);
	*pnode = NULL;
}

Loc setLoc(ASTNode node, Loc loc)
{
	if (node->loc == NULL )
		NEW0(node->loc);
	node->loc->first_line = loc->first_line;
	node->loc->first_column = loc->first_column;
	node->loc->last_line = loc->last_line;
	node->loc->last_column = loc->last_column;
	return node->loc;
}

void dumpType(Type type)
{
	char* typeName[] = {"void","int","const int"};
	printf(typeName[type]);
}

void dumpIndent(int indent)
{
	for (int j = 0; j<indent; ++j)
		printf("\t");
}

void
dumpASTNode(ASTNode node, int indent)
{
   if(node != NULL){
	int kind = node->kind;

	switch (kind) {
	case KNumber:
	{
		printf("%d", node->val);
		break;
	}
	case KVarExpr:
	{
		printf("%s", node->sym->name);
		break;
	}
	case KParenExpr:
	{
		printf("(");
		dumpASTNode(node->expr->oprands[0],indent);
		printf(")");
		break;
	}
	case KRelationExper:
	case KInfixExpr:
	{
		dumpASTNode(node->expr->oprands[0], indent);
		printf("%s", opname[node->expr->op]);
		dumpASTNode(node->expr->oprands[1], indent);
		break;
	}
	case KAssignExpr:
	{
		dumpIndent(indent);
		dumpASTNode(node->expr->oprands[0], indent);
		printf("%s", opname[node->expr->op]);
		dumpASTNode(node->expr->oprands[1], indent);
		printf(";\n");
		break; 
	}
	case KProgram:  
	{
		List stmts = node->program->decls;
		ListItr itr = newListItr(stmts, 0);
		while (hasNext(itr))  {
			dumpASTNode((ASTNode) nextItem(itr), indent);
		}
		destroyListItr(&itr);
		dumpASTNode(node->program->main, indent);
		break;
	}
	case KConstDeclStmt:
	case KVarDeclStmt:  
	{
		dumpIndent(indent);
		//printf("int ");
		dumpType(node->vardeclstmt->type);
		putchar(' ');
		//printf(node->vardeclstmt->type);
		List stmts = node->vardeclstmt->vars;
		ListItr itr = newListItr(stmts, 0);
		while (hasNext(itr))  {
			if (hasPrevious(itr)) printf(", ");
			dumpASTNode((ASTNode) nextItem(itr), indent);
		}
		destroyListItr(&itr);
		printf(";\n");
		break;
	}
	case KVariable:
	case KConstant:
	{
		printf(node->sym->name);
		if (node->sym->initexpr)
			printf(" = ");
			dumpASTNode(node->sym->initexpr, indent);
		break;
	}
	case KMainFunction:
	case KFunction:
	{
		printf("void ");
		printf(node->function->sym->name);
		printf("()\n");
		dumpASTNode(node->function->body, indent);
		break;
	}
	case KComposeStmt:
	{
		List stmts = node->compstmt->stmts;
		ListItr itr = newListItr(stmts, 0);
		dumpIndent(indent);
		printf("{\n");
		while ( hasNext(itr) )  {
			dumpASTNode((ASTNode) nextItem(itr), indent + 1);
		}
		dumpIndent(indent);
		printf("}\n");
		destroyListItr(&itr);
		break;		
	}
	case KIfStmt:
	{
		dumpIndent(indent);
		printf("if (");
		dumpASTNode(node->ifstmt->condition, indent);
		printf(")\n");
		if (node->ifstmt->thenAction->kind != KComposeStmt)
			dumpASTNode(node->ifstmt->thenAction, indent + 1);
		else
			dumpASTNode(node->ifstmt->thenAction, indent);
		break;
	}
	case KWhileStmt:
	{
		dumpIndent(indent);
		printf("while (");
		dumpASTNode(node->whilestmt->condition, indent);
		printf(" )\n");
		if (node->whilestmt->action->kind != KComposeStmt)
			dumpASTNode(node->whilestmt->action, indent + 1);
		else
			dumpASTNode(node->whilestmt->action, indent);
		break;
	}
	case KCallExper:
	{
		dumpIndent(indent);
		printf(node->callexpr->sym->name);
		printf("()");
		printf(";\n");
		break;
	}
	default:
		printf("Unhandled ASTNode kind!\n");
	}
   }
}
