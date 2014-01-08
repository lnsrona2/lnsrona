#pragma once

#ifndef _DECLARATION_CONTEXT_
#define _DECLARATION_H_

#include <string>
#include <list>
#include <map>
#include <memory>
#include <iterator>
#include "decl_context.h"
#include "type.h"

namespace C1
{
	namespace AST
	{
		class DeclContext;
		class Initializer;
		class CompoundStmt;
		class Stmt;
		class Declarator;

		template <typename DeclarationType>
		class Redeclarable
		{
		public:
			static const bool redeclarable = true;

			const DeclarationType* prev() const { return m_pPrev; }
			DeclarationType* prev() { return m_pPrev; }
			const DeclarationType* next() const { return m_pNext; }
			DeclarationType* next() { return m_pNext; }

			const DeclarationType* first() const
			{
				DeclarationType* decl = this;
				while (decl->prev()) decl = decl->prev();
				return decl;
			}
			DeclarationType* first()
			{
				DeclarationType* decl = this;
				while (decl->prev()) decl = decl->prev();
				return decl;
			}
			const DeclarationType* last() const
			{
				DeclarationType* decl = this;
				while (decl->next()) decl = decl->next();
				return decl;
			}
			DeclarationType* last()
			{
				DeclarationType* decl = this;
				while (decl->next()) decl = decl->next();
				return decl;
			}
			DeclarationType* add_last(DeclarationType* new_decl)
			{
				auto decl = last();
				decl->m_pNext = new_decl;
				new_decl->m_pPrev = decl;
			}
			DeclarationType* add_first(DeclarationType* new_decl)
			{
				auto decl = first();
				decl->m_pPrev = new_decl;
				new_decl->m_pNext = decl;
			}

			size_t redefination_count() const
			{
				size_t count = 1;
				DeclarationType* decl = this;
				while (decl->next()) {
					decl = decl->next();
					count++;
				}
				decl = this;
				while (decl->prev()) {
					decl = decl->prev();
					count++;
				}
				return count;
			}
		protected:
			DeclarationType *m_pPrev, *m_pNext;
		};

		// Represent an declaration in Semantic , the unit for lookup
		class Declaration
		{
		public:
			enum KindEnum{
				DECL_VARIABLE,
				DECL_FIELD,
				DECL_FUNCTION,
				DECL_TYPE,
				DECL_TYPEDEF,
				DECL_RECORD,
			};

			const KindEnum Kind() const
			{
				return m_kind;
			}

			// get the semantic affiliation of this declaration
			const DeclContext* Affiliation() const;
			DeclContext* Affiliation();
			void SetAffiliation(DeclContext*);

			virtual ~Declaration() = 0;

			static bool CheckCompatible(Declaration* lsh, Declaration* rhs);
		protected:
			void SetKind(KindEnum kind)
			{
				m_kind = kind;
			}

			KindEnum m_kind;
			DeclContext* m_Affiliation;
		};

		class NamedDeclaration : public Declaration
		{
		public:
			const std::string& Name() const
			{
				return m_name;
			}
			void SetName(const std::string& name)
			{
				m_name = name;
			}
		protected:
			std::string m_name;

		};

		class ValueDeclaration : public NamedDeclaration
		{
		public:
			const Type* DeclType() const;
			bool IsWeak() const;
			StorageClassSpecifierEnum StorageClassSpecifier() const;
		};

		// represent a local/global variable
		class VariableDeclaration : public ValueDeclaration//, Redeclarable<VariableDeclaration>
		{
		public:
			VariableDeclaration(StorageClassSpecifierEnum storage_class_specifier, QualType qual_type, Declarator* p_declarator);
			const Initializer* InitializeExpr() const;
		};

		// Represent a struct field
		class FieldDeclaration : public ValueDeclaration//, Redeclarable<FieldDeclaration>
		{
		public:
			static const size_t	Offset_Auto = -1;

			const Type* ParentRecordType() const;
			const int AccessModifier() const;
			const size_t Offset() const;
		};

		// Represent a function declaration
		class FunctionDeclaration;


		class TypeDeclaration : public NamedDeclaration
		{
		public:
			const Type* DeclType() const;
			Type* DeclType() ;
			void SetDeclType(Type* type);
		protected:
			Type* m_type;
		};

		class TypedefDeclaration : public TypeDeclaration//, Redeclarable<TypedefDeclaration>
		{
		public:
			TypedefDeclaration(QualType qual_type, Declarator* p_declarator);
		};


		//class RecordDeclaration : public TypeDeclaration
		//{
		//public:
		//	RecordDeclaration(int record_keyword ,const std::string &name,std::list<Declaration*>* fiel_decl_list);
		//};
	}

}

#endif // !_DECLARATION_CONTEXT_

