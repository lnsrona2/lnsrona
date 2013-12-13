/*
 * Functions of Symbolic Table
 * Author: Yu Zhang (yuzhang@ustc.edu.cn)
 */
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/**
 * Creates a symbolic table
 */
Table
newTable()
{
	Table new;
	NEW0(new);
	return new;
}

static void
destroyBucket(Entry *list)
{
	Entry node = *list, temp;
	while ( node != NULL ) {
		temp = node->next;
		free(node);
		node = temp;
	}
	*list = NULL;
}

/**
 * Destroys the specified table
 */
void
destroyTable(Table *tab)
{
	int i=0;
	Entry *bucket = (*tab)->buckets, *bucket_end = (*tab)->buckets+256;
	while (bucket < bucket_end ) {
		destroyBucket(bucket);
		bucket++;
	}
	free(*tab);
	*tab = NULL;
}

// former "entry", rename it because the name "entry" is not good
Symbol newSym(Table ptab, const char *name, short type, short lev){                // force create a new entry inside the symbol table
	//tx = tx + 1;
	//strcpy(table[tx].name, name);
	//table[tx].type = type;
	//switch (type){
	//case 0:
	//	if (num>amax){
	//		error(31);
	//		num = 0;
	//	}
	//	table[tx].val = num;
	//	break;
	//case 1:
	//	table[tx].level = lev; table[tx].addr = dx; dx = dx + 1;
	//	break;
	//case 2:
	//	table[tx].level = lev;
	//	break;
	//}

	// IMPORTANT:
	// Hi, my darling, you totally misunderstood the structure and usage of this symbol table.
	// This symbol is a HASH Table.
	// The symbol table inside PL0 project is a stack, but this one is not.
	// As the conclusion, your implemeamentation of the "entry" method is totally wrong...

	Entry pent;
	unsigned hashkey = (unsigned long) name[0] & (HASHSIZE - 1);
	NEW0(pent);
	pent->sym.name = (char *) name;
	pent->sym.val = 0;
	pent->sym.isInitial = FALSE;
	pent->next = ptab->buckets[hashkey];
	ptab->buckets[hashkey] = pent;
	return &pent->sym;
}

// Look up the symbolic table to get the symbol with specified name
Symbol
lookup(Table ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next)
		if ( strcmp(name, pent->sym.name) == 0 ) 
			return &pent->sym;
	return NULL;
}

// Get value of the specified name from the symbolic table
float
getVal(Table ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next)
		if ( strcmp(name, pent->sym.name) == 0 ) 
			return pent->sym.val;
	NEW0(pent);
	pent->sym.name = (char *)name;
	pent->sym.val = 0;
	printf("Warn: %s(%d) was not initialized before, set it 0 as default\n",
		(char *)name, hashkey);
	pent->next = ptab->buckets[hashkey];
	ptab->buckets[hashkey] = pent;
	return 0;
}

Symbol
getSym(Table ptab, const char *name)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next)
		if ( strcmp(name, pent->sym.name) == 0 ) 
			return &pent->sym;
	NEW0(pent);
	pent->sym.name = (char *)name;
	pent->sym.val = 0;
	pent->sym.isInitial = FALSE;
	pent->next = ptab->buckets[hashkey];
	ptab->buckets[hashkey] = pent;
	return &pent->sym;
}

// Set value of the specified name into the symbolic table
Symbol
setVal(Table ptab, const char *name, float val)
{
	Entry pent;
	unsigned hashkey = (unsigned long)name[0] & (HASHSIZE-1);
	for (pent = ptab->buckets[hashkey]; pent!=NULL; pent = pent->next) {
		if ( strcmp(name, pent->sym.name) == 0 ) {
			pent->sym.val = val;
#if DEBUG
			printf("update %s(%d)  %f\n", 
				(char *)name, hashkey, val);
#endif
			return &pent->sym;
		}
	}
	NEW0(pent);
	pent->sym.name = (char *)name;
	pent->sym.val = val;
#if DEBUG
	printf("create %s(%d) %f\n", (char *)name, 
		hashkey, val);
#endif
	pent->next = ptab->buckets[hashkey];
	ptab->buckets[hashkey] = pent;
	return &pent->sym;
}
