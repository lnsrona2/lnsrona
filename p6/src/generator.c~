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
		gen ( lit , 0 , root->value ) ;
		break;
	}
	case KName:
	{
		int l e v ;
 		int v a l = g e t V a l ( symtab , tptr, root->sym->name) ;
 		long addr = getAddr ( symtab , tptr, root->sym->name,&lev) ;
		switch ( r o o t −>sym−>type ) {


			case 0 : gen ( l i t , 0 , v a l ) ;

			break ;
			case 1 : gen ( lod , abs ( l e v − ( t p t r != NULL) ) ,addr ) ;
			break ;

			case 2 : p r i n t f ( ” u s e ␣ f u n c t i o n ␣name␣ i n ␣ exp \n” ) ;
			 e x i t ( −1) ;
			break;
		}
	}
	case KParenExp:
	{
		g e n e r a t e ( symtab , r o o t −>exp−>k i d s [ 0 ] ) ;
		break;
	}
	case KInfixExp:
	{
		generate(symtab,root->exp->kid[0]);
		if(root->exp->op==op_xx){
			ge n e r a t e ( symtab , r o o t −>exp−>k i d s [ 1 ] ) ;
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
		gen(sar,0,0)
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
		setAddr ( symtab , t p t r , dx , r o o t −>v d e c l −>i d e n t −>sym−>name ) ;
 		dx ++;
 		g e n e r a t e ( symtab , r o o t −>v d e c l −>v a r d e f ) ;
		break;
	}
	case KVdelf:  
	{
		List stmts = node->block->stmts;
		ListItr itr = newListItr(stmts, 0);
		while ( hasNext(itr) ){
			generate((ASTNode)nextItem(itr));
	//		printf("\n");
		}
		destroyListItr(&itr);
		break;		
	}

	case KCdecl:  
	{
		
		break;
	}
	case KAssn:
	{
		addr(symtab,name);
		generate(node->assn->name);
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
		generate(symtab,root->program->block)
		break;
	}
	case KMainDef:
	{
		g e n e r a t e ( symtab , r o o t −>program−>b l o c k ) ;
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
		setaddr(symtab,root)
		generate(node->loop->relation);
		generate(node->loop->stat);
		break;
	}
	case KWlop:
	{
		long cx1 = cx , cx2 ;
 		g e n e r a t e ( symtab , r o o t −>wstmt−>cond ) ;
 		cx2 = cx ;
		gen ( jpc , 0 , 0 ) ;
 		g e n e r a t e ( symtab , r o o t −>wstmt−>stmt ) ;
 		gen ( jmp , 0 , cx1 ) ;
		code [ cx2 ] . a = cx ;
		break;
	}
	case KFunctioncall:
	{
		
		break;
	}
	case KRelation:
	{
		g e n e r a t e ( symtab , r o o t −>exp−>k i d s [ 0 ] ) ;
		 g e n e r a t e ( symtab , r o o t −>exp−>k i d s [ 1 ] ) ;
		switch ( r o o t −>exp−>op ) {
345
case OP_DEQL:
346
347
break ;
case OP_GTR:
348
349
case OP_LSS :
case OP_GEQ:
case OP_LEQ:
gen ( opr , 0 , 1 3 ) ;
break ;
case OP_NEQ:
356
357
gen ( opr , 0 , 1 1 ) ;
break ;
354
355
gen ( opr , 0 , 1 0 ) ;
break ;
352
353
gen ( opr , 0 , 1 2 ) ;
break ;
350
351
gen ( opr , 0 , 8 ) ;
gen ( opr , 0 , 9 ) ;
break ;
default :
p r i n t f ( ” r e l o p ␣ e r r o r \n” ) ;
		break;
	}
	default:
		printf("Unhandled ASTNode kind!\n");
	}
   }
}
