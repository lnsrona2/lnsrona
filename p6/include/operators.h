#pragma once

#ifndef _OPERATORS_H_
#define _OPERATORS_H_
namespace C1
{
	enum OperatorsEnum
	{
#define opxx(a,b,c) OP_##a,
#include "opcfg.h"
#undef opxx
	};
}

#endif // !_OPERATORS_H_
