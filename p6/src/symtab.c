/*
* Functions of Symbolic Table (Stack)
* Author: Yu Zhang (yuzhang@ustc.edu.cn) , Yupeng Zhang (ypzhange@mail.ustc.edu.cn)
*/
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "symtab.h"
#include "util.h"

/**
* Creates a symbolic table
*/
SymbolTable
newTable()
{
	SymbolTable new;
	NEW0(new);
	new->top = 1;
	return new;
}

static void
destroyBucket(Entry *list)
{
	Entry node = *list, temp;
	while (node != NULL) {
		temp = node;
		node = temp->next;
		free(temp);
	}
	*list = NULL;
}

/**
* Destroys the specified table
*/
void
destroyTable(SymbolTable *tab)
{
	int i = 0;
	Entry *bucket = (*tab)->buckets, *bucket_end = (*tab)->buckets + 256;
	while (bucket < bucket_end) {
		destroyBucket(bucket);
		bucket++;
	}
	free(*tab);
	*tab = NULL;
}

Symbol
checkSym(SymbolTable ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long) name[0] & (HASHSIZE - 1);
	int lim = ptab->base[ptab->top - 1];
	//	printf("CheckSymBase:%d\n",lim);
	for (pent = ptab->buckets[hashkey]; (pent != NULL) && ((pent->id)>lim); pent = pent->next)
	if (strcmp(name, pent->sym->name) == 0)
	{
		printf("Re-Define Detail: '%s' (%ld)\n", pent->sym->name, pent->id);
		return pent->sym;
	}
	return NULL;
}


Symbol
lookupSymbol(SymbolTable ptab, const char *name)
{
	//	printf("look for ID : '%s'\n",name);
	Entry pent;
	unsigned hashkey = (unsigned long) name[0] & (HASHSIZE - 1);
	for (pent = ptab->buckets[hashkey]; pent != NULL; pent = pent->next)
	{
		//		printf("looked ID : '%s' id =%ld ",pent->sym.name,pent->sym.id);
		if (strcmp(name, pent->sym->name) == 0)
			//		{
			//			printf(", match!\n");
			return pent->sym;
		//		}
		//		else printf(", but not match\n");

	}
	//	printf("Never match.Lookup failed.\n");
	return NULL;
}

Symbol
checkSymbol(SymbolTable ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long) name[0] & (HASHSIZE - 1);
	int lim = ptab->base[ptab->top - 1];
	//	printf("CheckSymBase:%d\n",lim);
	for (pent = ptab->buckets[hashkey]; (pent != NULL) && ((pent->id)>lim); pent = pent->next)
	if (strcmp(name, pent->sym->name) == 0)
	{
		printf("Re-Defination Detail: '%s' (%ld)\n", pent->sym->name, pent->id);
		return pent->sym;
	}
	return NULL;
}


Symbol
createSymbol(SymbolTable ptab, const char *name)
{
	Entry pent;
	Symbol sym;
	unsigned hashkey = (unsigned long) name[0] & (HASHSIZE - 1);
	NEW0(pent);
	NEW0(sym);
	sym->name = (char *)malloc(strlen(name)+1);
	strcpy(sym->name, name);

	sym->isInitial = false;
	sym->level = ptab->top - 1;

	pent->sym = sym;
	pent->next = ptab->buckets[hashkey];
	pent->id = ++(ptab->base[ptab->top]);
	ptab->buckets[hashkey] = pent;
	Fakentry sp;
	NEW0(sp);
	sp->enode = pent;
	sp->next = ptab->index;
	ptab->index = sp;
	return pent->sym;
}

void
pushTable(SymbolTable ptab)
{
	//	printf("PushTable\n");
	ptab->top++;
	ptab->base[ptab->top] = ptab->base[ptab->top - 1];
}

void
popTable(SymbolTable ptab)
{
	//	printf("PopTable\n");
	Fakentry sp = ptab->index;
	Entry pent;
	unsigned hashkey;
	int N, i;
	i = ptab->base[ptab->top];
	ptab->top--;
	N = ptab->base[ptab->top];
	while (i>N){
		pent = sp->enode;
		hashkey = (unsigned long) ((pent->sym->name)[0]) & (HASHSIZE - 1);
		ptab->buckets[hashkey] = pent->next;
		free(pent);
		ptab->index = sp->next;
		free(sp);
		sp = ptab->index;
		i--;
	}
}

