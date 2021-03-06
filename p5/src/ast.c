/*
 * TODO Functions of Abstract Syntax Tree
 * Author: Yu Zhang (yuzhang@ustc.edu.cn)
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

int i = 0;      //记录缩进

char *opname[]={
#undef opxx
#define opxx(a, b) b,
#include "op.h"
	"Undefined Op"
};

ASTNode 
newNumber(float value)    //建立新节点，kind为数
{
	ASTNode new;
	NEW0(new);
	new->kind = KValue;
	new->val = value;
	return new;
}

ASTNode                //建立新节点，kind为id
newName(Table ptab, char *name)
{
	ASTNode new;
	NEW0(new);
	new->kind = KName;
	new->sym = getSym(ptab, name);
	return new;
}

ASTNode                          //建立新节点，kind为括号
newParenExp(ASTNode exp)
{
	ASTNode new;
	NEW0(new);
	new->kind = KParenExp;
	Exp newexp;
	NEW0(newexp);
	newexp->op = -1;
	newexp->kids[0] = exp;
	new->exp = newexp;
	return new;
}

ASTNode
newInfixExp(int op, ASTNode left, ASTNode right)
{
	ASTNode new;
	NEW0(new);
	new->kind = KInfixExp;
	Exp newexp;
	NEW0(newexp);
	new->exp = newexp;

	newexp->op = op;
	newexp->kids[0] = left;
	newexp->kids[1] = right;
	return new;
}

ASTNode
newAssignment(int op, ASTNode left, ASTNode right)
{
	ASTNode new;
	NEW0(new);
	new->kind = KAssignExp;
	Exp newexp;
	NEW0(newexp);

	newexp->op = op;
	newexp->kids[0] = left;
	newexp->kids[1] = right;
	new->exp = newexp;
	return new;
}

ASTNode newProgram(ASTNode block,ASTNode maindef)
{
	ASTNode new;
	NEW0(new);
	new->kind = KProgram;
	Program newprog;
	NEW0(newprog);
	new->program = newprog;

	newprog->block = block;
	newprog->maindef =  maindef;
	return new;	
}

ASTNode newBlock()
{
	ASTNode new;
	NEW0(new);
	new->kind = KBlock;
	Block newbloc;
	NEW0(newbloc);
	new->block = newbloc;

	newbloc->stmts = newList();
	return new;
}

ASTNode newVdecl(ASTNode name,ASTNode vdelf)
{
	ASTNode new;
	NEW0(new);
	new->kind = KVdecl;
	Vdecl newvdec;
	NEW0(newvdec);
	new->vdecl = newvdec;

	newvdec->name = name;
	newvdec->vdelf =  vdelf;
	return new;	
}

ASTNode newVdelf()
{
	ASTNode new;
	NEW0(new);
	new->kind = KVdelf;
	Block newvdlf;
	NEW0(newvdlf);
	new->block = newvdlf;

	newvdlf->stmts = newList();
	return new;
}

ASTNode newCdecl(ASTNode assn,ASTNode cdelf)
{
	ASTNode new;
	NEW0(new);
	new->kind = KCdecl;
	Cdecl newcdel;
	NEW0(newcdel);
	new->cdecl = newcdel;

	newcdel->assn = assn;
	newcdel->cdelf =  cdelf;
	return new;	
}

ASTNode newAssn(ASTNode name,int num)
{
	ASTNode new;
	NEW0(new);
	new->kind = KAssn;
	Assn newassn;
	NEW0(newassn);
	new->assn = newassn;

	newassn->name = name;
	newassn->num = num;
	return new;	
}

ASTNode newCdelf()
{
	ASTNode new;
	NEW0(new);
	new->kind = KCdelf;
	Block newcdelf;
	NEW0(newcdelf);
	new->block = newcdelf;

	newcdelf->stmts = newList();
	return new;
}

ASTNode newFunctionDef(ASTNode name,ASTNode compstat)
{
	ASTNode new;
	NEW0(new);
	new->kind = KFunctionDef;
	FunctionDef newfundef;
	NEW0(newfundef);
	new->functiondef = newfundef;

	newfundef->name = name;
	newfundef->compstat = compstat;
	return new;	
}

ASTNode newMainDef(ASTNode compstat)
{
	ASTNode new;
	NEW0(new);
	new->kind = KMainDef;
	MainDef newmain;
	NEW0(newmain);
	new->maindef = newmain;

	newmain->compstat = compstat;
	return new;	
}

ASTNode newCompStat(ASTNode statf)
{
	ASTNode new;
	NEW0(new);
	new->kind = KCompStat;
	CompStat newcomp;
	NEW0(newcomp);
	new->compstat = newcomp;

	newcomp->statf = statf;
	return new;	
}

ASTNode newStatf()
{
	ASTNode new;
	NEW0(new);
	new->kind = KStatf;
	Block newstatf;
	NEW0(newstatf);
	new->block = newstatf;

	newstatf->stmts = newList();
	return new;
}

ASTNode newStatif(ASTNode relation,ASTNode stat)
{
	ASTNode new;
	NEW0(new);
	new->kind = KStatif;
	Loop newstatif;
	NEW0(newstatif);
	new->loop = newstatif;

	newstatif->relation = relation;
	newstatif->stat = stat;
	return new;	
}

ASTNode newWlop(ASTNode relation,ASTNode stat)
{
	ASTNode new;
	NEW0(new);
	new->kind = KWlop;
	Loop newwlop;
	NEW0(newwlop);
	new->loop = newwlop;

	newwlop->relation = relation;
	newwlop->stat = stat;
	return new;	
}

ASTNode newFunctioncall(ASTNode name)
{
	ASTNode new;
	NEW0(new);
	new->kind = KFunctioncall;
	Functioncall newfuncall;
	NEW0(newfuncall);
	new-> functioncall = newfuncall;

	newfuncall-> name = name;
	return new;	
}

ASTNode newRelation(int relop,ASTNode lkid,ASTNode rkid)
{
	ASTNode new;
	NEW0(new);
	new->kind = KRelation;
	Relation newrelat;
	NEW0(newrelat);
	new-> relation = newrelat;

	newrelat->relop = relop;
	newrelat->kid[0] = lkid;
	newrelat->kid[1] = rkid; 
	return new;
}

void destroyExp(Exp *pnode)
{
	if (*pnode == NULL) return;
	Exp node = *pnode;
	destroyAST(&node->kids[0]);
	destroyAST(&node->kids[1]);
	free(node);
	*pnode = NULL;
}

void
destroyRelation(Relation *relat)
{
	if (*relat == NULL) return;
	Relation node = *relat;
	destroyAST(&node->kid[0]);
	destroyAST(&node->kid[1]);
	free(node);
	*relat = NULL;
}

void	destroyProgram(Program *prog)
{
	if (*prog == NULL) return;
	Program node=*prog;
	destroyAST(&node->block);
	destroyAST(&node->maindef);
	free(node);
	*prog = NULL;
}

void destroyBlock(Block *pnode)
{
	if (*pnode == NULL) return;
	Block node = *pnode;
	destroyList(&node->stmts, destroyAST);
	free(node);
	*pnode = NULL;
}

void	destroyLoop(Loop *pnode)
{
	if (*pnode == NULL) return;
	Loop node = *pnode;
	destroyAST(&node->relation);
	destroyAST(&node->stat);
	free(node);
	*pnode = NULL;
}

void	destroyCompStat(CompStat *pnode)
{
	if (*pnode == NULL) return;
	CompStat node = *pnode;
	destroyAST(&node->statf);
	free(node);
	*pnode = NULL;
}

void	destroyCdecl(Cdecl *pnode)
{
	if (*pnode == NULL) return;
	Cdecl node = *pnode;
	destroyAST(&node->cdelf);
	free(node);
	*pnode = NULL;
}

void	destroyVdecl(Vdecl *pnode)
{
	if (*pnode == NULL) return;
	Vdecl node = *pnode;
	destroyAST(&node->vdelf);
	free(node);
	*pnode = NULL;
}

ASTTree newAST()
{
	ASTTree new;
	NEW0(new);
	return new;
}

void destroyAST(ASTNode *pnode)
{
	if (*pnode == NULL) return;
	ASTNode node = *pnode;
	int kind = node->kind;
	
	switch (kind) {
	case KValue:
	case KName:
	case KFunctioncall:
	case KAssn:
		break;
	case KParenExp:
	case KInfixExp:
	case KAssignExp:
		destroyExp(&node->exp);
		break;
	case KProgram:
		destroyProgram(&node->program);
		break;
	case KBlock:
	case KVdelf:
	case KCdelf:
	case KStatf:
		destroyBlock(&node->block);
		break;
	case KVdecl:
		destroyVdecl(&node->vdecl);
		break;
	case KCdecl:
		destroyCdecl(&node->cdecl);
		break;
	case KRelation:
		destroyRelation(&node->relation);
		break;
	case KWlop:
	case KStatif:
		destroyLoop(&node->loop);
		break;
	case KCompStat:
	case KFunctionDef:
	case KMainDef:
		destroyCompStat(&node->compstat);
		break;
	default:
		printf("Unhandled ASTNode kind!\n");
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

void
dumpAST(ASTNode node)  
{
   if(node != NULL){
	int kind = node->kind;

	switch (kind) {
	case KValue:
	{
		printf("%g", node->val);
		break;
	}
	case KName:
	{
		printf("%s", node->sym->name);
		break;
	}
	case KParenExp:
	{
		printf("(");
		dumpAST(node->exp->kids[0]);
		printf(")");
		break;
	}
	case KInfixExp:
	{
		dumpAST(node->exp->kids[0]);
		printf("%s", opname[node->exp->op]);
		dumpAST(node->exp->kids[1]);
		break;
	}
	case KAssignExp:
	{
		int j;
		for(j=i;j>0;j--)
			printf("\t");
		dumpAST(node->exp->kids[0]);
		printf("%s", opname[node->exp->op]);
		dumpAST(node->exp->kids[1]);
		printf(" ; ");
		break; 
	}
	case KProgram:  
	{
		dumpAST(node->program->block);
		dumpAST(node->program->maindef);
		break;
	}
	case KBlock:  
	{
		List stmts = node->block->stmts;
		ListItr itr = newListItr(stmts, 0);
		while ( hasNext(itr) )  {
			dumpAST((ASTNode)nextItem(itr));
			printf("\n  ");
		}
		destroyListItr(&itr);
		break;		
	}
	case KVdecl:  
	{
		int j;
		for(j=i;j>0;j--)
			printf("\t");
		printf("int ");
		dumpAST(node->vdecl->name);
		dumpAST(node->vdecl->vdelf);
		printf(";\n ");
		break;
	}
	case KVdelf:  
	{
		List stmts = node->block->stmts;
		ListItr itr = newListItr(stmts, 0);
		while ( hasNext(itr) )  {
			printf(" , ");
			dumpAST((ASTNode)nextItem(itr));
	//		printf("\n");
		}
		destroyListItr(&itr);
		break;		
	}

	case KCdecl:  
	{
		int j;
		for(j=i;j>0;j--)
			printf("\t");
		printf("const int ");
		dumpAST(node->cdecl->assn);
		dumpAST(node->cdecl->cdelf);
		printf(";\n");
		break;
	}
	case KAssn:
	{
		dumpAST(node->assn->name);
		printf(" = ");
		printf("%d",node->assn->num);
		break;
	}
	case KCdelf:  
	{
		List stmts = node->block->stmts;
		ListItr itr = newListItr(stmts, 0);
		while ( hasNext(itr) )  {
			printf(" , ");
			dumpAST((ASTNode)nextItem(itr));
//			printf("\n ");
		}
		destroyListItr(&itr);
		break;		
	}
	case KFunctionDef:
	{
		printf("void ");
		dumpAST(node->functiondef->name);
		printf("()\n"  );
		i++;
		dumpAST(node->functiondef->compstat);
		i--;
		break;
	}
	case KMainDef:
	{
		printf("void ");
		printf(" main ");
		printf("()\n");
		i++;
		dumpAST(node->maindef->compstat);
		i--;
		break;
	}
	case KCompStat:
	{
		int j;
		for(j=i;j>1;j--)
			printf("\t");
		printf("{ \n");
		dumpAST(node->compstat->statf);
		for(j=i;j>1;j--)
			printf("\t");
		printf("}\n ");
		break;
	}
	case KStatf:  
	{
		List stmts = node->block->stmts;
		ListItr itr = newListItr(stmts, 0);
		while ( hasNext(itr) )  {
			dumpAST((ASTNode)nextItem(itr));
			printf("\n ");
		}
		destroyListItr(&itr);
		break;		
	}
	case KStatif:
	{
		int j;
		for(j=i;j>0;j--)
			printf("\t");
		printf("if (");
		dumpAST(node->loop->relation);
		i++;
		printf(")\n");
		dumpAST(node->loop->stat);
		i--;
		break;
	}
	case KWlop:
	{
		int j;
		for(j=i;j>0;j--)
			printf("\t");
		printf("while ( ");
		dumpAST(node->loop->relation);
		printf(" )\n");
		i++;
		dumpAST(node->loop->stat);
		i--;
		break;
	}
	case KFunctioncall:
	{
		int j;
		for(j=i;j>0;j--)
			printf("\t");
		dumpAST(node->functioncall->name);
		printf(" () ");
		printf(" ; ");
		printf("\n");
		break;
	}
	case KRelation:
	{
		dumpAST(node->relation->kid[0]);
		printf("%s", opname[node->relation->relop]);
		dumpAST(node->relation->kid[1]);
		break;
	}
	default:
		printf("Unhandled ASTNode kind!\n");
	}
   }
}
