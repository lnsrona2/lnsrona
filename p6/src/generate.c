#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

void gen(enum fct x, long y, long z){
    if(cx>cxmax){
	printf("program too long\n");
	exit(1);
    }
    code[cx].f=x; code[cx].l=y; code[cx].a=z;
    cx=cx+1;
}

void generate(ASTNode node)
{
   if(node != NULL){
	int kind = node->kind;
	switch (kind) {
	case KValue:
	{
		gen (lit,0,root->value) ;
		break;
	}
	case KName:
	{
		int lev ;
 		int val=getval (symtab,tpt,root->sym->name) ;
 		long addr=getAddr(symtab,tpt,root->sym->name,&lev) ;
		switch (root−>sym−>type) {
			case 0 : 
				gen(lit,0,val) ;
				break ;
			case 1 : 
				gen(lod,abs(lev−(tptr != NULL) ),addr) ;
				break ;
			case 2 : 
				printf ("use functionname in exp \n" );
				exit ( −1) ;
				break;
		}
		break;
	}
	case KParenExp:
	{
		generate(symtab,root->exp->kid[0]);
		break;
	}
	case KInfixExp:
	{
		generate(symtab,root->exp->kid[0]);
		if(root->exp->op==op_xx){
			generate(symtab,root->exp->kid[1]);
			switch(root->exp->op){
				case PLUS:
					gen(opr,0,2);break;
				case MINUS:
					gen(opr,0,3);break;
				case MULT:
					gen(opr,0,4);break;
				case DIV:
					gen(opr,0,5);break;
			}
		}
		else gen(opr,0,6);break;

		break;
	}
	case KAssignExp:
	{
		Genarate(node->exp->kids[1]);
		Generate(node->exp->kids[0]);
		gen(sto,0,dx)
		break; 
	}
	case KProgram:  
	{
		Generate(node->program->block);
		Generate(node->program->maindef);
		break;
	}
	case KBlock:  
	{
		List stmts = node->block->stmts;
		ListItr itr = newListItr(stmts, 0);
		while ( hasNext(itr) )  {
			Generate((ASTNode)nextItem(itr));
			printf("\n  ");
		}
		destroyListItr(&itr);
		break;		
	}
	case KVdecl:  
	{
		setAddr(symtab,tptr,dx,root−>vdecl−>name);
 		dx ++;
 		generate(symtab , root −>vdecl−>vdelf ) ;
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
		generate(root->cdecl->assn);
 		dx ++;
 		generate(symtab , root −>cdecl−>cdelf ) ;
		break;
	}
	case KAssn:
	{
		setAddr(symtab,tptr,dx,root−>assn−>name);
		gen(lit,0,num);
		break;
	}
	case KCdelf:  
	{
		List stmts = node->block->stmts;
		ListItr itr = newListItr(stmts, 0);
		while ( hasNext(itr) )  {
			printf(" , ");
			generate((ASTNode)nextItem(itr));
		}
		destroyListItr(&itr);
		break;		
	}
	case KFunctionDef:
	{
		generate(symtab,root->functiondef->compstat)
		break;
	}
	case KMainDef:
	{
		generate (symtab , root−>maindef−>compstat) ;
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
		long cx1 = cx , cx2 ;
 		generate ( symtab , root−>loop−>relation ) ;
 		cx2 = cx ;
		gen (jpc ,0 ,0) ;
 		generate ( symtab , root−>loop−>stat ) ;
 		gen (jmp , 0 , cx1) ;
		code [cx2] . a = cx ;
		break;
	}
	case KWlop:
	{
		long cx1 = cx , cx2 ;
 		generate ( symtab , root−>loop−>relation ) ;
 		cx2 = cx ;
		gen (jpc ,0 ,0) ;
 		generate ( symtab , root−>loop−>stat ) ;
 		gen (jmp , 0 , cx1) ;
		code [cx2] . a = cx ;
		break;
	}
	case KFunctioncall:
	{
		gen(cal,0,cx);
		break;
	}
	case KRelation:
	{
		Genarate(node->exp->kids[0]);
		Generate(node->exp->kids[1]);
		switch ( roor->exp->op ) {
			case EQ :
				gen(opr,0,8); break;
			case NE:
				gen(opr,0,9); break;
			case LT:
				gen(opr,0,10); break;
			case BE:
				gen(opr,0,11); break;	
			case GT:
				gen(opr,0,12); break;
			case LE:
				gen(opr,0,13); break;
			default :
				printf("relop error\n");
		}
		break;
	}
	default:
		printf("unvalid node\n");
}
