#pragma once

#ifndef _DECLARATION_CONTEXT_
#define _DECLARATION_CONTEXT_

#include <string>
#include <list>
#include <map>
#include <memory>
#include "type.h"

namespace C1
{
	namespace Declarations
	{
		enum DECL_TYPE
		{
			VARIABLE,
			FUNCTION,
			TYPEDEF,
		};

		enum StorageClassSpecifierEnum
		{
			EXTERN,
			STATIC,
			AUTO,
			REGISTER,
		};

		class Declaration
		{
			const std::string& Name() const;
			std::string& Name();

			virtual ~Declaration() = 0;
		};

		class Variable : public Declaration
		{
			const Type* Type() const;
			const size_t Offset() const; //Only valid when it is a struct member ?
		};

		class Function : public Declaration
		{
			const Type* ReturnType() const;
			const std::list<Variable*>& ParameterList() const;
			const Type* FunctionType() const;
		};

		class TypedefName : public Declaration
		{
			const Type* CanonicalType() const;
		};

		class DeclContext
		{
			const DeclContext* ParentContext() const;
			DeclContext* ParentContext();

			const Declaration* Lookup_Recursive(const std::string& name) const;
			Declaration* Lookup_Recursive(const std::string& name);

			const Declaration* Lookup(const std::string& name) const;
			Declaration* Lookup(const std::string& name);

			const Declaration* operator[](const std::string& name) const
			{ return Lookup(name); }
			Declaration* operator[](const std::string& name)
			{ return Lookup(name); }		

			void AddDeclaration(Declaration* declaration);

		private:
			DeclContext* m_ParentContext;
			std::multimap<std::string&, std::unique_ptr<Declaration>> m_Table;
		};
	}
}

#endif // !_DECLARATION_CONTEXT_

