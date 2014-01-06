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
		class Declarator;

		template <typename T>
		class Redeclarable
		{
		public:
			typedef T DeclarationType;
			class iterator
			{
			public:
				typedef std::bidirectional_iterator_tag iterator_category;
				typedef T value_type;        // Type of element
				//  iterator 'points
				//  to'.
				typedef size_t	size_type;         // Container index
				//  type.
				typedef size_t	difference_type;  // Container difference
				//  type.
				typedef value_type	*pointer;         // Type of a pointer to
				//  element.
				typedef const value_type *const_pointer;     // As above, but const.
				typedef value_type	&reference;         // Type of a reference
				//  to element.
				typedef const value_type &const_reference;   // As above, but const.

				iterator& operator++()
				{
					m_pEntity = m_pEntity->NextDecl();
				}
				iterator& operator--()
				{
					m_pEntity = m_pEntity->PrevDecl();
				}
				iterator& operator++(int)
				{
					m_pEntity = m_pEntity->NextDecl();
				}
				iterator& operator--(int)
				{
					m_pEntity = m_pEntity->PrevDecl();
				}
			private:
				T* m_pEntity;
			};


			bool HasNextDecl() const;
			bool HasPrevDecl() const;
			const DeclarationType* PrevDecl() const;
			DeclarationType* PrevDecl();
			const DeclarationType* NextDecl() const;
			DeclarationType* NextDecl();

			const DeclarationType* FirstDecl() const;
			DeclarationType* FirstDecl();
			const DeclarationType* LastDecl() const;
			DeclarationType* LastDecl();
		protected:
			DeclarationType *m_pPrev, *m_pNext;
		};

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
			const std::string& Name() const
			{
				return m_name;
			}
			void SetName(const std::string& name)
			{
				m_name = name;
			}

			const KindEnum Kind() const
			{
				return m_kind;
			}
			void SetKind(KindEnum kind)
			{
				m_kind = kind;
			}

			const DeclContext* DeclarationContext() const;

			virtual ~Declaration() = 0;
		protected:
			KindEnum m_kind;
			std::string m_name;
		};

		class NamedDeclaration : public Declaration
		{
		public:

		};

		class ValueDeclaration : public Declaration
		{
		public:
			const Type* DeclType() const;
			bool IsWeak() const;
			StorageClassSpecifierEnum StorageClassSpecifier() const;
		};

		class ParameterDeclaration : public Declaration
		{
		public:
			ParameterDeclaration(QualType qual_type, Declarator* declarator);
			const Type* DeclType() const;
			bool IsNamed() const;
			//const Expr* DefaultValue() const;
		};

		class VariableDeclaration : public ValueDeclaration, Redeclarable<VariableDeclaration>
		{
		public:
			VariableDeclaration(StorageClassSpecifierEnum storage_class_specifier, QualType qual_type, Declarator* p_declarator);
			const Initializer* InitializeExpr() const;
		};

		class FieldDeclaration : public ValueDeclaration, Redeclarable<FieldDeclaration>
		{
		public:
			static const size_t	Offset_Auto = -1;

			const Type* ParentRecordType() const;
			const int AccessModifier() const;
			const size_t Offset() const;
		};

		class FunctionDeclaration : public ValueDeclaration, Redeclarable<FunctionDeclaration>
		{
		public:
			FunctionDeclaration(StorageClassSpecifierEnum storage_class_specifier, QualType qual_type, Declarator* p_declarator);
			const QualType ReturnType() const;
		};


		class TypeDeclaration : public Declaration
		{
		public:
			const Type* DeclType() const;
			Type* DeclType() ;
			void SetDeclType(Type* type);
		protected:
			Type* m_type;
		};

		class TypedefDeclaration : public TypeDeclaration, Redeclarable<TypedefDeclaration>
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

