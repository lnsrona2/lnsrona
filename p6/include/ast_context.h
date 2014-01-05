#pragma once

#include "declaration_context.h"

namespace C1
{
	namespace AST {
		class ASTContext
		{
			DeclContext* CurrentDeclContext();
		};
	}
}