/*
 * Functions of Symbolic Table
 * Author: Yu Zhang (yuzhang@ustc.edu.cn)
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <common.h>

/**
 * Creates a symbolic table
 */
Table
newTable()    //建立新的函数符号表
{
	Table new;
	NEW0(new);
	return new;
}

static void
destroyBucket(Entry *list)   //删除符号表中的节点
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
destroyTable(Table *tab)  //释放整个符号表
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
lookup(Table ptab, const char *name)  //查询符号表
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
getVal(Table ptab, const char *name)  //查找name对应的val属性。若不存在，构建节点name，val设置为0
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
getSym(Table ptab, const char *name)  //从符号表中查找name对应的sym属性，若不存在，构建节点name
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
setVal(Table ptab, const char *name, float val)    //将符号表中name对应的val修改为输入值
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
