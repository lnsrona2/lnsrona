/*
* Functions of Symbolic Table (Stack)
* Author: Yu Zhang (yuzhang@ustc.edu.cn) , Yupeng Zhang (ypzhange@mail.ustc.edu.cn)
* Usage:
*	This is a simple implementation of a symbol table stack.
*	Use createSymbol to create a new symbol inside the sybol table stack
*	with lookupSymbol to return the first finded one or NULL to not find
*	And use checkSymbol to check for a re-defination
*	Use pushTable when you get inside a new scope
*	And popTable when you leave a scope
*/
#ifndef _SYMTAB_H
#define _SYMTAB_H

#include <stdbool.h>

enum SymbolCatalog
{
	SYMBOL_Constant,
	SYMBOL_Variable,
	SYMBOL_Functional,
	SYMBOL_Type,
	SYMBOL_Template,
	SYMBOL_NameSpace,
	None
};
// symbolic table
typedef struct symbol {
	char	*name;	// name of the symbol
	bool	isInitial;	// whether it is initialized	
	int		val;
	//int		type;
	enum SymbolCatalog catalog; // changed from type 

	//Type   type;	// type of the symbol
	//struct astnode	*initexp;	// value of the symbol
	int	addr;	// addr of the symbol
	int	level;  // if is a local symbol
	struct astnode	*declaration;	//Pointer to the declaration.
} *Symbol;

typedef struct entry {
	struct symbol	sym;
	struct entry	*next;
	long			id;	// using this to liner the hash tab.
} *Entry;

typedef struct fakentry {
	struct entry *enode;
	struct fakentry *next;
} *Fakentry;

typedef struct table {
	// a hashtable to store symbols
	struct entry *buckets[256];
	Fakentry index;
	int top;
	int base[100]; //a stack to store the 
} *SymbolTable;
#define HASHSIZE 256

// Function declarations corresponding to symbolic table
SymbolTable 	newTable();
void 	destroyTable();

/// <summary>
/// Lookup for the first symbol with the specified name inside a symbolic table stack.
/// Retrun NULL for not found.
/// </summary>
/// <param name="ptab">The Symbol table stack your are looking.</param>
/// <param name="name">The symbol's name</param>
/// <returns>NULL for not found</returns>
Symbol 	lookupSymbol(SymbolTable ptab, const char *name);


/// <summary>
/// Perform a re-defination check for the specifec name. The pass condition is there is no same name exist inside the same level of symbol table.
/// Return NULL for success.
/// Return the pre-defined symbol for failed.
/// </summary>
/// <param name="ptab">The Symbol table stack your are looking.</param>
/// <param name="name">The symbol's name</param>
/// <returns>Return NULL for success, return with a specific pointer for failed.</returns>
Symbol	checkSymbol(SymbolTable ptab, const char *name);

/// <summary>
/// Creates the symbol without any check of invialid.
/// The typical usage is checkSymbol first and createSymbol followed.
/// </summary>
/// <param name="ptab">The Symbol table stack your are looking.</param>
/// <param name="name">The symbol's name</param>
/// <returns></returns>
Symbol 	createSymbol(SymbolTable ptab, const char *name);

// Use this function when you enter a new scope to save the mark a new layler in the symbol table
void	pushTable(SymbolTable ptab);
// Use this function when you leave a scope to discard the current layler in the symbol table
void	popTable(SymbolTable ptab);

//float 	getVal(Table ptab, const char *name);
//Symbol 	setVal(Table ptab, const char *name, float val);
#endif // !_SYMTAB_H