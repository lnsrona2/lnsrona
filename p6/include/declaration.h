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
		class Node;
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
				DECL_UNKNOWN,
				DECL_VALUE,
				DECL_VARIABLE,
				DECL_FIELD,
				DECL_PARAMETER,
				DECL_FUNCTION,
				DECL_TYPE,
				DECL_TYPEDEF,
				DECL_RECORD,
				DECL_STRUCT,
			};

			const KindEnum Kind() const
			{ return m_Kind; }

			// get the semantic affiliation of this declaration
			const DeclContext* Affiliation() const { return m_Affiliation; }
			DeclContext* Affiliation() { return m_Affiliation; }
			void SetAffiliation(DeclContext* affiliation) { m_Affiliation = affiliation; }

			// get the node which contains this declaration literally
			// return self for non-compound-declarations like ParameterDeclaration, FunctionDeclaration, StructDeclaration
			const Node* SourceNode() const { return m_Source; }
			Node* SourceNode() { return m_Source; }
			void SetSourceNode(Node* source) { m_Source = source; }


			virtual ~Declaration() = 0;

			static bool CheckCompatible(Declaration* lsh, Declaration* rhs);
		protected:
			Declaration(KindEnum kind = DECL_UNKNOWN, DeclContext* affiliation = nullptr)
				: m_Affiliation(nullptr), m_Kind(kind)
			{}

			void SetKind(KindEnum kind)
			{
				m_Kind = kind;
			}

			Node*	m_Source;
			KindEnum m_Kind;
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
			NamedDeclaration()
				: m_name("%anonymous")
			{}
			NamedDeclaration(const std::string& name)
				: m_name(name)
			{}

			std::string m_name;
		};

		class ValueDeclaration : public NamedDeclaration
		{
		public:
			const QualType DeclType() const
				{ return m_decltype; }
			QualType DeclType()
				{ return m_decltype; }
			void SetDeclType(QualType qual_type)
				{ m_decltype = qual_type; }

			StorageClassSpecifierEnum StorageClassSpecifier() const 
				{ return m_storage_specifier; }
		protected:
			ValueDeclaration()
			{}
			ValueDeclaration(StorageClassSpecifierEnum scs, QualType decl_type, const std::string& name)
				: NamedDeclaration(name), m_storage_specifier(scs), m_decltype(decl_type)
			{}

			StorageClassSpecifierEnum	m_storage_specifier;
			QualType					m_decltype;
		};

		// represent a local/global variable
		// A variable's declaration may not directly bind to an location since the exist of coumpound declaration
		class VariableDeclaration : public ValueDeclaration//, Redeclarable<VariableDeclaration>
		{
		public:
			VariableDeclaration(StorageClassSpecifierEnum storage_class_specifier, QualType qual_type, Declarator* p_declarator);
			const Initializer* InitializeExpr() const;

		protected:
			VariableDeclaration()
			{
				SetKind(DECL_VARIABLE);
			}
		};

		// Represent a struct field
		// A field's declaration may not directly bind to an location since the exist of coumpound declaration
		class FieldDeclaration : public ValueDeclaration//, Redeclarable<FieldDeclaration>
		{
		public:
			FieldDeclaration(QualType qual_type, Declarator* declarator);
			static const size_t	Offset_Auto = -1;

			const Type* ParentRecordType() const;
			const int AccessModifier() const;
			const size_t Offset() const;

		protected:
			FieldDeclaration()
			{
				SetKind(DECL_FIELD);
			}
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

		protected:
			TypedefDeclaration()
			{
				SetKind(DECL_TYPEDEF);
			}
		};


		//class RecordDeclaration : public TypeDeclaration
		//{
		//public:
		//	RecordDeclaration(int record_keyword ,const std::string &name);
		//};
	}

}

#endif // !_DECLARATION_CONTEXT_

