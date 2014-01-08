#pragma once

#include "decl_context.h"
#include "type.h"

namespace C1
{
	namespace AST
	{
		class Type;
		class DeclContext;
		class Enumerator;
		class QualifiedTypeSpecifier;
		class TranslationUnit;

		struct ASTContext
		{
		public:
			// The current translation unit
			TranslationUnit* CurrentTranslationUnit;
			// Storage the current working scope , the coming declaration is stored in it
			DeclContext*	CurrentDeclContext;
			// The type context object for creating and retrieving predefined types
			TypeContext*	TypeContext;
			// The name of the current working source file
			std::string		FileName;
			// The input stream for source file
			std::istream*	SourceFile;
			// Storage the qualified type for declarations.
			QualType		CurrentQualifiedType;

		};
	}

}