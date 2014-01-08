#pragma once
#include <map>
#include <string>
#include <list>
#include <memory>

namespace C1
{
	namespace AST{
		class Declaration;
		class NamedDeclaration;

		// A container specified for declarations , provide a list and lookup table interface
		class DeclContext
		{
		public:
			enum InsertionResult
			{
				Success = 0,
				Success_CompatibleRedefinition = 1,
				Success_IncompatibleRedefinition = 2,
				Failed_Redefinition = 3,
				Failed_IncompatibleRedefinition = 4,
			};

			enum NameCollisonPolicy
			{
				NonRedefinfinable = 0,
				CompatibleRedefinable = 1,
				AlwaysRedefinable = 2,
			};

			typedef std::list<Declaration*>::iterator iterator;
			typedef std::list<Declaration*>::const_iterator const_iterator;
			typedef std::list<Declaration*>::reverse_iterator reverse_iterator;
			typedef std::list<Declaration*>::const_reverse_iterator const_reverse_iterator;

			const DeclContext& parent() const
			{
				return m_ParentContext;
			}
			DeclContext& parent()
			{
				return m_ParentContext;
			}

			bool empty() const{
				return m_ListView.empty();
			}
			size_t size() const{ return m_ListView.size(); }

			// Lookup table interfaces
			const Declaration* lookup_local(const std::string& name) const;
			Declaration* lookup_local(const std::string& name);
			const Declaration* lookup(const std::string& name) const;
			Declaration* lookup(const std::string& name);

			const Declaration* operator[](const std::string& name) const
			{return lookup(name);}
			Declaration* operator[](const std::string& name)
			{return lookup(name);}

			// List interfaces
			InsertionResult push_back(NamedDeclaration* declaration, NameCollisonPolicy = NonRedefinfinable);
			InsertionResult push_back(Declaration* declaration);
			//void push_front(Declaration* declaration);
			//void remove(Declaration* declaration);

			// Don't provide non-const access interfaces for safety
			const Declaration* front() const { return m_ListView.front(); }
			const Declaration* back() const { return m_ListView.back(); }
			const_iterator begin() const { return m_ListView.begin(); }
			const_iterator end() const { return m_ListView.end(); }
			const_reverse_iterator rbegin() const { return m_ListView.rbegin(); }
			const_reverse_iterator rend() const { return m_ListView.rend(); }
			//Declaration* front() { return m_ListView.front(); }
			//Declaration* back() { return m_ListView.back(); }
			//iterator begin() { return m_ListView.begin(); }
			//iterator end() { return m_ListView.end(); }
			//reverse_iterator rbegin() { return m_ListView.rbegin(); }
			//reverse_iterator rend() { return m_ListView.rend(); }

		private:
			static const size_t IndexizeThreshold = 10;
			void Indexize();

			DeclContext& m_ParentContext;
			// The basic view
			std::list<Declaration*> m_ListView;
			std::unique_ptr<std::multimap<std::string, NamedDeclaration*>> m_pTableView;
		};

		inline Declaration* DeclContext::lookup_local(const std::string& name)
		{
			auto decl = const_cast<const DeclContext*>(this)->lookup_local(name);
			return const_cast<Declaration*>(decl);
		}

		inline Declaration* DeclContext::lookup(const std::string& name)
		{
			auto decl = const_cast<const DeclContext*>(this)->lookup(name);
			return const_cast<Declaration*>(decl);
		}


	}

}