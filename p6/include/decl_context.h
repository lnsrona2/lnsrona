#pragma once
#include <map>
#include <string>
#include <memory>

namespace C1
{
	namespace AST{
		class Declaration;
		class DeclContext
		{
		public:
			const DeclContext* ParentContext() const;
			DeclContext* ParentContext();

			const Declaration* Lookup_Recursive(const std::string& name) const;
			Declaration* Lookup_Recursive(const std::string& name);

			const Declaration* Lookup(const std::string& name) const;
			Declaration* Lookup(const std::string& name);

			const Declaration* operator[](const std::string& name) const
			{
				return Lookup(name);
			}
			Declaration* operator[](const std::string& name)
			{
				return Lookup(name);
			}

			void AddDeclaration(Declaration* declaration);

		private:
			DeclContext* m_ParentContext;
			std::multimap<std::string&, std::unique_ptr<Declaration>> m_Table;
		};
	}

}