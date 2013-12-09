#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "C0.h"

void gen(enum fct x, long y, long z){
	if(cx>cxmax){
	printf("program too long\n");
	exit(1);
	}
	code[cx].f=x; code[cx].l=y; code[cx].a=z;
	cx=cx+1;
}

void Print(instruction *code){
	
}

void generate(ASTNode node)
{
	if(node != NULL)
	{
		int kind = node->kind;
		switch (kind)
		{
			case KValue:
			{
				gen (lit,0,node->sym->val) ;
				break;
			}
			case KName:
			{
//				int lev;
//				int val=getval(node->sym->name);
//				long addr=getAddr(node->sym->name,&lev);
				switch(node->sym->type){
					case 0:
						gen(lit,0,node->sym->val); break;
					case 1:
						gen(lod,abs(lev),dx);
						break;
					case 2:
						printf("use functionname in exp\n");
						exit(-1);
						break;
				}
				break;
			}
			case KParenExp:
			{
				generate(node->exp->kids[0]);
				break;
			}
			case KInfixExp:
			{
				generate(node->exp->kids[0]);		
				generate(node->exp->kids[1]);
				switch(node->exp->op){
					case OP_PLUS:
						gen(opr,0,2);break;
					case OP_MINUS:
						gen(opr,0,3);break;
					case OP_MULT:
						gen(opr,0,4);break;
					case OP_DIV:
						gen(opr,0,5);break;
					default:
						break;
				}	
				break;
			}
			case KAssignExp:
			{
				generate(node->exp->kids[1]);
				generate(node->exp->kids[0]);
				gen(sto,0,dx);
				break; 
			}
			case KProgram:  
			{
				generate(node->program->block);
				generate(node->program->maindef);
				break;
			}
			case KBlock:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) )  {
					generate((ASTNode)nextItem(itr));
					printf("\n  ");
				}
				destroyListItr(&itr);
				break;		
			}
			case KVdecl:  
			{
				generate(node->vdecl->name);
				dx++;
				generate(node->vdecl->vdelf);
				break;
			}
			case KVdelf:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) ){
					generate((ASTNode)nextItem(itr));
				}
				destroyListItr(&itr);
				break;		
			}
			case KCdecl:  
			{
				generate(node->cdeclar->assn);
				dx++;
				generate(node->cdeclar->cdelf);
				break;
			}
			case KAssn:
			{
				generate(node->assn->name);
				gen(lit,0,num);
				break;
			}
			case KCdelf:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) )  {
					generate((ASTNode)nextItem(itr));
				}
				destroyListItr(&itr);
				break;		
			}
			case KFunctionDef:
			{
				generate(node->functiondef->compstat);
				break;
			}
			case KMainDef:
			{
				generate(node->maindef->compstat);
				break;
			}
			case KCompStat:
			{
				generate(node->compstat->statf);
				break;
			}
			case KStatf:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) )  {
					generate((ASTNode)nextItem(itr));
					printf("\n ");
				}
				destroyListItr(&itr);
				break;		
			}
			case KStatif:
			{
				long cx1=cx,cx2;
				generate(node->loop->relation);
				cx2=cx;
				gen(jpc,0,0);
				generate(node->loop->stat);
				gen(jmp,0,cx1);
				code[cx2].a=cx;
				break;
			}
			case KWlop:
			{
				long cx1=cx,cx2;
				generate(node->loop->relation);
				cx2=cx;
				gen(jpc,0,0);
				generate(node->loop->stat);
				gen(jmp,0,cx1);
				code[cx2].a=cx;
				break;
			}
			case KFunctioncall:
			{
				gen(cal,0,cx);
				break;
			}
			case KRelation:
			{
				generate(node->exp->kids[0]);
				generate(node->exp->kids[1]);
				switch ( node->exp->op ) {
					case OP_EQ :
						gen(opr,0,8); break;
					case OP_NE:
						gen(opr,0,9); break;
					case OP_LT:
						gen(opr,0,10); break;
					case OP_BE:
						gen(opr,0,11); break;	
					case OP_GT:
						gen(opr,0,12); break;
					case OP_LE:
						gen(opr,0,13); break;
					default :
						printf("relop error\n");
				}
				break;
			}
			default:
				printf("Unhandled ASTNode kind!\n");
		}
	}
}
