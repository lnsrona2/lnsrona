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
