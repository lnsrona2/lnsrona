#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "C0.h"

lev=0;

void gen(enum fct x, long y, long z){
    if(cx>cxmax){
	printf("program too long\n");
	exit(1);
    }
    code[cx].f=x; code[cx].l=y; code[cx].a=z;
    cx=cx+1;
}

void interpret()
{
	FILE *fp;
	if((fp = fopen("interpret","wb"))==NULL)
		printf("file cannot open\n");
	long i;
	for(i=0;i<cx;i++)
	{
		fwrite(&code[i],sizeof(instruction),1,fp);
		printf("%10ld%5s%3ld%5ld\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
	fclose(fp);	
}

void generate(Table symtab,ASTNode node)
{
	if(node != NULL)
	{
		int kind = node->kind;
		switch (kind)
		{
			case KValue:
			{
				gen (lit,0,node->sym->val);
				break;
			}
			case KName:
			{
				sym s=lookup(symtab,node->sym->name);
				switch(node->sym->type){
					case 0:
						gen(lit,0,node->sym->val); break;
					case 1:
						gen(lod,s->level,s->addr);
						break;
					case 2:
						break;
				}
				break;
			}
			case KParenExp:
			{
				generate(symtab,node->exp->kids[0]);
				break;
			}
			case KInfixExp:
			{
				generate(symtab,node->exp->kids[0]);		
				generate(symtab,node->exp->kids[1]);
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
				sym s=lookup(symtab,(node->exp->kids[0])->sym->name);
				generate(symtab,node->exp->kids[1]);
				gen(sto,s->level,s->addr);
				break; 
			}
			case KProgram:  
			{
				generate(symtab,node->program->block);
				generate(symtab,node->program->maindef);
				break;
			}
			case KBlock:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) )  {
					generate(symtab,(ASTNode)nextItem(itr));
				}
				destroyListItr(&itr);
				break;		
			}
			case KVdecl:  
			{
				enter(symtab,node->vdecl->name,1,lev);
				dx++;
				generate(symtab,node->vdecl->vdelf);
				break;
			}
			case KVdelf:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) ){
					generate(symtab,(ASTNode)nextItem(itr));
				}
				destroyListItr(&itr);
				break;		
			}
			case KCdecl:  
			{
				generate(symtab,node->cdecl->assn);
				generate(symtab,node->cdecl->cdelf);
				break;
			}
			case KAssn:
			{
				enter(symtab,node->assn->name,0,lev);
				dx++;
				generate(symtab,node->assn->name);
				break;
			}
			case KCdelf:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) )  {
					generate(symtab,(ASTNode)nextItem(itr));
				}
				destroyListItr(&itr);
				break;		
			}
			case KFunctionDef:
			{
				lev++;
				enter(symtab,node->assn->name,2,lev);
				dx++;
				generate(symtab,node->functiondef->compstat);
				lev--;
				break;
			}
			case KMainDef:
			{
				lev++;
				generate(symtab,node->maindef->compstat);
				lev--;
				break;
			}
			case KCompStat:
			{
				generate(symtab,node->compstat->statf);
				break;
			}
			case KStatf:  
			{
				List stmts = node->block->stmts;
				ListItr itr = newListItr(stmts, 0);
				while ( hasNext(itr) )  {
					generate(symtab,(ASTNode)nextItem(itr));
					printf("\n ");
				}
				destroyListItr(&itr);
				break;		
			}
			case KStatif:
			{
				long cx1=cx;
				generate(symtab,node->loop->relation);
				cx2=cx;
				gen(jpc,0,0);
				generate(symtab,node->loop->stat);
				code[cx1].a=cx;
				break;
			}
			case KWlop:
			{
				long cx1=cx,cx2;
				generate(symtab,node->loop->relation);
				cx2=cx;
				gen(jpc,0,0);
				generate(symtab,node->loop->stat);
				gen(jmp,0,cx1);
				code[cx2].a=cx;
				break;
			}
			case KFunctioncall:
			{
				sym s=lookup(symtab,node->functioncall->name);
				gen(cal,2,s->addr);
				break;
			}
			case KRelation:
			{
				generate(symtab,node->exp->kids[0]);
				generate(symtab,node->exp->kids[1]);
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
