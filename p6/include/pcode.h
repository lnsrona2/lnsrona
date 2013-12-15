#ifndef _PCODE_H_
#define _PCODE_H_

enum fct {
	lit, opr, lod, sto, cal, Int, jmp, jpc         // functions
};
// operator kinds
// You could add more kinds of error messages into op.h 
enum OPERATORS {
#define opxx(a, b) OP_##a,
#include "opcfg.h"
#undef opxx
	OPLAST
};

static char* fctname [] = {"lit","opr","lod","sto","cal","int","jmp","jpc"};

typedef struct{
	enum fct f;		// function code
	long l; 		// level
	long a; 		// displacement address
} instruction;
/*  lit 0, a : load constant a
	opr 0, a : execute operation a
	lod l, a : load variable l, a
	sto l, a : store variable l, a
	cal l, a : call procedure a at level l
	Int 0, a : increment t-register by a
	jmp 0, a : jump to a
	jpc 0, a : jump conditional to a       */

#define BLOCK_MARK_SIZE 3

#endif //if! _PCODE_H_
